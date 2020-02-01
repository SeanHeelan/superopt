#include <iostream>
#include <cassert>
#include "inst.h"

using namespace std;

#define DSTREG(inst_var) (inst_var)._args[0]
#define SRCREG(inst_var) (inst_var)._args[1]
#define IMM1VAL(inst_var) (inst_var)._args[0]
#define IMM2VAL(inst_var) (inst_var)._args[1]
#define UNCOND_OFFVAL(inst_var) (inst_var)._args[0]
#define COND_OFFVAL(inst_var) (inst_var)._args[2]

#define IMM1VAL32(inst_var) (int32_t)(IMM1VAL(inst_var))
#define IMM2VAL32(inst_var) (int32_t)(IMM2VAL(inst_var))
#define UNCOND_OFFVAL16(inst_var) (int16_t)(UNCOND_OFFVAL(inst_var))
#define COND_OFFVAL16(inst_var) (int16_t)(COND_OFFVAL(inst_var))

constexpr int ebpf::num_operands[NUM_INSTR];
constexpr int ebpf::insn_num_regs[NUM_INSTR];
constexpr int ebpf::opcode_type[NUM_INSTR];
constexpr int ebpf::optable[NUM_INSTR];

string ebpf_inst::opcode_to_str(int opcode) const {
  switch (opcode) {
    case ebpf::NOP: return "nop";
    case ebpf::ADD64XC: return "addxc";
    case ebpf::ADD64XY: return "addxy";
    case ebpf::LSH64XC: return "lshxc";
    case ebpf::LSH64XY: return "lshxy";
    case ebpf::RSH64XC: return "rshxc";
    case ebpf::RSH64XY: return "rshxy";
    case ebpf::MOV64XC: return "movxc";
    case ebpf::MOV64XY: return "movxy";
    case ebpf::ARSH64XC: return "arshxc";
    case ebpf::ARSH64XY: return "arshxy";
    case ebpf::ADD32XC: return "add32xc";
    case ebpf::ADD32XY: return "add32xy";
    case ebpf::LSH32XC: return "lsh32xc";
    case ebpf::LSH32XY: return "lsh32xy";
    case ebpf::RSH32XC: return "rsh32xc";
    case ebpf::RSH32XY: return "rsh32xy";
    case ebpf::MOV32XC: return "mov32xc";
    case ebpf::MOV32XY: return "mov32xy";
    case ebpf::ARSH32XC: return "arsh32xc";
    case ebpf::ARSH32XY: return "arsh32xy";
    case ebpf::LE16: return "le16";
    case ebpf::LE32: return "le32";
    case ebpf::LE64: return "le64";
    case ebpf::BE16: return "be16";
    case ebpf::BE32: return "be32";
    case ebpf::BE64: return "be64";
    case ebpf::JA: return "ja";
    case ebpf::JEQXC: return "jeqxc";
    case ebpf::JEQXY: return "jeqxy";
    case ebpf::JGTXC: return "jgtxc";
    case ebpf::JGTXY: return "jgtxy";
    case ebpf::JSGTXC: return "jsgtxc";
    case ebpf::JSGTXY: return "jsgtxy";
    case ebpf::EXIT: return "exit";
    default: return "unknown opcode";
  }
}

ebpf_inst& ebpf_inst::operator=(const inst &rhs) {
  _opcode = rhs._opcode;
  _args[0] = rhs._args[0];
  _args[1] = rhs._args[1];
  _args[2] = rhs._args[2];
  return *this;
}

// For jmp opcode, it can only jump forward
int32_t ebpf_inst::get_max_operand_val(int op_index, int inst_index) const {
  // max valufor each operand type
  int32_t max_val[6] = {
    [ebpf::OP_UNUSED] = 0,
    [ebpf::OP_REG] = ebpf::NUM_REGS,
    [ebpf::OP_IMM] = ebpf::MAX_IMM,
    [ebpf::OP_OFF] = min((int32_t)ebpf::MAX_OFF, ebpf::MAX_PROG_LEN - inst_index - 1),
    [ebpf::OP_IMM_SH32] = ebpf::MAX_IMM_SH32,
    [ebpf::OP_IMM_SH64] = ebpf::MAX_IMM_SH64,
  };
  return max_val[EBPF_OPTYPE(_opcode, op_index)];
}

void ebpf_inst::make_insts(vector<inst*> &insts, const vector<inst*> &other) const {
  int num_inst = insts.size();
  ebpf_inst* new_insts = new ebpf_inst[num_inst];
  for (int i = 0; i < num_inst; i++) {
    new_insts[i] = *other[i];
  }
  for (int i = 0; i < num_inst; i++) {
    insts[i] = &new_insts[i];
  }
}

