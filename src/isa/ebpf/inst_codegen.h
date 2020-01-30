#pragma once

#include "z3++.h"
#include "../../../src/utils.h"
#include "../../../src/verify/smt_var.h"

using namespace std;

// return (b = op a)
int64_t compute_mov(int64_t a, int64_t b = 0);
int64_t compute_mov32(int32_t a, int64_t b = 0);
int64_t compute_le16(int64_t a, int64_t b = 0);
int64_t compute_le32(int64_t a, int64_t b = 0);
int64_t compute_le64(int64_t a, int64_t b = 0);
int64_t compute_be16(int64_t a, int64_t b = 0);
int64_t compute_be32(int64_t a, int64_t b = 0);
int64_t compute_be64(int64_t a, int64_t b = 0);

// return (c = a op b)
int64_t compute_add(int64_t a, int64_t b, int64_t c = 0);
int64_t compute_add32(int32_t a, int32_t b, int64_t c = 0);
int64_t compute_lsh(int64_t a, int64_t b, int64_t c = 0);
int64_t compute_lsh32(int32_t a, int32_t b, int64_t c = 0);
int64_t compute_rsh(int64_t a, int64_t b, int64_t c = 0);
int64_t compute_rsh32(int32_t a, int32_t b, int64_t c = 0);
int64_t compute_arsh(int64_t a, int64_t b, int64_t c = 0);
int64_t compute_arsh32(int32_t a, int32_t b, int64_t c = 0);

// return (b == op a)
z3::expr predicate_mov(z3::expr a, z3::expr b);
z3::expr predicate_mov32(z3::expr a, z3::expr b);
z3::expr predicate_le16(z3::expr a, z3::expr b);
z3::expr predicate_le32(z3::expr a, z3::expr b);
z3::expr predicate_le64(z3::expr a, z3::expr b);
z3::expr predicate_be16(z3::expr a, z3::expr b);
z3::expr predicate_be32(z3::expr a, z3::expr b);
z3::expr predicate_be64(z3::expr a, z3::expr b);
// return (c == a op b)
z3::expr predicate_add(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_add32(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_lsh(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_lsh32(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_rsh(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_rsh32(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_arsh(z3::expr a, z3::expr b, z3::expr c);
z3::expr predicate_arsh32(z3::expr a, z3::expr b, z3::expr c);
