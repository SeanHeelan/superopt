#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include "inst.h"
#include "inout.h"
#include "cost.h"

/* Requires support for advanced bit manipulation (ABM) instructions on the
 * architecture where this program is run. */
unsigned int pop_count_asm(unsigned int x) {
  unsigned int y = x;
  unsigned int z;
  asm ("popcnt %1, %0"
       : "=a" (z)
       : "b" (y)
      );
  return z;
}

cost::cost() {}

cost::~cost() {}

int cost::num_real_instructions(inst* program, int len) {
  int count = 0;
  for (int i = 0; i < len; i++) {
    if (program[i]._opcode != NOP) count++;
  }
  return count;
}

void cost::set_orig(inst* orig, int len) {
  _vld.set_orig(orig, len);
  _num_real_orig = num_real_instructions(orig, len);
}

int cost::error_cost(inst* synth, int len) {
  double total_cost = 0;
  prog_state ps;
  int output1, output2;
  vector<inout> counterexs;
  for (int i = 0; i < _examples._exs.size(); i++) {
    output1 = _examples._exs[i].output;
    output2 = interpret(synth, len, ps, _examples._exs[i].input);
    // cout << "Expected output: " << output1 << " Got output " << output2 << endl;
    // int ex_cost = pop_count_asm(output1 ^ output2);
    int ex_cost = abs(output1 - output2);
    if (!ex_cost) {
      bool is_equal = _vld.is_equal_to(synth, len);
      if (!is_equal) {
        counterexs.push_back(_vld.counterex);
      }
      ex_cost = 1 - (int)is_equal;
    }
    total_cost += ex_cost;
  }
  for (size_t i = 0; i < counterexs.size(); i++) {
    _examples.insert(counterexs[i]);
  }
  return (int)(total_cost);
}

int cost::perf_cost(inst* synth, int len) {
  return MAX_PROG_LEN - _num_real_orig + num_real_instructions(synth, len);
}

double cost::total_prog_cost(inst* synth, int len) {
  double err_cost = error_cost(synth, len);
  cout << "Error cost: " << err_cost << endl;
  double per_cost = perf_cost(synth, len);
  cout << "Perf cost: " << per_cost << endl;
  return (_w_e * err_cost) + (_w_p * per_cost);
}
