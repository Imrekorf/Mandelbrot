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

typedef big_uint_t big_int_t;

/**
 * return results for big_int_change_sign
 */
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

void 				big_int_set_max(big_int_t* self);
void 				big_int_set_min(big_int_t* self);
void 				big_int_set_zero(big_int_t* self);
void 				big_int_swap(big_int_t* self, big_int_t* ss2);

void				big_int_set_sign_one(big_int_t* self);
big_int_sign_ret_t	big_int_change_sign(big_int_t* self);
void				big_int_set_sign(big_int_t* self);
bool				big_int_is_sign(big_int_t self);
big_num_strg_t		big_int_abs(big_int_t* self);

big_num_carry_t 	big_int_add(big_int_t* self, big_int_t ss2);
big_num_strg_t		big_int_add_int(big_int_t* self, big_num_strg_t value);
big_num_carry_t 	big_int_sub(big_int_t* self, big_int_t ss2);
big_num_strg_t		big_int_sub_int(big_int_t* self, big_num_strg_t value);

size_t	 			big_int_compensation_to_left(big_int_t* self);

big_num_carry_t		big_int_mul_int(big_int_t* self, big_num_sstrg_t ss2);
big_num_carry_t		big_int_mul(big_int_t* self, big_int_t ss2);

big_num_div_ret_t 	big_int_div_int(big_int_t* self, big_num_sstrg_t divisor, big_num_sstrg_t * remainder);
big_num_div_ret_t 	big_int_div(big_int_t* self, big_int_t divisor, big_int_t * remainder);

big_num_ret_t		big_int_pow(big_int_t* self, big_int_t pow);

big_num_carry_t		big_int_init_uint(big_int_t* self, big_num_strg_t value);
big_num_carry_t		big_int_init_ulint(big_int_t* self, big_num_lstrg_t value);
big_num_carry_t		big_int_init_big_uint(big_int_t* self, big_uint_t value);
void				big_int_init_int(big_int_t* self, big_num_sstrg_t value);
big_num_carry_t		big_int_init_lint(big_int_t* self, big_num_lsstrg_t value);
void				big_int_init_big_int(big_int_t* self, big_int_t value);
big_num_carry_t		big_int_to_uint(big_int_t self, big_num_strg_t * result);
big_num_carry_t		big_int_to_int(big_int_t self, big_num_sstrg_t * result);
big_num_carry_t		big_int_to_luint(big_int_t self, big_num_lstrg_t * result);
big_num_carry_t		big_int_to_lint(big_int_t self, big_num_lsstrg_t * result);

bool 				big_int_cmp_smaller(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger(big_int_t self, big_int_t l);
bool 				big_int_cmp_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_smaller_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger_equal(big_int_t self, big_int_t l);

void				big_int_print(big_int_t self);

#ifdef __cplusplus
}
#endif

#endif // __BIG_INT_H__