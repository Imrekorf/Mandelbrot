/**
 * @file BigFloat.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef _BIG_FLOAT_H_
#define _BIG_FLOAT_H_

#ifndef GL_core_profile
#include "BigNum/BigNum.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigUInt.h"
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
extern "C" {
#endif

#ifndef GL_core_profile
typedef big_num_strg_t 		big_float_info_t;
#else
#define big_float_info_t	big_num_strg_t
#endif

struct big_float_s{
	big_int_t exponent;
	big_uint_t mantissa;
	big_float_info_t info;
};

#ifndef GL_core_profile
typedef struct big_float_s big_float_t;
#else
#define big_float_t big_float_s
#endif

void 				big_float_set_max(_big_num_inout(big_float_t, self));
void 				big_float_set_min(_big_num_inout(big_float_t, self));
void 				big_float_set_zero(_big_num_inout(big_float_t, self));
void 				big_float_set_one(_big_num_inout(big_float_t, self));
void 				big_float_set_05(_big_num_inout(big_float_t, self));
void				big_float_set_e(_big_num_inout(big_float_t, self));
void				big_float_set_ln2(_big_num_inout(big_float_t, self));
void 				big_float_set_nan(_big_num_inout(big_float_t, self));
void 				big_float_set_zero_nan(_big_num_inout(big_float_t, self));
void 				big_float_swap(_big_num_inout(big_float_t, self), _big_num_inout(big_float_t, ss2));

bool 				big_float_is_zero(big_float_t self);
bool 				big_float_is_sign(big_float_t self);
bool 				big_float_is_nan(big_float_t self);

void 				big_float_abs(_big_num_inout(big_float_t, self));
big_num_carry_t		big_float_round(_big_num_inout(big_float_t, self));
void 				big_float_sgn(_big_num_inout(big_float_t, self));
void 				big_float_set_sign(_big_num_inout(big_float_t, self));

big_num_carry_t 	big_float_add(_big_num_inout(big_float_t, self), big_float_t ss2, bool round);
big_num_carry_t 	big_float_sub(_big_num_inout(big_float_t, self), big_float_t ss2, bool round);
big_num_carry_t 	big_float_mul_uint(_big_num_inout(big_float_t, self), big_num_strg_t ss2);
big_num_carry_t 	big_float_mul_int(_big_num_inout(big_float_t, self), big_num_sstrg_t ss2);
big_num_carry_t 	big_float_mul(_big_num_inout(big_float_t, self), big_float_t ss2, bool round);

big_num_ret_t 		big_float_div(_big_num_inout(big_float_t, self), big_float_t ss2, bool round);
big_num_ret_t 		big_float_mod(_big_num_inout(big_float_t, self), big_float_t ss2);
big_num_strg_t 		big_float_mod2(big_float_t self);

big_num_ret_t 		big_float_pow_big_uint(_big_num_inout(big_float_t, self), big_uint_t pow);
big_num_ret_t 		big_float_pow_big_int(_big_num_inout(big_float_t, self), big_int_t pow);
big_num_ret_t 		big_float_pow_big_frac(_big_num_inout(big_float_t, self), big_float_t pow);
big_num_ret_t		big_float_pow(_big_num_inout(big_float_t, self), big_float_t pow);
big_num_ret_t 		big_float_sqrt(_big_num_inout(big_float_t, self));
big_num_carry_t		big_float_exp(_big_num_inout(big_float_t, self), big_float_t x);
big_num_ret_t 		big_float_ln(_big_num_inout(big_float_t, self), big_float_t x);
big_num_ret_t 		big_float_log(_big_num_inout(big_float_t, self), big_float_t x, big_float_t base);

void				big_float_init(_big_num_inout(big_float_t, self), size_t man, size_t exp);
void 				big_float_init_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, float value);
void				big_float_init_double(_big_num_inout(big_float_t, self), size_t man, size_t exp, double value);
void 				big_float_init_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_strg_t value);
void 				big_float_init_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_sstrg_t value);
big_num_carry_t		big_float_init_big_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_float_t value);
void				big_float_init_big_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_uint_t value);
void				big_float_init_big_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_int_t value);
#define				big_float_set_double(ptr, value)		big_float_init_double(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
#define				big_float_set_uint(ptr, value)			big_float_init_uint(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
#define				big_float_set_int(ptr, value)			big_float_init_int(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
#define				big_float_set_big_float(ptr, value)		big_float_init_big_float(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
#define				big_float_set_big_uint(ptr, value)		big_float_init_big_uint(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
#define				big_float_set_big_int(ptr, value)		big_float_init_big_int(ptr, (ptr)->mantissa.size, (ptr)->exponent.size, value)
big_num_carry_t		big_float_to_double(big_float_t self, _big_num_out(double, result));
big_num_carry_t		big_float_to_float(big_float_t self, _big_num_out(float, result));
big_num_carry_t		big_float_to_uint(big_float_t self, _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_float_to_int(big_float_t self, _big_num_out(big_num_sstrg_t, result));

bool 				big_float_cmp_smaller(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger(big_float_t self, big_float_t l);
bool 				big_float_cmp_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_smaller_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger_equal(big_float_t self, big_float_t l);

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif

#endif // _BIG_FLOAT_H_