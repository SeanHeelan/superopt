#include <iostream>
#include "../../src/utils.h"
#include "../../src/isa/ebpf/inst.h"
#include "validator.h"

using namespace z3;

void test1() {
  std::cout << "test 1:" << endl;
  inst instructions1[9] = {inst(MOV32XC, 0, -1),         /* r0 = 0x00000000ffffffff */
                           inst(ADD64XC, 0, 0x1),        /* r0 = 0x0000000100000000 */
                           inst(MOV64XC, 1, 0x0),        /* r1 = 0 */
                           inst(JEQXC, 0, 0, 4),         /* if r0 == 0, ret r0 = 0x100000000 */
                           inst(MOV64XC, 0, -1),         /* else r0 = 0xffffffffffffffff */
                           inst(JEQXC, 0, 0xffffffff, 1),/* if r0 == -1, ret r0 = 0 */
                           inst(EXIT),                   /* else ret r0 = 0xffffffffffffffff */
                           inst(MOV64XC, 0, 0),
                           inst(EXIT),
                          };

  inst instructions2[9] = {inst(MOV32XC, 0, -1),         /* r0 = 0x00000000ffffffff */
                           inst(ADD64XC, 0, 0x1),        /* r0 = 0x0000000100000000 */
                           inst(MOV64XC, 1, 0x0),        /* r1 = 0 */
                           inst(JEQXC, 0, 0, 4),         /* if r0 == 0, ret r0 = 0x100000000 */
                           inst(MOV64XC, 0, -1),         /* else r0 = 0xffffffffffffffff */
                           inst(JEQXC, 0, 0xffffffff, 1),/* if r0 == -1, ret r0 = 0 */
                           inst(JA, 1),                  /* else ret r0 = 0xffffffffffffffff */
                           inst(MOV64XC, 0, 0),
                           inst(EXIT),
                          };

  validator vld(instructions1, 9);
  print_test_res(vld.is_equal_to(instructions1, 9), "instructions1 == instructions1");
  print_test_res(vld.is_equal_to(instructions2, 9), "instructions1 == instructions2");

  // output = L32(input)
  inst instructions3[2] = {inst(MOV32XY, 0, 1),
                           inst(EXIT),
                          };

  inst instructions4[3] = {inst(STXW, 10, -4, 1),
                           inst(LDXW, 0, 10, -4),
                           inst(EXIT),
                          };
  vld.set_orig(instructions3, 2);
  print_test_res(vld.is_equal_to(instructions4, 3), "instructions3 == instructions4");

  inst instructions5[3] = {inst(STXDW, 10, -8, 1),
                           inst(LDXDW, 0, 10, -8),
                           inst(EXIT),
                          };
  inst instructions6[9] = {inst(STXW, 10, -8, 1),
                           inst(RSH64XC, 1, 32),
                           inst(STXH, 10, -4, 1),
                           inst(RSH64XC, 1, 16),
                           inst(STXB, 10, -2, 1),
                           inst(RSH64XC, 1, 8),
                           inst(STXB, 10, -1, 1),
                           inst(LDXDW, 0, 10, -8),
                           inst(EXIT),
                          };
  vld.set_orig(instructions5, 3);
  print_test_res(vld.is_equal_to(instructions6, 9), "instructions5 == instructions6");
}

void test2() {
  // check branch with ld/st
  inst p1[6] = {inst(STXB, 10, -1, 1),
                inst(JEQXC, 1, 0x12, 2),
                inst(MOV64XC, 1, 0x12),
                inst(STXB, 10, -1, 1),
                inst(LDXB, 0, 10, -1),
                inst(EXIT),
               };
  inst p2[4] = {inst(MOV64XC, 1, 0x12),
                inst(STXB, 10, -1, 1),
                inst(LDXB, 0, 10, -1),
                inst(EXIT),
               };
  vld.set_orig(p1, 6);
  print_test_res(vld.is_equal_to(p2, 4), "p1 == p2");
}

int main() {
  test1();
  test2();

  return 0;
}
