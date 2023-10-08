/**
 * @file BigFloat.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef __BIG_FLOAT_H__
#define __BIG_FLOAT_H__

#include "BigNum/BigNum.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigUInt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char big_float_info_t;

typedef struct {
	big_int_t exponent;
	big_uint_t mantissa;
	big_float_info_t info;
} big_float_t;

void 				big_float_set_max(big_float_t* self);
void 				big_float_set_min(big_float_t* self);
void 				big_float_set_zero(big_float_t * self);
void 				big_float_set_one(big_float_t * self);
void 				big_float_set_05(big_float_t * self);
void				big_float_set_e(big_float_t * self);
void				big_float_set_ln2(big_float_t * self);
void 				big_float_set_nan(big_float_t * self);
void 				big_float_set_zero_nan(big_float_t * self);
void 				big_float_swap(big_float_t * self, big_float_t * ss2);

bool 				big_float_is_zero(big_float_t self);
bool 				big_float_is_sign(big_float_t self);
bool 				big_float_is_nan(big_float_t self);

void 				big_float_abs(big_float_t * self);
big_num_carry_t		big_float_round(big_float_t * self);
void 				big_float_sgn(big_float_t * self);
void 				big_float_set_sign(big_float_t * self);

big_num_carry_t 	big_float_add(big_float_t * self, big_float_t ss2, bool round);
big_num_carry_t 	big_float_sub(big_float_t * self, big_float_t ss2, bool round);
big_num_carry_t 	big_float_mul_uint(big_float_t * self, big_num_strg_t ss2);
big_num_carry_t 	big_float_mul_int(big_float_t * self, big_num_sstrg_t ss2);
big_num_carry_t 	big_float_mul(big_float_t * self, big_float_t ss2, bool round);

big_num_ret_t 		big_float_div(big_float_t * self, big_float_t ss2, bool round);
big_num_ret_t 		big_float_mod(big_float_t * self, big_float_t ss2);
big_num_strg_t 		big_float_mod2(big_float_t self);

big_num_ret_t 		big_float_pow_big_uint(big_float_t * self, big_uint_t pow);
big_num_ret_t 		big_float_pow_big_int(big_float_t * self, big_int_t pow);
big_num_ret_t 		big_float_pow_big_frac(big_float_t * self, big_float_t pow);
big_num_ret_t		big_float_pow(big_float_t * self, big_float_t pow);
big_num_ret_t 		big_float_sqrt(big_float_t * self);
big_num_carry_t		big_float_exp(big_float_t * self, big_float_t x);
big_num_ret_t 		big_float_ln(big_float_t * self, big_float_t x);
big_num_ret_t 		big_float_log(big_float_t * self, big_float_t x, big_float_t base);

void 				big_float_init_float(big_float_t * self, float value);
void				big_float_init_double(big_float_t * self, double value);
void 				big_float_init_uint(big_float_t * self, big_num_strg_t value);
void 				big_float_init_int(big_float_t * self, big_num_sstrg_t value);
void				big_float_init_big_float(big_float_t * self, big_float_t value);
void				big_float_init_big_uint(big_float_t * self, big_uint_t value);
void				big_float_init_big_int(big_float_t * self, big_int_t value);
big_num_carry_t		big_float_to_double(big_float_t self, double * result);
big_num_carry_t		big_float_to_float(big_float_t self, float * result);
big_num_carry_t		big_float_to_uint(big_float_t self, big_num_strg_t * result);
big_num_carry_t		big_float_to_int(big_float_t self, big_num_sstrg_t * result);

bool 				big_float_cmp_smaller(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger(big_float_t self, big_float_t l);
bool 				big_float_cmp_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_smaller_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger_equal(big_float_t self, big_float_t l);

void				big_float_print(big_float_t self);

#ifdef __cplusplus
}
#endif

#endif // __BIG_FLOAT_H__