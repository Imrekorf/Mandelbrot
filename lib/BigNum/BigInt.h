/**
 * @file BigInt.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#ifndef __BIG_INT_H__
#define __BIG_INT_H__

#include "BigNum/BigNum.h"
#include "BigNum/BigUInt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GL_core_profile
typedef big_uint_t big_int_t;
#else
#define big_int_t	big_uint_t
#endif

/**
 * return results for big_int_change_sign
 */
#ifndef GL_core_profile
typedef enum {
	/**
	 * changed sign
	 */
	BIG_INT_SIGN_CHANGE_OK = 0,
	/**
	 * returned if impossible to change sign value
	 */
	BIG_INT_SIGN_CHANGE_FAILED = 1,
} big_int_sign_ret_t;
#else
#define BIG_INT_SIGN_CHANGE_OK		0
#define BIG_INT_SIGN_CHANGE_FAILED 	1
#define big_int_sign_ret_t			uint
#endif

void 				big_int_set_max(_big_num_inout(big_int_t, self));
void 				big_int_set_min(_big_num_inout(big_int_t, self));
void 				big_int_set_zero(_big_num_inout(big_int_t, self));
void 				big_int_swap(_big_num_inout(big_int_t, self), _big_num_inout(big_int_t, ss2));

void				big_int_set_sign_one(_big_num_inout(big_int_t, self));
big_int_sign_ret_t	big_int_change_sign(_big_num_inout(big_int_t, self));
void				big_int_set_sign(_big_num_inout(big_int_t, self));
bool				big_int_is_sign(big_int_t self);
big_num_strg_t		big_int_abs(_big_num_inout(big_int_t, self));

big_num_carry_t 	big_int_add(_big_num_inout(big_int_t, self), big_int_t ss2);
big_num_strg_t		big_int_add_int(_big_num_inout(big_int_t, self), big_num_strg_t value);
big_num_carry_t 	big_int_sub(_big_num_inout(big_int_t, self), big_int_t ss2);
big_num_strg_t		big_int_sub_int(_big_num_inout(big_int_t, self), big_num_strg_t value);

size_t	 			big_int_compensation_to_left(_big_num_inout(big_int_t, self));

big_num_carry_t		big_int_mul_int(_big_num_inout(big_int_t, self), big_num_sstrg_t ss2);
big_num_carry_t		big_int_mul(_big_num_inout(big_int_t, self), big_int_t ss2);

big_num_div_ret_t 	big_int_div_int(_big_num_inout(big_int_t, self), big_num_sstrg_t divisor, _big_num_out(big_num_sstrg_t, remainder));
big_num_div_ret_t 	big_int_div(_big_num_inout(big_int_t, self), big_int_t divisor, _big_num_out(big_int_t, remainder));

big_num_ret_t		big_int_pow(_big_num_inout(big_int_t, self), big_int_t pow);

void				big_int_init(_big_num_inout(big_int_t, self), size_t size);
big_num_carry_t		big_int_init_uint(_big_num_inout(big_int_t, self), size_t size, big_num_strg_t value);
big_num_carry_t		big_int_init_ulint(_big_num_inout(big_int_t, self), size_t size, big_num_lstrg_t value);
big_num_carry_t		big_int_init_big_uint(_big_num_inout(big_int_t, self), size_t size, big_uint_t value);
void				big_int_init_int(_big_num_inout(big_int_t, self), size_t size, big_num_sstrg_t value);
big_num_carry_t		big_int_init_lint(_big_num_inout(big_int_t, self), size_t size, big_num_lsstrg_t value);
big_num_carry_t		big_int_init_big_int(_big_num_inout(big_int_t, self), size_t size, big_int_t value);
big_num_carry_t		big_int_to_uint(big_int_t self, _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_int_to_int(big_int_t self, _big_num_out(big_num_sstrg_t, result));
big_num_carry_t		big_int_to_luint(big_int_t self, _big_num_out(big_num_lstrg_t, result));
big_num_carry_t		big_int_to_lint(big_int_t self, _big_num_out(big_num_lsstrg_t, result));

bool 				big_int_cmp_smaller(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger(big_int_t self, big_int_t l);
bool 				big_int_cmp_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_smaller_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger_equal(big_int_t self, big_int_t l);

#ifdef __cplusplus
}
#endif

#endif // __BIG_INT_H__