void ebpf_inst::make_insts(vector<inst*> &insts, const inst* instruction) const {
  int num_inst = insts.size();
  ebpf_inst* new_insts = new ebpf_inst[num_inst];
  for (int i = 0; i < num_inst; i++) {
    new_insts[i] = instruction[i];
  }
  for (int i = 0; i < num_inst; i++) {
    insts[i] = &new_insts[i];
  }
}

void ebpf_inst::clear_insts() {
  delete []this;
}

int ebpf_inst::get_jmp_dis() const {
  switch (get_opcode_type()) {
    case (OP_UNCOND_JMP): return UNCOND_OFFVAL16(*this);
    case (OP_COND_JMP): return COND_OFFVAL16(*this);
    default: cout << "Error: opcode is not jmp" << endl; return 0;
  }
}

void ebpf_inst::insert_jmp_opcodes(unordered_set<int>& jmp_set) const {
  jmp_set.insert(ebpf::JA);
  jmp_set.insert(ebpf::JEQXC);
  jmp_set.insert(ebpf::JEQXY);
  jmp_set.insert(ebpf::JGTXC);
  jmp_set.insert(ebpf::JGTXY);
  jmp_set.insert(ebpf::JSGTXC);
  jmp_set.insert(ebpf::JSGTXY);
}

int ebpf_inst::inst_output_opcode_type() const {
  switch (_opcode) {
    case ebpf::EXIT: return RET_X;
    default: /* cout << "Error: opcode is not EXIT" << endl; */ return RET_X;
  }
}

int ebpf_inst::inst_output() const {
  switch (_opcode) {
    case ebpf::EXIT: return 0;
    default: /* cout << "Error: opcode is not EXIT" << endl; */ return 0;
  }
}

bool ebpf_inst::is_real_inst() const {
  if (_opcode == ebpf::NOP) return false;
  return true;
}

void ebpf_inst::set_as_nop_inst() {
  _opcode = ebpf::NOP;
  _args[0] = 0;
  _args[1] = 0;
  _args[2] = 0;
}

int64_t ebpf_inst::interpret(const vector<inst*> &insts, prog_state &ps, int64_t input) const {
// type: int64_t
#define DST ps.regs[DSTREG(*insts[insn])]
#define SRC ps.regs[SRCREG(*insts[insn])]
#define IMM1 (int64_t)IMM1VAL32(*insts[insn])
#define IMM2 (int64_t)IMM2VAL32(*insts[insn])
#define SRC_L6 L6(SRC)
#define SRC_L5 L5(SRC)
#define UNCOND_OFF (int64_t)UNCOND_OFFVAL16(*insts[insn])
#define COND_OFF (int64_t)COND_OFFVAL16(*insts[insn])

// type: uint64_t
#define UDST (uint64_t)DST
#define USRC (uint64_t)SRC
#define UIMM1 (uint64_t)IMM1
#define UIMM2 (uint64_t)IMM2

#define ALU_UNARY(OPCODE, OP, OPERAND)                             \
  INSN_##OPCODE:                                                   \
    DST = compute_##OP(OPERAND);                                   \
    CONT;

#define ALU_BINARY(OPCODE, OP, OPERAND1, OPERAND2)                 \
  INSN_##OPCODE:                                                   \
    DST = compute_##OP(OPERAND1, OPERAND2);                        \
    CONT;

#define BYTESWAP(OPCODE, OP)                                       \
  INSN_##OPCODE:                                                   \
    DST = compute_##OP(DST);                                       \
    CONT;

#define COND_JMP(OPCODE, OP, OPERAND1, OPERAND2)                   \
  INSN_##OPCODE:                                                   \
    if (OPERAND1 OP OPERAND2)                                      \
      insn += COND_OFF;                                            \
  CONT;

  int insn = 0;
  int length = insts.size();
  ps.clear();
  ps.regs[1] = input;

  static void *jumptable[ebpf::NUM_INSTR] = {
    [ebpf::NOP]      = && INSN_NOP,
    [ebpf::ADD64XC]  = && INSN_ADD64XC,
    [ebpf::ADD64XY]  = && INSN_ADD64XY,
    [ebpf::LSH64XC]  = && INSN_LSH64XC,
    [ebpf::LSH64XY]  = && INSN_LSH64XY,
    [ebpf::RSH64XC]  = && INSN_RSH64XC,
    [ebpf::RSH64XY]  = && INSN_RSH64XY,
    [ebpf::MOV64XC]  = && INSN_MOV64XC,
    [ebpf::MOV64XY]  = && INSN_MOV64XY,
    [ebpf::ARSH64XC] = && INSN_ARSH64XC,
    [ebpf::ARSH64XY] = && INSN_ARSH64XY,

    [ebpf::ADD32XC]  = && INSN_ADD32XC,
    [ebpf::ADD32XY]  = && INSN_ADD32XY,
    [ebpf::LSH32XC]  = && INSN_LSH32XC,
    [ebpf::LSH32XY]  = && INSN_LSH32XY,
    [ebpf::RSH32XC]  = && INSN_RSH32XC,
    [ebpf::RSH32XY]  = && INSN_RSH32XY,
    [ebpf::MOV32XC]  = && INSN_MOV32XC,
    [ebpf::MOV32XY]  = && INSN_MOV32XY,
    [ebpf::ARSH32XC] = && INSN_ARSH32XC,
    [ebpf::ARSH32XY] = && INSN_ARSH32XY,

    [ebpf::LE16]     = && INSN_LE16,
    [ebpf::LE32]     = && INSN_LE32,
    [ebpf::LE64]     = && INSN_LE64,
    [ebpf::BE16]     = && INSN_BE16,
    [ebpf::BE32]     = && INSN_BE32,
    [ebpf::BE64]     = && INSN_BE64,

    [ebpf::JA]       = && INSN_JA,
    [ebpf::JEQXC]    = && INSN_JEQXC,
    [ebpf::JEQXY]    = && INSN_JEQXY,
    [ebpf::JGTXC]    = && INSN_JGTXC,
    [ebpf::JGTXY]    = && INSN_JGTXY,
    [ebpf::JSGTXC]   = && INSN_JSGTXC,
    [ebpf::JSGTXY]   = && INSN_JSGTXY,
    [ebpf::EXIT]     = && INSN_EXIT,
  };

#define CONT {                                                     \
      insn++;                                                      \
      if (insn < length) {                                         \
        goto *jumptable[insts[insn]->_opcode];                     \
      } else goto out;                                             \
  }

