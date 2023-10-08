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

typedef struct {
	big_num_strg_t table[BIG_NUM_PREC];
} big_uint_t;

// used for MulXBigN functions
typedef struct {
	big_num_strg_t table[2*BIG_NUM_PREC];
} big_big_uint_t;

size_t	 			big_uint_size(big_uint_t* self);
void 				big_uint_set_zero(big_uint_t* self);
void 				big_uint_set_zero_big(big_big_uint_t* self);
void 				big_uint_set_one(big_uint_t* self);
void				big_uint_set_one_big(big_big_uint_t* self);
void 				big_uint_set_max(big_uint_t* self);
void 				big_uint_set_min(big_uint_t* self);
void 				big_uint_swap(big_uint_t* self, big_uint_t* ss2);
void 				big_uint_set_from_table(big_uint_t* self, const big_num_strg_t temp_table[], size_t temp_table_len);


big_num_carry_t		big_uint_add_uint(big_uint_t* self, big_num_strg_t val);
big_num_carry_t 	big_uint_add(big_uint_t* self, big_uint_t ss2, big_num_carry_t c);
big_num_carry_t 	big_uint_add_big(big_big_uint_t* self, big_big_uint_t ss2, big_num_carry_t c);
big_num_carry_t		big_uint_sub_uint(big_uint_t* self, big_num_strg_t val);
big_num_carry_t 	big_uint_sub(big_uint_t* self, big_uint_t ss2, big_num_carry_t c);
big_num_carry_t 	big_uint_sub_big(big_big_uint_t* self, big_big_uint_t ss2, big_num_carry_t c);
big_num_strg_t 		big_uint_rcl(big_uint_t* self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcl_big(big_big_uint_t* self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr(big_uint_t* self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr_big(big_big_uint_t* self, size_t bits, big_num_carry_t c);
size_t	 			big_uint_compensation_to_left(big_uint_t* self);
size_t	 			big_uint_compensation_to_left_big(big_big_uint_t* self);
bool 				big_uint_find_leading_bit(big_uint_t self, size_t * table_id, size_t * index);
bool 				big_uint_find_lowest_bit(big_uint_t self, size_t * table_id, size_t * index);
bool	 			big_uint_get_bit(big_uint_t self, size_t bit_index);
bool	 			big_uint_set_bit(big_uint_t* self, size_t bit_index);
void 				big_uint_bit_and(big_uint_t* self, big_uint_t ss2);
void 				big_uint_bit_or(big_uint_t* self, big_uint_t ss2);
void 				big_uint_bit_xor(big_uint_t* self, big_uint_t ss2);
void 				big_uint_bit_not(big_uint_t* self);
void				big_uint_bit_not2(big_uint_t* self);


big_num_carry_t		big_uint_mul_int(big_uint_t* self, big_num_strg_t ss2);
big_num_carry_t 	big_uint_mul_int_big(big_big_uint_t* self, big_num_strg_t ss2);
big_num_carry_t		big_uint_mul(big_uint_t* self, big_uint_t ss2, big_num_algo_t algorithm);
void				big_uint_mul_no_carry(big_uint_t* self, big_uint_t ss2, big_big_uint_t * result, big_num_algo_t algorithm);

big_num_div_ret_t 	big_uint_div_int(big_uint_t* self, big_num_strg_t divisor, big_num_strg_t * remainder);
big_num_div_ret_t 	big_uint_div(big_uint_t* self, big_uint_t divisor, big_uint_t * remainder, big_num_algo_t algorithm);
big_num_div_ret_t 	big_uint_div_big(big_big_uint_t* self, big_big_uint_t divisor, big_big_uint_t * remainder, big_num_algo_t algorithm);

big_num_ret_t		big_uint_pow(big_uint_t* self, big_uint_t pow);
void 				big_uint_sqrt(big_uint_t* self);


void 				big_uint_clear_first_bits(big_uint_t* self, size_t n);
bool 				big_uint_is_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_zero(big_uint_t self);
bool 				big_uint_are_first_bits_zero(big_uint_t self, size_t bits);


void 				big_uint_init_uint(big_uint_t* self, big_num_strg_t value);
big_num_carry_t		big_uint_init_ulint(big_uint_t* self, big_num_lstrg_t value);
void 				big_uint_init_big_uint(big_uint_t* self, big_uint_t value);
big_num_carry_t		big_uint_init_int(big_uint_t* self, big_num_sstrg_t value);
big_num_carry_t		big_uint_init_lint(big_uint_t* self, big_num_lsstrg_t value);
big_num_carry_t		big_uint_to_uint(big_uint_t self, big_num_strg_t * result);
big_num_carry_t		big_uint_to_int(big_uint_t self, big_num_sstrg_t * result);
big_num_carry_t		big_uint_to_luint(big_uint_t self, big_num_lstrg_t * result);
big_num_carry_t		big_uint_to_lint(big_uint_t self, big_num_lsstrg_t * result);

bool 				big_uint_cmp_smaller(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_equal(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_smaller_equal(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger_equal(big_uint_t self, const big_uint_t l, ssize_t index);

void				big_uint_print(big_uint_t self);

#ifdef __cplusplus
}
#endif

#endif // __BIG_UINT_H__