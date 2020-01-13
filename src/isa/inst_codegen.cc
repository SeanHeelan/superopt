#include <iostream>
#include "inst_codegen.h"

using namespace std;

#define SWAP16(v) ((((uint16_t)(v) & 0xff00) >> 8) | \
                   (((uint16_t)(v) & 0x00ff) << 8) )
#define SWAP32(v) ((((uint32_t)(v) & 0xff000000) >> 24) | \
                   (((uint32_t)(v) & 0x00ff0000) >> 8 ) | \
                   (((uint32_t)(v) & 0x0000ff00) << 8 ) | \
                   (((uint32_t)(v) & 0x000000ff) << 24) )
#define SWAP64(v) ((((uint64_t)(v) & 0xff00000000000000) >> 56) | \
                   (((uint64_t)(v) & 0x00ff000000000000) >> 40) | \
                   (((uint64_t)(v) & 0x0000ff0000000000) >> 24) | \
                   (((uint64_t)(v) & 0x000000ff00000000) >> 8 ) | \
                   (((uint64_t)(v) & 0x00000000ff000000) << 8 ) | \
                   (((uint64_t)(v) & 0x0000000000ff0000) << 24) | \
                   (((uint64_t)(v) & 0x000000000000ff00) << 40) | \
                   (((uint64_t)(v) & 0x00000000000000ff) << 56) )

#define SWAP_L16(v) (H48(v) | SWAP16(v))
#define SWAP_L32(v) (H32(v) | SWAP32(v))
#define SWAP_L64(v) SWAP64(v)

/* Inputs x, y must be side-effect-free expressions. */
#define NEG_EXPR(x, y) (y GENMODE ~x)
#define MOV_EXPR(x, y) (y GENMODE x)
#define LE16_EXPR(x, y) if (! is_little_endian()) {y GENMODE SWAP_L16(x);} else {y GENMODE x;}
#define LE32_EXPR(x, y) if (! is_little_endian()) {y GENMODE SWAP_L32(x);} else {y GENMODE x;}
#define LE64_EXPR(x, y) if (! is_little_endian()) {y GENMODE SWAP_L64(x);} else {y GENMODE x;}
#define BE16_EXPR(x, y) if (is_little_endian())   {y GENMODE SWAP_L16(x);} else {y GENMODE x;}
#define BE32_EXPR(x, y) if (is_little_endian())   {y GENMODE SWAP_L32(x);} else {y GENMODE x;}
#define BE64_EXPR(x, y) if (is_little_endian())   {y GENMODE SWAP_L64(x);} else {y GENMODE x;}
/* Inputs x, y, z must be side-effect-free expressions. */
#define ADD_EXPR(x, y, z) (z GENMODE x + y)
#define SUB_EXPR(x, y, z) (z GENMODE x - y)
#define MUL_EXPR(x, y, z) (z GENMODE x * y)
#define DIV_EXPR(x, y, z) (z GENMODE x / y)
#define OR_EXPR(x, y, z)  (z GENMODE x | y)
#define AND_EXPR(x, y, z) (z GENMODE x & y)
#define LSH_EXPR(x, y, z) (z GENMODE x << y)
#define RSH_EXPR(x, y, z) (z GENMODE x >> y)
#define MOD_EXPR(x, y, z) (z GENMODE x % y)
#define XOR_EXPR(x, y, z) (z GENMODE x ^ y)

/* Predicate expressions capture instructions like MAX which have different
 * results on a register based on the evaluation of a predicate. */
/* Inputs x, y, z, pred_if, pred_else must be side-effect-free. */
#define PRED_EXPR(x, y, z, pred_if, ret_if, ret_else) ({  \
    IF_PRED_ACTION(pred_if, ret_if, z)                    \
    CONNECTIFELSE                                         \
    ELSE_PRED_ACTION(pred_if, ret_else, z);           \
  })

#define MAX_EXPR(a, b, c) (PRED_EXPR(a, b, c, a > b, a, b))

#undef GENMODE
#define GENMODE =
#undef IF_PRED_ACTION
#define IF_PRED_ACTION(pred, expr, var) if(pred) var GENMODE expr
#undef CONNECTIFELSE
#define CONNECTIFELSE  ;
#undef ELSE_PRED_ACTION
#define ELSE_PRED_ACTION(pred, expr, var) else var GENMODE expr

#define COMPUTE_UNARY(func_name, operation, para1_t, para2_t, ret_t)             \
ret_t compute_##func_name(para1_t a, para2_t b) {                                \
  operation(a, b);                                                               \
  return b;                                                                      \
}

#define COMPUTE_BINARY(func_name, operation, para1_t, para2_t, para3_t, ret_t)   \
ret_t compute_##func_name(para1_t a, para2_t b, para3_t c) {                     \
  operation(a, b, c);                                                            \
  return c;                                                                      \
}
                                                              
COMPUTE_UNARY(mov, MOV_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(le16, LE16_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(le32, LE32_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(le64, LE64_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(be16, BE16_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(be32, BE32_EXPR, int64_t, int64_t, int64_t)
COMPUTE_UNARY(be64, BE64_EXPR, int64_t, int64_t, int64_t)

COMPUTE_BINARY(add, ADD_EXPR, int64_t, int64_t, int64_t, int64_t)
COMPUTE_BINARY(add, ADD_EXPR, int64_t, int32_t, int64_t, int64_t)
COMPUTE_BINARY(add, ADD_EXPR, int32_t, int32_t, int32_t, int32_t)
COMPUTE_BINARY(max, MAX_EXPR, int64_t, int64_t, int64_t, int64_t)

#undef GENMODE
#define GENMODE ==
#undef IF_PRED_ACTION
#define IF_PRED_ACTION(pred, expr, var) ((pred) && (var GENMODE expr))
#undef CONNECTIFELSE
#define CONNECTIFELSE ||
#undef ELSE_PRED_ACTION
#define ELSE_PRED_ACTION(pred, expr, var) (!(pred) && (var GENMODE expr))

z3::expr predicate_add(z3::expr a, z3::expr b, z3::expr c) {
  return ADD_EXPR(a, b, c);
}

z3::expr predicate_mov(int a, z3::expr b) {
  return MOV_EXPR(a, b);
}

z3::expr predicate_max(z3::expr a, int b, z3::expr c) {
  return MAX_EXPR(a, b, c);
}

z3::expr predicate_max(z3::expr a, z3::expr b, z3::expr c) {
  return MAX_EXPR(a, b, c);
}