select_insn:
  goto *jumptable[insts[insn]->_opcode];

INSN_NOP:
  CONT;

  ALU_UNARY(MOV64XC, mov, IMM2)
  ALU_UNARY(MOV64XY, mov, SRC)
  ALU_BINARY(ADD64XC, add, DST, IMM2)
  ALU_BINARY(ADD64XY, add, DST, SRC)
  ALU_BINARY(LSH64XC, lsh, DST, IMM2)
  ALU_BINARY(LSH64XY, lsh, DST, SRC_L6)
  ALU_BINARY(RSH64XC, rsh, DST, IMM2)
  ALU_BINARY(RSH64XY, rsh, DST, SRC_L6)
  ALU_BINARY(ARSH64XC, arsh, DST, IMM2)
  ALU_BINARY(ARSH64XY, arsh, DST, SRC_L6)

  ALU_UNARY(MOV32XC, mov32, IMM2)
  ALU_UNARY(MOV32XY, mov32, SRC)
  ALU_BINARY(ADD32XC, add32, DST, IMM2)
  ALU_BINARY(ADD32XY, add32, DST, SRC)
  ALU_BINARY(LSH32XC, lsh32, DST, IMM2)
  ALU_BINARY(LSH32XY, lsh32, DST, SRC_L5)
  ALU_BINARY(RSH32XC, rsh32, DST, IMM2)
  ALU_BINARY(RSH32XY, rsh32, DST, SRC_L5)
  ALU_BINARY(ARSH32XC, arsh32, DST, IMM2)
  ALU_BINARY(ARSH32XY, arsh32, DST, SRC_L5)

  BYTESWAP(LE16, le16)
  BYTESWAP(LE32, le32)
  BYTESWAP(LE64, le64)
  BYTESWAP(BE16, be16)
  BYTESWAP(BE32, be32)
  BYTESWAP(BE64, be64)

INSN_JA:
  insn += UNCOND_OFF;
  CONT;

  COND_JMP(JEQXC, ==, DST, IMM2)
  COND_JMP(JEQXY, ==, DST, SRC)
  COND_JMP(JGTXC, >, UDST, UIMM2)
  COND_JMP(JGTXY, >, UDST, USRC)
  COND_JMP(JSGTXC, >, DST, IMM2)
  COND_JMP(JSGTXY, >, DST, SRC)

INSN_EXIT:
  return ps.regs[0];

error_label:
  cout << "Error in processing instruction; unknown opcode" << endl;
  return -1;

out:
  //cout << "Error: program terminated without RET; returning R0" << endl;
  return ps.regs[0]; /* return default R0 value */
}

#undef IMM2
// z3 64-bit bv
#define NEWDST newDst
#define CURDST curDst
#define CURSRC curSrc
#define IMM2 to_expr(imm2)
#define CURSRC_L6 (CURSRC & to_expr((int64_t)0x3f))
#define CURSRC_L5 (CURSRC & to_expr((int64_t)0x1f))

