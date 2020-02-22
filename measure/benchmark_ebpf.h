#pragma once

#include <vector>
#include "../src/isa/ebpf/inst.h"

using namespace std;

// instruction_list set
#undef N
#define N MAX_PROG_LEN

#undef NUM_ORIG
#define NUM_ORIG 1
extern inst bm0[N];

extern inst bm_opti00[N];

void init_benchmarks(inst** bm, vector<inst*> &bm_optis_orig, int bm_id);