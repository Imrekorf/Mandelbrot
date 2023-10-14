/**
 * @file BigUInt.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef __BIG_UINT_H__
#define __BIG_UINT_H__

#include <stdint.h>
#include <stdbool.h>
#include "BigNum/BigNum.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UINT_PREC BIG_NUM_PREC

struct big_uint_s {
	size_t			size;
	big_num_strg_t 	table[2*UINT_PREC]; // use 2* for non-carry functions
};
#ifndef GL_core_profile
typedef struct big_uint_s big_uint_t;
#else
#define big_uint_t big_uint_s
#endif

size_t	 			big_uint_size(_big_num_inout(big_uint_t, self));
void 				big_uint_set_zero(_big_num_inout(big_uint_t, self));
void 				big_uint_set_one(_big_num_inout(big_uint_t, self));
void 				big_uint_set_max(_big_num_inout(big_uint_t, self));
void 				big_uint_set_min(_big_num_inout(big_uint_t, self));
void 				big_uint_swap(_big_num_inout(big_uint_t, self), _big_num_inout(big_uint_t, ss2));
void 				big_uint_set_from_table(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t temp_table[2*UINT_PREC], size_t temp_table_len);


big_num_carry_t		big_uint_add_uint(_big_num_inout(big_uint_t, self), big_num_strg_t val);
big_num_carry_t 	big_uint_add(_big_num_inout(big_uint_t, self), big_uint_t ss2, big_num_carry_t c);
big_num_carry_t		big_uint_sub_uint(_big_num_inout(big_uint_t, self), big_num_strg_t val);
big_num_carry_t 	big_uint_sub(_big_num_inout(big_uint_t, self), big_uint_t ss2, big_num_carry_t c);
big_num_strg_t 		big_uint_rcl(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c);
size_t	 			big_uint_compensation_to_left(_big_num_inout(big_uint_t, self));
bool 				big_uint_find_leading_bit(big_uint_t self, _big_num_out(size_t, table_id), _big_num_out(size_t, index));
bool 				big_uint_find_lowest_bit(big_uint_t self, _big_num_out(size_t, table_id), _big_num_out(size_t, index));
bool	 			big_uint_get_bit(big_uint_t self, size_t bit_index);
bool	 			big_uint_set_bit(_big_num_inout(big_uint_t, self), size_t bit_index);
void 				big_uint_bit_and(_big_num_inout(big_uint_t, self), big_uint_t ss2);
void 				big_uint_bit_or(_big_num_inout(big_uint_t, self), big_uint_t ss2);
void 				big_uint_bit_xor(_big_num_inout(big_uint_t, self), big_uint_t ss2);
void 				big_uint_bit_not(_big_num_inout(big_uint_t, self));
void				big_uint_bit_not2(_big_num_inout(big_uint_t, self));


big_num_carry_t		big_uint_mul_int(_big_num_inout(big_uint_t, self), big_num_strg_t ss2);
big_num_carry_t		big_uint_mul(_big_num_inout(big_uint_t, self), big_uint_t ss2, big_num_algo_t algorithm);
void				big_uint_mul_no_carry(_big_num_inout(big_uint_t, self), big_uint_t ss2, _big_num_out(big_uint_t , result), big_num_algo_t algorithm);

big_num_div_ret_t 	big_uint_div_int(_big_num_inout(big_uint_t, self), big_num_strg_t divisor, _big_num_out(big_num_strg_t, remainder));
big_num_div_ret_t 	big_uint_div(_big_num_inout(big_uint_t, self), big_uint_t divisor, _big_num_out(big_uint_t , remainder), big_num_algo_t algorithm);

big_num_ret_t		big_uint_pow(_big_num_inout(big_uint_t, self), big_uint_t pow);
void 				big_uint_sqrt(_big_num_inout(big_uint_t, self));


void 				big_uint_clear_first_bits(_big_num_inout(big_uint_t, self), size_t n);
bool 				big_uint_is_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_zero(big_uint_t self);
bool 				big_uint_are_first_bits_zero(big_uint_t self, size_t bits);

void				big_uint_init(_big_num_inout(big_uint_t, self), size_t size);
void 				big_uint_init_uint(_big_num_inout(big_uint_t, self), size_t size, big_num_strg_t value);
big_num_carry_t		big_uint_init_ulint(_big_num_inout(big_uint_t, self), size_t size, big_num_lstrg_t value);
big_num_carry_t		big_uint_init_big_uint(_big_num_inout(big_uint_t, self), size_t size, big_uint_t value);
big_num_carry_t		big_uint_init_int(_big_num_inout(big_uint_t, self), size_t size, big_num_sstrg_t value);
big_num_carry_t		big_uint_init_lint(_big_num_inout(big_uint_t, self), size_t size, big_num_lsstrg_t value);
big_num_carry_t		big_uint_to_uint(big_uint_t self, _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_uint_to_int(big_uint_t self, _big_num_out(big_num_sstrg_t, result));
big_num_carry_t		big_uint_to_luint(big_uint_t self, _big_num_out(big_num_lstrg_t, result));
big_num_carry_t		big_uint_to_lint(big_uint_t self, _big_num_out(big_num_lsstrg_t, result));

bool 				big_uint_cmp_smaller(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_smaller_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);

#ifdef __cplusplus
}
#endif

#endif // __BIG_UINT_H__