z3::expr ebpf_inst::smt_inst(smt_var& sv) const {
  // check whether opcode is valid. If invalid, curDst cannot be updated to get newDst
  // If opcode is valid, then define curDst, curSrc, imm2 and newDst
  if (get_opcode_type() != OP_OTHERS) return string_to_expr("false");
  // Get curDst, curSrc, imm2 and newDst at the begining to avoid using switch case to
  // get some of these values for different opcodes. So need to check whether the value is valid.
  // E.g., for curSrc, the range of register is [0, get_num_regs()], for opcode ends up with XC,
  // actually the value is an immediate number whose value may be out of register's range.
  // Should get curDst and curSrc before updating curDst (curSrc may be the same reg as curDst)
  z3::expr curDst = sv.get_cur_reg_var(DSTREG(*this));
  z3::expr curSrc = string_to_expr("false");
  // check whether the value is within the range of regisers, or function get_cur_reg_var() will raise exception
  if (SRCREG(*this) < get_num_regs() && SRCREG(*this) >= 0) {
    curSrc = sv.get_cur_reg_var(SRCREG(*this));
  }
  z3::expr newDst = sv.update_reg_var(DSTREG(*this));
  int64_t imm2 = (int64_t)IMM2VAL32(*this);

  switch (_opcode) {
    case ebpf::ADD64XC: return predicate_add(CURDST, IMM2, NEWDST);
    case ebpf::ADD64XY: return predicate_add(CURDST, CURSRC, NEWDST);
    case ebpf::LSH64XC: return predicate_lsh(CURDST, IMM2, NEWDST);
    case ebpf::LSH64XY: return predicate_lsh(CURDST, CURSRC_L6, NEWDST);
    case ebpf::RSH64XC: return predicate_rsh(CURDST, IMM2, NEWDST);
    case ebpf::RSH64XY: return predicate_rsh(CURDST, CURSRC_L6, NEWDST);
    case ebpf::MOV64XC: return predicate_mov(IMM2, NEWDST);
    case ebpf::MOV64XY: return predicate_mov(CURSRC, NEWDST);
    case ebpf::ARSH64XC: return predicate_arsh(CURDST, IMM2, NEWDST);
    case ebpf::ARSH64XY: return predicate_arsh(CURDST, CURSRC_L6, NEWDST);
    case ebpf::ADD32XC: return predicate_add32(CURDST, IMM2, NEWDST);
    case ebpf::ADD32XY: return predicate_add32(CURDST, CURSRC, NEWDST);
    case ebpf::LSH32XC: return predicate_lsh32(CURDST, IMM2, NEWDST);
    case ebpf::LSH32XY: return predicate_lsh32(CURDST, CURSRC_L5, NEWDST);
    case ebpf::RSH32XC: return predicate_rsh32(CURDST, IMM2, NEWDST);
    case ebpf::RSH32XY: return predicate_rsh32(CURDST, CURSRC_L5, NEWDST);
    case ebpf::MOV32XC: return predicate_mov32(IMM2, NEWDST);
    case ebpf::MOV32XY: return predicate_mov32(CURSRC, NEWDST);
    case ebpf::ARSH32XC: return predicate_arsh32(CURDST, IMM2, NEWDST);
    case ebpf::ARSH32XY: return predicate_arsh32(CURDST, CURSRC_L5, NEWDST);
    case ebpf::LE16: return predicate_le16(CURDST, NEWDST);
    case ebpf::LE32: return predicate_le32(CURDST, NEWDST);
    case ebpf::LE64: return predicate_le64(CURDST, NEWDST);
    case ebpf::BE16: return predicate_be16(CURDST, NEWDST);
    case ebpf::BE32: return predicate_be32(CURDST, NEWDST);
    case ebpf::BE64: return predicate_be64(CURDST, NEWDST);
    default: return string_to_expr("false");
  }
}

z3::expr ebpf_inst::smt_inst_jmp(smt_var& sv) const {
  // If opcode is valid, then define curDst, curSrc, imm2
  if (get_opcode_type() != OP_COND_JMP) return string_to_expr("false");
  z3::expr curDst = sv.get_cur_reg_var(DSTREG(*this));
  z3::expr curSrc = string_to_expr("false");
  if (SRCREG(*this) < get_num_regs() && SRCREG(*this) >= 0) {
    curSrc = sv.get_cur_reg_var(SRCREG(*this));
  }
  int64_t imm2 = (int64_t)IMM2VAL32(*this);

  switch (_opcode) {
    case ebpf::JEQXC: return (CURDST == IMM2);
    case ebpf::JEQXY: return (CURDST == CURSRC);
    case ebpf::JGTXC: return (ugt(CURDST, IMM2));
    case ebpf::JGTXY: return (ugt(CURDST, CURSRC));
    case ebpf::JSGTXC: return (CURDST > IMM2);
    case ebpf::JSGTXY: return (CURDST > CURSRC);
    default: return string_to_expr("false");
  }
}
