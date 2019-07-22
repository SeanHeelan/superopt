#include "prog.h"

using namespace std;

// TODO: find canonical way to invoke one constructor from another
prog::prog(const prog& other) {
  prog_length = other.prog_length;
  inst_list = (inst*)malloc(prog_length * sizeof(inst));
  for (int i=0; i < other.prog_length; i++) {
    inst_list[i] = other.inst_list[i];
  }
}

prog::prog(inst* instructions, int _prog_length) {
  prog_length = _prog_length;
  inst_list = (inst*)malloc(prog_length * sizeof(inst));
  for (int i=0; i < prog_length; i++) {
    inst_list[i] = instructions[i];
  }
}

prog::prog() {
  inst_list = NULL;
  prog_length = 0;
}

prog::~prog() {
  free(inst_list);
}

void prog::print() {
  print_program(inst_list, prog_length);
}

bool prog::operator==(const prog &x) const {
  if (prog_length != x.prog_length) return false;
  for (int i=0; i < prog_length; i++) {
    if (! (inst_list[i] == x.inst_list[i])) return false;
  }
  return true;
}

size_t progHash::operator()(const prog &x) const {
  size_t hval = hash<int>()(x.prog_length);
  for (int i=0; i < x.prog_length; i++) {
    hval = hval ^ (instHash()(x.inst_list[i]) << (i % 4));
  }
  return hval;
}
