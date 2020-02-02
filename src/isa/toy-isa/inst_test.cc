#include <iostream>
#include <bitset>
#include "../../../src/utils.h"
#include "inst.h"

/* r0 contains the input */
inst_t instructions[6] = {inst_t(MOVXC, 2, 4),  /* mov r2, 4  */
                          inst_t(ADDXY, 0, 2),  /* add r0, r2 */
                          inst_t(MOVXC, 3, 15),  /* mov r3, 15  */
                          inst_t(JMPGT, 0, 3, 1),  /* if r0 <= r3: */
                          inst_t(RETX, 3),      /* ret r3 */
                          inst_t(RETX, 0),      /* else ret r0 */
                         };

inst_t instructions2[4] = {inst_t(MOVXC, 2, 4),     /* mov r2, 4 */
                           inst_t(ADDXY, 0, 2),     /* add r0, r2 */
                           inst_t(MAXC, 0, 15),     /* max r0, 15 */
                           inst_t(RETX, 0),         /* ret r0 */
                          };

inst_t instructions3[2] = {inst_t(NOP), /* test no-op */
                           inst_t(RETX, 0), /* ret r0 */
                          };

/* test unconditional jmp */
inst_t instructions4[3] = {inst_t(JMP, 1),
                           inst_t(ADDXY, 0, 0),
                           inst_t(RETX, 0),
                          };

void test1(int input) {
  prog_state_t ps;
  cout << "Test 1: full interpretation check" << endl;
  vector<inst*> instptr_list(6);
  instructions->convert_to_pointers(instptr_list, instructions);
  print_test_res(instptr_list[0]->interpret(instptr_list, ps, input) == max(input + 4, 15),
                 "interpret program 1");

  instptr_list.resize(4);
  instructions2->convert_to_pointers(instptr_list, instructions2);
  print_test_res(instptr_list[0]->interpret(instptr_list, ps, input) == max(input + 4, 15),
                 "interpret program 2");

  instptr_list.resize(2);
  instructions3->convert_to_pointers(instptr_list, instructions3);
  print_test_res(instptr_list[0]->interpret(instptr_list, ps, input) == input,
                 "interpret program 3");

  instptr_list.resize(3);
  instructions4->convert_to_pointers(instptr_list, instructions4);
  print_test_res(instptr_list[0]->interpret(instptr_list, ps, input) == input,
                 "interpret program 4");
}

void test2() {
  cout << "Test 2" << endl;
  inst_t x = inst_t(MOVXC, 2, 4);
  inst_t y = inst_t(MOVXC, 2, 4);
  inst_t z = inst_t(MOVXC, 2, 3);
  inst_t w = inst_t(RETX, 3);

  cout << "Instruction operator== check" << endl;
  print_test_res((x == y) == true, "operator== 1");
  print_test_res((inst_t(RETX, 3) == inst_t(RETC, 3)) == false, "operator== 2");
  print_test_res((inst_t(RETX, 3) == inst_t(RETX, 2)) == false, "operator== 3");
  print_test_res((inst_t(RETX, 3) == inst_t(RETX, 3)) == true, "operator== 4");

  cout << "Instruction hash value check" << endl;
  print_test_res(instHash()(x) == 22, "hash value 1");
  print_test_res(instHash()(y) == 22, "hash value 2");
  print_test_res(instHash()(z) == 10, "hash value 3");
  print_test_res(instHash()(w) == 5, "hash value 4");
}

void test3() {
  cout << "Test 3" << endl;
  string expected_bv_str = string("00010000100010000000") +
                           string("00001000000001000000") +
                           string("00010000110111100000") +
                           string("00111000000001100001") +
                           string("00011000110000000000") +
                           string("00011000000000000000");
  string bv_str = "";
  for (int i = 0; i < 6; i++) {
    inst_t x = instructions[i];
    vector<int> abs_bv;
    x.to_abs_bv(abs_bv);
    for (int j = 0; j < abs_bv.size(); j++) {
      bv_str += bitset<OP_NUM_BITS>(abs_bv[j]).to_string();
    }
  }
  print_test_res(bv_str == expected_bv_str, "inst_to_abs_bv");
}

int main(int argc, char *argv[]) {
  /* Add the notion of program input */
  int input = 10;
  if (argc > 1) {
    input = atoi(argv[1]);
  }

  test1(input);
  test2();
  test3();

  return 0;
}
