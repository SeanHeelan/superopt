using namespace std;

#define NUM_REGS 4
#define MAX_CONST 20

class prog_state {
  int pc = 0; /* Assume only straight line code execution for now */
 public:
  int regs[NUM_REGS] = {}; /* assume only registers for now */
  void print();
};

class inst {
 public:
  int _opcode;
  int _args[3];
  inst(int opcode, int arg1=0, int arg2=0, int arg3=0) {
    _opcode  = opcode;
    _args[0] = arg1;
    _args[1] = arg2;
    _args[2] = arg3;
  }
  void print();
  string opcode_to_str(int);
};

// Operand types for instructions
#define OP_UNUSED 0
#define OP_REG 1
#define OP_IMM 2
#define OP_OFF 3

// Instruction opcodes
#define NOP 0
#define ADDXY 1
#define MOVXC 2
#define RETX 3
#define RETC 4
#define JMPEQ 5
#define JMPGT 6
#define JMPGE 7
#define JMPLT 8
#define JMPLE 9
#define MAXC 10
#define MAXX 11

#define NUM_INSTR 12

/* The definitions below assume a minimum 16-bit integer data type */
#define FSTOP(x) (x)
#define SNDOP(x) (x << 5)
#define TRDOP(x) (x << 10)
#define OPTYPE(opcode, opindex) ((optable[opcode] >> ((opindex) * 5)) & 31)

#define JMP_OPS (FSTOP(OP_REG) | SNDOP(OP_REG) | TRDOP(OP_OFF))
#define UNUSED_OPS (FSTOP(OP_UNUSED) | SNDOP(OP_UNUSED) | TRDOP(OP_UNUSED))

static int optable[256] = {
  [NOP]   = UNUSED_OPS,
  [ADDXY] = FSTOP(OP_REG) | SNDOP(OP_REG) | TRDOP(OP_UNUSED),
  [MOVXC] = FSTOP(OP_REG) | SNDOP(OP_IMM) | TRDOP(OP_UNUSED),
  [RETX]  = FSTOP(OP_REG) | SNDOP(OP_UNUSED) | TRDOP(OP_UNUSED),
  [RETC]  = FSTOP(OP_IMM) | SNDOP(OP_UNUSED) | TRDOP(OP_UNUSED),
  [JMPEQ] = JMP_OPS,
  [JMPGT] = JMP_OPS,
  [JMPGE] = JMP_OPS,
  [JMPLT] = JMP_OPS,
  [JMPLE] = JMP_OPS,
  [MAXC]  = FSTOP(OP_REG) | SNDOP(OP_IMM) | TRDOP(OP_UNUSED),
  [MAXX]  = FSTOP(OP_REG) | SNDOP(OP_REG) | TRDOP(OP_UNUSED),
  [NUM_INSTR ... 255] = UNUSED_OPS,
};

static int num_operands[256] = {
  [NOP]   = 0,
  [ADDXY] = 2,
  [MOVXC] = 2,
  [RETX]  = 1,
  [RETC]  = 1,
  [JMPEQ] = 3,
  [JMPGT] = 3,
  [JMPGE] = 3,
  [JMPLT] = 3,
  [JMPLE] = 3,
  [MAXC]  = 2,
  [MAXX]  = 2,
  [NUM_INSTR ... 255] = 0,
};

void print_program(inst* program, int length);
int interpret(inst* program, int length, prog_state &ps);