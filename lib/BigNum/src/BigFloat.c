/**
 * @file BigFloat.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#ifndef GL_core_profile
#include "BigNum/BigNum.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigFloat.h"
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
extern "C" {
#endif

#define BIG_MAN_PREC	BIG_NUM_PREC

#ifndef GL_core_profile
#define _big_float_info_t(x) 	((big_float_info_t)(x))
#else
#define _big_float_info_t(x)	(big_float_info_t(x))
#endif

#define BIG_FLOAT_INFO_SIGN 	(_big_float_info_t(1 << 7))
#define BIG_FLOAT_INFO_NAN		(_big_float_info_t(1 << 6))
#define BIG_FLOAT_INFO_ZERO		(_big_float_info_t(1 << 5))

#ifndef GL_core_profile
extern big_num_sstrg_t  _big_uint_find_leading_bit_in_word(big_num_strg_t x);
#endif

_big_num_static big_num_carry_t 	_big_float_check_carry(_big_num_inout(big_float_t, self), big_num_carry_t c);
_big_num_static big_num_carry_t 	_big_float_standardizing(_big_num_inout(big_float_t, self));
_big_num_static bool				_big_float_correct_zero(_big_num_inout(big_float_t, self));
_big_num_static void				_big_float_clear_info_bit(_big_num_inout(big_float_t, self), big_float_info_t bit);
_big_num_static void				_big_float_set_info_bit(_big_num_inout(big_float_t, self), big_float_info_t bit);
_big_num_static bool				_big_float_is_info_bit(big_float_t self, big_float_info_t bit);

_big_num_static big_num_carry_t 	_big_float_round_half_to_even(_big_num_inout(big_float_t, self), bool is_half, bool rounding_up);
_big_num_static void 				_big_float_add_check_exponents(_big_num_inout(big_float_t, self), _big_num_inout(big_float_t, ss2), big_int_t exp_offset, _big_num_out(bool, last_bit_set), _big_num_out(bool, rest_zero), _big_num_out(bool, do_adding), _big_num_out(bool, do_rounding));
_big_num_static big_num_carry_t 	_big_float_add_mantissas(_big_num_inout(big_float_t, self), big_float_t ss2, _big_num_out(bool, last_bit_set), _big_num_out(bool, rest_zero));
_big_num_static big_num_carry_t 	_big_float_add(_big_num_inout(big_float_t, self), big_float_t ss2, bool round, bool adding);
_big_num_static	bool				_big_float_check_greater_or_equal_half(_big_num_inout(big_float_t, self), big_num_strg_t tab[2*BIG_MAN_PREC], big_num_strg_t len);
_big_num_static big_num_ret_t 		_big_float_pow_big_float_uint(_big_num_inout(big_float_t, self), big_float_t pow);
_big_num_static big_num_ret_t 		_big_float_pow_big_float_int(_big_num_inout(big_float_t, self), big_float_t pow);
_big_num_static void 				_big_float_exp_surrounding_0(_big_num_inout(big_float_t, self), big_float_t x, _big_num_out(size_t, steps));
_big_num_static void 				_big_float_ln_surrounding_1(_big_num_inout(big_float_t, self), big_float_t x, _big_num_out(size_t, steps));
_big_num_static void 				_big_float_init_uint_or_int(_big_num_inout(big_float_t, self), big_uint_t value, big_num_sstrg_t compensation);
_big_num_static big_num_carry_t 	_big_float_to_uint_or_int(big_float_t self, _big_num_out(big_num_strg_t, result));
_big_num_static void 				_big_float_init_double_set_exp_and_man(_big_num_inout(big_float_t, self), bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2);
_big_num_static double 				_big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan);
_big_num_static bool 				_big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2);
_big_num_static bool				_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2);
_big_num_static bool				_big_float_equal_without_sign(big_float_t self, big_float_t ss2);
_big_num_static void 				_big_float_skip_fraction(_big_num_inout(big_float_t, self));
_big_num_static bool 				_big_float_is_integer(big_float_t self);
/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_float_set_zero(_big_num_inout(big_float_t, self))
{
	self->info = BIG_FLOAT_INFO_ZERO;
	big_int_set_zero(_big_num_ref(self->exponent));
	big_uint_set_zero(_big_num_ref(self->mantissa));
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_float_set_one(_big_num_inout(big_float_t, self))
{
	self->info = 0;
	big_uint_set_zero(_big_num_ref(self->mantissa));
	self->mantissa.table[self->mantissa.size-1] = BIG_NUM_HIGHEST_BIT;
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, -_big_num_sstrg_t(self->mantissa.size * BIG_NUM_BITS_PER_UNIT - 1));
}

/**
 * this method sets 0.5
 * @param[in, out] self the big num object
 */
void big_float_set_05(_big_num_inout(big_float_t, self))
{
	big_float_set_one(self);
	big_int_sub_int(_big_num_ref(self->exponent), 1, 0);
}

/**
 * this method sets the value of e
 * (the base of the natural logarithm)
 * @param[in] self the big num object
 */
void big_float_set_e(_big_num_inout(big_float_t, self))
{
	_big_num_static const big_num_strg_t temp_table[2*BIG_MAN_PREC] = {
		0xadf85458,
		#if 2*BIG_MAN_PREC == 1
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa2bb4a9a,
		#if 2*BIG_MAN_PREC == 2
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xafdc5620,
		#if 2*BIG_MAN_PREC == 3
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x273d3cf1,
		#if 2*BIG_MAN_PREC == 4
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd8b9c583,
		#if 2*BIG_MAN_PREC == 5
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xce2d3695,
		#if 2*BIG_MAN_PREC == 6
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa9e13641,
		#if 2*BIG_MAN_PREC == 7
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x146433fb,
		#if 2*BIG_MAN_PREC == 8
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xcc939dce,
		#if 2*BIG_MAN_PREC == 9
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x249b3ef9,
		#if 2*BIG_MAN_PREC == 10
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7d2fe363,
		#if 2*BIG_MAN_PREC == 11
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x630c75d8,
		#if 2*BIG_MAN_PREC == 12
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf681b202,
		#if 2*BIG_MAN_PREC == 13
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xaec4617a,
		#if 2*BIG_MAN_PREC == 14
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd3df1ed5,
		#if 2*BIG_MAN_PREC == 15
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd5fd6561,
		#if 2*BIG_MAN_PREC == 16
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x2433f51f,
		#if 2*BIG_MAN_PREC == 17
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5f066ed0,
		#if 2*BIG_MAN_PREC == 18
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x85636555,
		#if 2*BIG_MAN_PREC == 19
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3ded1af3,
		#if 2*BIG_MAN_PREC == 20
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb557135e,
		#if 2*BIG_MAN_PREC == 21
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7f57c935,
		#if 2*BIG_MAN_PREC == 22
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x984f0c70,
		#if 2*BIG_MAN_PREC == 23
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe0e68b77,
		#if 2*BIG_MAN_PREC == 24
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xe2a689da,
		#if 2*BIG_MAN_PREC == 25
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf3efe872,
		#if 2*BIG_MAN_PREC == 26
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1df158a1,
		#if 2*BIG_MAN_PREC == 27
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x36ade735,
		#if 2*BIG_MAN_PREC == 28
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x30acca4f,
		#if 2*BIG_MAN_PREC == 29
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x483a797a,
		#if 2*BIG_MAN_PREC == 30
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbc0ab182,
		#if 2*BIG_MAN_PREC == 31
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb324fb61,
		#if 2*BIG_MAN_PREC == 32
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xd108a94b,
		#if 2*BIG_MAN_PREC == 33
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb2c8e3fb,
		#if 2*BIG_MAN_PREC == 34
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb96adab7,
		#if 2*BIG_MAN_PREC == 35
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x60d7f468,
		#if 2*BIG_MAN_PREC == 36
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1d4f42a3,
		#if 2*BIG_MAN_PREC == 37
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xde394df4,
		#if 2*BIG_MAN_PREC == 38
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xae56ede7,
		#if 2*BIG_MAN_PREC == 39
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6372bb19,
		#if 2*BIG_MAN_PREC == 40
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x0b07a7c8,
		#if 2*BIG_MAN_PREC == 41
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xee0a6d70,
		#if 2*BIG_MAN_PREC == 42
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9e02fce1,
		#if 2*BIG_MAN_PREC == 43
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcdf7e2ec,
		#if 2*BIG_MAN_PREC == 44
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc03404cd,
		#if 2*BIG_MAN_PREC == 45
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x28342f61,
		#if 2*BIG_MAN_PREC == 46
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9172fe9c,
		#if 2*BIG_MAN_PREC == 47
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe98583ff,
		#if 2*BIG_MAN_PREC == 48
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x8e4f1232,
		#if 2*BIG_MAN_PREC == 49
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xeef28183,
		#if 2*BIG_MAN_PREC == 50
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc3fe3b1b,
		#if 2*BIG_MAN_PREC == 51
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4c6fad73,
		#if 2*BIG_MAN_PREC == 52
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3bb5fcbc,
		#if 2*BIG_MAN_PREC == 53
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2ec22005,
		#if 2*BIG_MAN_PREC == 54
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc58ef183,
		#if 2*BIG_MAN_PREC == 55
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7d1683b2,
		#if 2*BIG_MAN_PREC == 56
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xc6f34a26,
		#if 2*BIG_MAN_PREC == 57
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc1b2effa,
		#if 2*BIG_MAN_PREC == 58
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x886b4238,
		#if 2*BIG_MAN_PREC == 59
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x611fcfdc,
		#if 2*BIG_MAN_PREC == 60
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xde355b3b,
		#if 2*BIG_MAN_PREC == 61
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6519035b,
		#if 2*BIG_MAN_PREC == 62
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbc34f4de,
		#if 2*BIG_MAN_PREC == 63
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf99c0238,
		#if 2*BIG_MAN_PREC == 64
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x61b46fc9,
		#if 2*BIG_MAN_PREC == 65
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd6e6c907,
		#if 2*BIG_MAN_PREC == 66
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7ad91d26,
		#if 2*BIG_MAN_PREC == 67
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x91f7f7ee,
		#if 2*BIG_MAN_PREC == 68
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x598cb0fa,
		#if 2*BIG_MAN_PREC == 69
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc186d91c,
		#if 2*BIG_MAN_PREC == 70
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xaefe1309,
		#if 2*BIG_MAN_PREC == 71
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x85139270,
		#if 2*BIG_MAN_PREC == 72
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xb4130c93,
		#if 2*BIG_MAN_PREC == 73
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbc437944,
		#if 2*BIG_MAN_PREC == 74
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf4fd4452,
		#if 2*BIG_MAN_PREC == 75
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe2d74dd3,
		#if 2*BIG_MAN_PREC == 76
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x64f2e21e,
		#if 2*BIG_MAN_PREC == 77
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x71f54bff,
		#if 2*BIG_MAN_PREC == 78
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5cae82ab,
		#if 2*BIG_MAN_PREC == 79
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9c9df69e,
		#if 2*BIG_MAN_PREC == 80
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xe86d2bc5,
		#if 2*BIG_MAN_PREC == 81
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x22363a0d,
		#if 2*BIG_MAN_PREC == 82
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xabc52197,
		#if 2*BIG_MAN_PREC == 83
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9b0deada,
		#if 2*BIG_MAN_PREC == 84
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1dbf9a42,
		#if 2*BIG_MAN_PREC == 85
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd5c4484e,
		#if 2*BIG_MAN_PREC == 86
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0abcd06b,
		#if 2*BIG_MAN_PREC == 87
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfa53ddef,
		#if 2*BIG_MAN_PREC == 88
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x3c1b20ee,
		#if 2*BIG_MAN_PREC == 89
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3fd59d7c,
		#if 2*BIG_MAN_PREC == 90
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x25e41d2b,
		#if 2*BIG_MAN_PREC == 91
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x669e1ef1,
		#if 2*BIG_MAN_PREC == 92
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6e6f52c3,
		#if 2*BIG_MAN_PREC == 93
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x164df4fb,
		#if 2*BIG_MAN_PREC == 94
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7930e9e4,
		#if 2*BIG_MAN_PREC == 95
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe58857b6,
		#if 2*BIG_MAN_PREC == 96
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xac7d5f42,
		#if 2*BIG_MAN_PREC == 97
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd69f6d18,
		#if 2*BIG_MAN_PREC == 98
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7763cf1d,
		#if 2*BIG_MAN_PREC == 99
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x55034004,
		#if 2*BIG_MAN_PREC == 100
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x87f55ba5,
		#if 2*BIG_MAN_PREC == 101
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7e31cc7a,
		#if 2*BIG_MAN_PREC == 102
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7135c886,
		#if 2*BIG_MAN_PREC == 103
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xefb4318a,
		#if 2*BIG_MAN_PREC == 104
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xed6a1e01,
		#if 2*BIG_MAN_PREC == 105
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2d9e6832,
		#if 2*BIG_MAN_PREC == 106
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa907600a,
		#if 2*BIG_MAN_PREC == 107
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x918130c4,
		#if 2*BIG_MAN_PREC == 108
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6dc778f9,
		#if 2*BIG_MAN_PREC == 109
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x71ad0038,
		#if 2*BIG_MAN_PREC == 110
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x092999a3,
		#if 2*BIG_MAN_PREC == 111
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x33cb8b7a,
		#if 2*BIG_MAN_PREC == 112
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x1a1db93d,
		#if 2*BIG_MAN_PREC == 113
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7140003c,
		#if 2*BIG_MAN_PREC == 114
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2a4ecea9,
		#if 2*BIG_MAN_PREC == 115
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf98d0acc,
		#if 2*BIG_MAN_PREC == 116
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0a8291cd,
		#if 2*BIG_MAN_PREC == 117
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcec97dcf,
		#if 2*BIG_MAN_PREC == 118
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8ec9b55a,
		#if 2*BIG_MAN_PREC == 119
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7f88a46b,
		#if 2*BIG_MAN_PREC == 120
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x4db5a851,
		#if 2*BIG_MAN_PREC == 121
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf44182e1,
		#if 2*BIG_MAN_PREC == 122
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc68a007e,
		#if 2*BIG_MAN_PREC == 123
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5e0dd902,
		#if 2*BIG_MAN_PREC == 124
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0bfd64b6,
		#if 2*BIG_MAN_PREC == 125
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x45036c7a,
		#if 2*BIG_MAN_PREC == 126
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4e677d2c,
		#if 2*BIG_MAN_PREC == 127
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x38532a3a,
		#if 2*BIG_MAN_PREC == 128
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x23ba4442,
		#if 2*BIG_MAN_PREC == 129
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcaf53ea6,
		#if 2*BIG_MAN_PREC == 130
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3bb45432,
		#if 2*BIG_MAN_PREC == 131
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9b7624c8,
		#if 2*BIG_MAN_PREC == 132
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x917bdd64,
		#if 2*BIG_MAN_PREC == 133
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb1c0fd4c,
		#if 2*BIG_MAN_PREC == 134
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb38e8c33,
		#if 2*BIG_MAN_PREC == 135
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4c701c3a,
		#if 2*BIG_MAN_PREC == 136
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xcdad0657,
		#if 2*BIG_MAN_PREC == 137
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfccfec71,
		#if 2*BIG_MAN_PREC == 138
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9b1f5c3e,
		#if 2*BIG_MAN_PREC == 139
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4e46041f,
		#if 2*BIG_MAN_PREC == 140
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x388147fb,
		#if 2*BIG_MAN_PREC == 141
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4cfdb477,
		#if 2*BIG_MAN_PREC == 142
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa52471f7,
		#if 2*BIG_MAN_PREC == 143
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa9a96910,
		#if 2*BIG_MAN_PREC == 144
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xb855322e,
		#if 2*BIG_MAN_PREC == 145
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdb6340d8,
		#if 2*BIG_MAN_PREC == 146
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa00ef092,
		#if 2*BIG_MAN_PREC == 147
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x350511e3,
		#if 2*BIG_MAN_PREC == 148
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0abec1ff,
		#if 2*BIG_MAN_PREC == 149
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf9e3a26e,
		#if 2*BIG_MAN_PREC == 150
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7fb29f8c,
		#if 2*BIG_MAN_PREC == 151
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x183023c3,
		#if 2*BIG_MAN_PREC == 152
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x587e38da,
		#if 2*BIG_MAN_PREC == 153
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0077d9b4,
		#if 2*BIG_MAN_PREC == 154
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x763e4e4b,
		#if 2*BIG_MAN_PREC == 155
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x94b2bbc1,
		#if 2*BIG_MAN_PREC == 156
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x94c6651e,
		#if 2*BIG_MAN_PREC == 157
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x77caf992,
		#if 2*BIG_MAN_PREC == 158
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xeeaac023,
		#if 2*BIG_MAN_PREC == 159
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2a281bf6,
		#if 2*BIG_MAN_PREC == 160
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xb3a739c1,
		#if 2*BIG_MAN_PREC == 161
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x22611682,
		#if 2*BIG_MAN_PREC == 162
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0ae8db58,
		#if 2*BIG_MAN_PREC == 163
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x47a67cbe,
		#if 2*BIG_MAN_PREC == 164
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf9c9091b,
		#if 2*BIG_MAN_PREC == 165
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x462d538c,
		#if 2*BIG_MAN_PREC == 166
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd72b0374,
		#if 2*BIG_MAN_PREC == 167
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6ae77f5e,
		#if 2*BIG_MAN_PREC == 168
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x62292c31,
		#if 2*BIG_MAN_PREC == 169
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1562a846,
		#if 2*BIG_MAN_PREC == 170
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x505dc82d,
		#if 2*BIG_MAN_PREC == 171
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb854338a,
		#if 2*BIG_MAN_PREC == 172
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe49f5235,
		#if 2*BIG_MAN_PREC == 173
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc95b9117,
		#if 2*BIG_MAN_PREC == 174
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8ccf2dd5,
		#if 2*BIG_MAN_PREC == 175
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcacef403,
		#if 2*BIG_MAN_PREC == 176
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xec9d1810,
		#if 2*BIG_MAN_PREC == 177
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc6272b04,
		#if 2*BIG_MAN_PREC == 178
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5b3b71f9,
		#if 2*BIG_MAN_PREC == 179
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdc6b80d6,
		#if 2*BIG_MAN_PREC == 180
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3fdd4a8e,
		#if 2*BIG_MAN_PREC == 181
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9adb1e69,
		#if 2*BIG_MAN_PREC == 182
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x62a69526,
		#if 2*BIG_MAN_PREC == 183
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd43161c1,
		#if 2*BIG_MAN_PREC == 184
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xa41d570d,
		#if 2*BIG_MAN_PREC == 185
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7938dad4,
		#if 2*BIG_MAN_PREC == 186
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa40e329c,
		#if 2*BIG_MAN_PREC == 187
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcff46aaa,
		#if 2*BIG_MAN_PREC == 188
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x36ad004c,
		#if 2*BIG_MAN_PREC == 189
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf600c838,
		#if 2*BIG_MAN_PREC == 190
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1e425a31,
		#if 2*BIG_MAN_PREC == 191
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd951ae64,
		#if 2*BIG_MAN_PREC == 192
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0xfdb23fce,
		#if 2*BIG_MAN_PREC == 193
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc9509d43,
		#if 2*BIG_MAN_PREC == 194
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x687feb69,
		#if 2*BIG_MAN_PREC == 195
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xedd1cc5e,
		#if 2*BIG_MAN_PREC == 196
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0b8cc3bd,
		#if 2*BIG_MAN_PREC == 197
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf64b10ef,
		#if 2*BIG_MAN_PREC == 198
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x86b63142,
		#if 2*BIG_MAN_PREC == 199
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa3ab8829,
		#if 2*BIG_MAN_PREC == 200
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x555b2f74,
		#if 2*BIG_MAN_PREC == 201
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7c932665,
		#if 2*BIG_MAN_PREC == 202
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcb2c0f1c,
		#if 2*BIG_MAN_PREC == 203
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc01bd702,
		#if 2*BIG_MAN_PREC == 204
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x29388839,
		#if 2*BIG_MAN_PREC == 205
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd2af05e4,
		#if 2*BIG_MAN_PREC == 206
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x54504ac7,
		#if 2*BIG_MAN_PREC == 207
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8b758282,
		#if 2*BIG_MAN_PREC == 208
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x2846c0ba,
		#if 2*BIG_MAN_PREC == 209
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x35c35f5c,
		#if 2*BIG_MAN_PREC == 210
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x59160cc0,
		#if 2*BIG_MAN_PREC == 211
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x46fd8251,
		#if 2*BIG_MAN_PREC == 212
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x541fc68c,
		#if 2*BIG_MAN_PREC == 213
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9c86b022,
		#if 2*BIG_MAN_PREC == 214
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbb709987,
		#if 2*BIG_MAN_PREC == 215
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6a460e74,
		#if 2*BIG_MAN_PREC == 216
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x51a8a931,
		#if 2*BIG_MAN_PREC == 217
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x09703fee,
		#if 2*BIG_MAN_PREC == 218
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1c217e6c,
		#if 2*BIG_MAN_PREC == 219
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3826e52c,
		#if 2*BIG_MAN_PREC == 220
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x51aa691e,
		#if 2*BIG_MAN_PREC == 221
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0e423cfc,
		#if 2*BIG_MAN_PREC == 222
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x99e9e316,
		#if 2*BIG_MAN_PREC == 223
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x50c1217b,
		#if 2*BIG_MAN_PREC == 224
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x624816cd,
		#if 2*BIG_MAN_PREC == 225
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xad9a95f9,
		#if 2*BIG_MAN_PREC == 226
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd5b80194,
		#if 2*BIG_MAN_PREC == 227
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x88d9c0a0,
		#if 2*BIG_MAN_PREC == 228
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa1fe3075,
		#if 2*BIG_MAN_PREC == 229
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa577e231,
		#if 2*BIG_MAN_PREC == 230
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x83f81d4a,
		#if 2*BIG_MAN_PREC == 231
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3f2fa457,
		#if 2*BIG_MAN_PREC == 232
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x1efc8ce0,
		#if 2*BIG_MAN_PREC == 233
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xba8a4fe8,
		#if 2*BIG_MAN_PREC == 234
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb6855dfe,
		#if 2*BIG_MAN_PREC == 235
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x72b0a66e,
		#if 2*BIG_MAN_PREC == 236
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xded2fbab,
		#if 2*BIG_MAN_PREC == 237
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfbe58a30,
		#if 2*BIG_MAN_PREC == 238
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfafabe1c,
		#if 2*BIG_MAN_PREC == 239
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5d71a87e,
		#if 2*BIG_MAN_PREC == 240
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x2f741ef8,
		#if 2*BIG_MAN_PREC == 241
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc1fe86fe,
		#if 2*BIG_MAN_PREC == 242
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa6bbfde5,
		#if 2*BIG_MAN_PREC == 243
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x30677f0d,
		#if 2*BIG_MAN_PREC == 244
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x97d11d49,
		#if 2*BIG_MAN_PREC == 245
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf7a8443d,
		#if 2*BIG_MAN_PREC == 246
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0822e506,
		#if 2*BIG_MAN_PREC == 247
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa9f4614e,
		#if 2*BIG_MAN_PREC == 248
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif 
		0x011e2a94,
		#if 2*BIG_MAN_PREC == 249
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x838ff88c,
		#if 2*BIG_MAN_PREC == 250
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd68c8bb7,
		#if 2*BIG_MAN_PREC == 251
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc51eef6d,
		#if 2*BIG_MAN_PREC == 252
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x49ea8ab4,
		#if 2*BIG_MAN_PREC == 253
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf2c3df5b,
		#if 2*BIG_MAN_PREC == 254
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb4e0735a,
		#if 2*BIG_MAN_PREC == 255
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb0d68749
	// 0x2fe26dd4, ...
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};

	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using ExpSurrounding0(1) method
	// which took 1420 iterations
	// (the result was compared with e taken from http://antwrp.gsfc.nasa.gov/htmltest/gifcity/e.2mil)

	big_uint_set_from_table(_big_num_ref(self->mantissa), temp_table, BIG_MAN_PREC);
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, -_big_num_sstrg_t(self->mantissa.size)*_big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT) + 2);
	self->info = 0;
}

/**
 * this method sets the value of ln(2)
 * the natural logarithm from 2
 * @param[in] self the big num object
 */
void big_float_set_ln2(_big_num_inout(big_float_t, self))
{
	_big_num_static const big_num_strg_t temp_table[2*BIG_MAN_PREC] = {
		0xb17217f7, 
		#if 2*BIG_MAN_PREC == 1
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd1cf79ab, 
		#if 2*BIG_MAN_PREC == 2
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc9e3b398, 
		#if 2*BIG_MAN_PREC == 3
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x03f2f6af, 
		#if 2*BIG_MAN_PREC == 4
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x40f34326, 
		#if 2*BIG_MAN_PREC == 5
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7298b62d, 
		#if 2*BIG_MAN_PREC == 6
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8a0d175b, 
		#if 2*BIG_MAN_PREC == 7
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8baafa2b, 
		#if 2*BIG_MAN_PREC == 8
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe7b87620, 
		#if 2*BIG_MAN_PREC == 9
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6debac98, 
		#if 2*BIG_MAN_PREC == 10
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x559552fb, 
		#if 2*BIG_MAN_PREC == 11
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4afa1b10, 
		#if 2*BIG_MAN_PREC == 12
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xed2eae35, 
		#if 2*BIG_MAN_PREC == 13
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc1382144, 
		#if 2*BIG_MAN_PREC == 14
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x27573b29, 
		#if 2*BIG_MAN_PREC == 15
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1169b825, 
		#if 2*BIG_MAN_PREC == 16
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3e96ca16, 
		#if 2*BIG_MAN_PREC == 17
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x224ae8c5, 
		#if 2*BIG_MAN_PREC == 18
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1acbda11, 
		#if 2*BIG_MAN_PREC == 19
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x317c387e, 
		#if 2*BIG_MAN_PREC == 20
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb9ea9bc3, 
		#if 2*BIG_MAN_PREC == 21
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb136603b, 
		#if 2*BIG_MAN_PREC == 22
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x256fa0ec, 
		#if 2*BIG_MAN_PREC == 23
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7657f74b, 
		#if 2*BIG_MAN_PREC == 24
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x72ce87b1, 
		#if 2*BIG_MAN_PREC == 25
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9d6548ca, 
		#if 2*BIG_MAN_PREC == 26
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf5dfa6bd, 
		#if 2*BIG_MAN_PREC == 27
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x38303248, 
		#if 2*BIG_MAN_PREC == 28
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x655fa187, 
		#if 2*BIG_MAN_PREC == 29
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2f20e3a2, 
		#if 2*BIG_MAN_PREC == 30
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xda2d97c5, 
		#if 2*BIG_MAN_PREC == 31
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0f3fd5c6, 
		#if 2*BIG_MAN_PREC == 32
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x07f4ca11, 
		#if 2*BIG_MAN_PREC == 33
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfb5bfb90, 
		#if 2*BIG_MAN_PREC == 34
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x610d30f8, 
		#if 2*BIG_MAN_PREC == 35
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8fe551a2, 
		#if 2*BIG_MAN_PREC == 36
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xee569d6d, 
		#if 2*BIG_MAN_PREC == 37
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfc1efa15, 
		#if 2*BIG_MAN_PREC == 38
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7d2e23de, 
		#if 2*BIG_MAN_PREC == 39
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1400b396, 
		#if 2*BIG_MAN_PREC == 40
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x17460775, 
		#if 2*BIG_MAN_PREC == 41
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdb8990e5, 
		#if 2*BIG_MAN_PREC == 42
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc943e732, 
		#if 2*BIG_MAN_PREC == 43
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb479cd33, 
		#if 2*BIG_MAN_PREC == 44
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcccc4e65, 
		#if 2*BIG_MAN_PREC == 45
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9393514c, 
		#if 2*BIG_MAN_PREC == 46
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4c1a1e0b, 
		#if 2*BIG_MAN_PREC == 47
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd1d6095d, 
		#if 2*BIG_MAN_PREC == 48
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x25669b33, 
		#if 2*BIG_MAN_PREC == 49
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3564a337, 
		#if 2*BIG_MAN_PREC == 50
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6a9c7f8a, 
		#if 2*BIG_MAN_PREC == 51
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5e148e82, 
		#if 2*BIG_MAN_PREC == 52
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x074db601, 
		#if 2*BIG_MAN_PREC == 53
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5cfe7aa3, 
		#if 2*BIG_MAN_PREC == 54
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0c480a54, 
		#if 2*BIG_MAN_PREC == 55
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x17350d2c, 
		#if 2*BIG_MAN_PREC == 56
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x955d5179, 
		#if 2*BIG_MAN_PREC == 57
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb1e17b9d, 
		#if 2*BIG_MAN_PREC == 58
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xae313cdb, 
		#if 2*BIG_MAN_PREC == 59
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6c606cb1, 
		#if 2*BIG_MAN_PREC == 60
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x078f735d, 
		#if 2*BIG_MAN_PREC == 61
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1b2db31b, 
		#if 2*BIG_MAN_PREC == 62
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5f50b518, 
		#if 2*BIG_MAN_PREC == 63
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5064c18b, 
		#if 2*BIG_MAN_PREC == 64
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4d162db3, 
		#if 2*BIG_MAN_PREC == 65
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb365853d, 
		#if 2*BIG_MAN_PREC == 66
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x7598a195, 
		#if 2*BIG_MAN_PREC == 67
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1ae273ee, 
		#if 2*BIG_MAN_PREC == 68
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5570b6c6, 
		#if 2*BIG_MAN_PREC == 69
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8f969834, 
		#if 2*BIG_MAN_PREC == 70
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x96d4e6d3, 
		#if 2*BIG_MAN_PREC == 71
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x30af889b, 
		#if 2*BIG_MAN_PREC == 72
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x44a02554, 
		#if 2*BIG_MAN_PREC == 73
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x731cdc8e, 
		#if 2*BIG_MAN_PREC == 74
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa17293d1, 
		#if 2*BIG_MAN_PREC == 75
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x228a4ef9, 
		#if 2*BIG_MAN_PREC == 76
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8d6f5177, 
		#if 2*BIG_MAN_PREC == 77
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfbcf0755, 
		#if 2*BIG_MAN_PREC == 78
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x268a5c1f, 
		#if 2*BIG_MAN_PREC == 79
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9538b982, 
		#if 2*BIG_MAN_PREC == 80
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x61affd44, 
		#if 2*BIG_MAN_PREC == 81
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6b1ca3cf, 
		#if 2*BIG_MAN_PREC == 82
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5e9222b8, 
		#if 2*BIG_MAN_PREC == 83
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8c66d3c5, 
		#if 2*BIG_MAN_PREC == 84
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x422183ed, 
		#if 2*BIG_MAN_PREC == 85
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc9942109, 
		#if 2*BIG_MAN_PREC == 86
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0bbb16fa, 
		#if 2*BIG_MAN_PREC == 87
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf3d949f2, 
		#if 2*BIG_MAN_PREC == 88
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x36e02b20, 
		#if 2*BIG_MAN_PREC == 89
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcee886b9, 
		#if 2*BIG_MAN_PREC == 90
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x05c128d5, 
		#if 2*BIG_MAN_PREC == 91
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3d0bd2f9, 
		#if 2*BIG_MAN_PREC == 92
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x62136319, 
		#if 2*BIG_MAN_PREC == 93
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6af50302, 
		#if 2*BIG_MAN_PREC == 94
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0060e499, 
		#if 2*BIG_MAN_PREC == 95
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x08391a0c, 
		#if 2*BIG_MAN_PREC == 96
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x57339ba2, 
		#if 2*BIG_MAN_PREC == 97
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbeba7d05, 
		#if 2*BIG_MAN_PREC == 98
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2ac5b61c, 
		#if 2*BIG_MAN_PREC == 99
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc4e9207c, 
		#if 2*BIG_MAN_PREC == 100
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xef2f0ce2, 
		#if 2*BIG_MAN_PREC == 101
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd7373958, 
		#if 2*BIG_MAN_PREC == 102
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd7622658, 
		#if 2*BIG_MAN_PREC == 103
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x901e646a, 
		#if 2*BIG_MAN_PREC == 104
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x95184460, 
		#if 2*BIG_MAN_PREC == 105
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdc4e7487, 
		#if 2*BIG_MAN_PREC == 106
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x156e0c29, 
		#if 2*BIG_MAN_PREC == 107
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2413d5e3, 
		#if 2*BIG_MAN_PREC == 108
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x61c1696d, 
		#if 2*BIG_MAN_PREC == 109
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd24aaebd, 
		#if 2*BIG_MAN_PREC == 110
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x473826fd, 
		#if 2*BIG_MAN_PREC == 111
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa0c238b9, 
		#if 2*BIG_MAN_PREC == 112
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0ab111bb, 
		#if 2*BIG_MAN_PREC == 113
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbd67c724, 
		#if 2*BIG_MAN_PREC == 114
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x972cd18b, 
		#if 2*BIG_MAN_PREC == 115
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfbbd9d42, 
		#if 2*BIG_MAN_PREC == 116
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6c472096, 
		#if 2*BIG_MAN_PREC == 117
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe76115c0, 
		#if 2*BIG_MAN_PREC == 118
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5f6f7ceb, 
		#if 2*BIG_MAN_PREC == 119
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xac9f45ae, 
		#if 2*BIG_MAN_PREC == 120
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcecb72f1, 
		#if 2*BIG_MAN_PREC == 121
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9c38339d, 
		#if 2*BIG_MAN_PREC == 122
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8f682625, 
		#if 2*BIG_MAN_PREC == 123
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0dea891e, 
		#if 2*BIG_MAN_PREC == 124
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf07afff3, 
		#if 2*BIG_MAN_PREC == 125
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa892374e, 
		#if 2*BIG_MAN_PREC == 126
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x175eb4af, 
		#if 2*BIG_MAN_PREC == 127
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc8daadd8, 
		#if 2*BIG_MAN_PREC == 128
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x85db6ab0, 
		#if 2*BIG_MAN_PREC == 129
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3a49bd0d, 
		#if 2*BIG_MAN_PREC == 130
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc0b1b31d, 
		#if 2*BIG_MAN_PREC == 131
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8a0e23fa, 
		#if 2*BIG_MAN_PREC == 132
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc5e5767d, 
		#if 2*BIG_MAN_PREC == 133
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf95884e0, 
		#if 2*BIG_MAN_PREC == 134
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6425a415, 
		#if 2*BIG_MAN_PREC == 135
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x26fac51c, 
		#if 2*BIG_MAN_PREC == 136
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3ea8449f, 
		#if 2*BIG_MAN_PREC == 137
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe8f70edd, 
		#if 2*BIG_MAN_PREC == 138
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x062b1a63, 
		#if 2*BIG_MAN_PREC == 139
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa6c4c60c, 
		#if 2*BIG_MAN_PREC == 140
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x52ab3316, 
		#if 2*BIG_MAN_PREC == 141
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1e238438, 
		#if 2*BIG_MAN_PREC == 142
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x897a39ce, 
		#if 2*BIG_MAN_PREC == 143
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x78b63c9f, 
		#if 2*BIG_MAN_PREC == 144
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x364f5b8a, 
		#if 2*BIG_MAN_PREC == 145
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xef22ec2f, 
		#if 2*BIG_MAN_PREC == 146
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xee6e0850, 
		#if 2*BIG_MAN_PREC == 147
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xeca42d06, 
		#if 2*BIG_MAN_PREC == 148
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfb0c75df, 
		#if 2*BIG_MAN_PREC == 149
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5497e00c, 
		#if 2*BIG_MAN_PREC == 150
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x554b03d7, 
		#if 2*BIG_MAN_PREC == 151
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd2874a00, 
		#if 2*BIG_MAN_PREC == 152
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0ca8f58d, 
		#if 2*BIG_MAN_PREC == 153
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x94f0341c, 
		#if 2*BIG_MAN_PREC == 154
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbe2ec921, 
		#if 2*BIG_MAN_PREC == 155
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x56c9f949, 
		#if 2*BIG_MAN_PREC == 156
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdb4a9316, 
		#if 2*BIG_MAN_PREC == 157
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf281501e, 
		#if 2*BIG_MAN_PREC == 158
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x53daec3f, 
		#if 2*BIG_MAN_PREC == 159
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x64f1b783, 
		#if 2*BIG_MAN_PREC == 160
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x154c6032, 
		#if 2*BIG_MAN_PREC == 161
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0e2ff793, 
		#if 2*BIG_MAN_PREC == 162
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x33ce3573, 
		#if 2*BIG_MAN_PREC == 163
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfacc5fdc, 
		#if 2*BIG_MAN_PREC == 164
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf1178590, 
		#if 2*BIG_MAN_PREC == 165
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3155bbd9, 
		#if 2*BIG_MAN_PREC == 166
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0f023b22, 
		#if 2*BIG_MAN_PREC == 167
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x0224fcd8, 
		#if 2*BIG_MAN_PREC == 168
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x471bf4f4, 
		#if 2*BIG_MAN_PREC == 169
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x45f0a88a, 
		#if 2*BIG_MAN_PREC == 170
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x14f0cd97, 
		#if 2*BIG_MAN_PREC == 171
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6ea354bb, 
		#if 2*BIG_MAN_PREC == 172
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x20cdb5cc, 
		#if 2*BIG_MAN_PREC == 173
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb3db2392, 
		#if 2*BIG_MAN_PREC == 174
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x88d58655, 
		#if 2*BIG_MAN_PREC == 175
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4e2a0e8a, 
		#if 2*BIG_MAN_PREC == 176
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6fe51a8c, 
		#if 2*BIG_MAN_PREC == 177
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfaa72ef2, 
		#if 2*BIG_MAN_PREC == 178
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xad8a43dc, 
		#if 2*BIG_MAN_PREC == 179
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4212b210, 
		#if 2*BIG_MAN_PREC == 180
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb779dfe4, 
		#if 2*BIG_MAN_PREC == 181
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x9d7307cc, 
		#if 2*BIG_MAN_PREC == 182
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x846532e4, 
		#if 2*BIG_MAN_PREC == 183
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb9694eda, 
		#if 2*BIG_MAN_PREC == 184
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd162af05, 
		#if 2*BIG_MAN_PREC == 185
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3b1751f3, 
		#if 2*BIG_MAN_PREC == 186
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa3d091f6, 
		#if 2*BIG_MAN_PREC == 187
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x56658154, 
		#if 2*BIG_MAN_PREC == 188
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x12b5e8c2, 
		#if 2*BIG_MAN_PREC == 189
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x02461069, 
		#if 2*BIG_MAN_PREC == 190
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xac14b958, 
		#if 2*BIG_MAN_PREC == 191
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x784934b8, 
		#if 2*BIG_MAN_PREC == 192
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd6cce1da, 
		#if 2*BIG_MAN_PREC == 193
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa5053701, 
		#if 2*BIG_MAN_PREC == 194
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1aa4fb42, 
		#if 2*BIG_MAN_PREC == 195
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb9a3def4, 
		#if 2*BIG_MAN_PREC == 196
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1bda1f85, 
		#if 2*BIG_MAN_PREC == 197
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xef6fdbf2, 
		#if 2*BIG_MAN_PREC == 198
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf2d89d2a, 
		#if 2*BIG_MAN_PREC == 199
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4b183527, 
		#if 2*BIG_MAN_PREC == 200
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8fd94057, 
		#if 2*BIG_MAN_PREC == 201
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x89f45681, 
		#if 2*BIG_MAN_PREC == 202
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x2b552879, 
		#if 2*BIG_MAN_PREC == 203
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xa6168695, 
		#if 2*BIG_MAN_PREC == 204
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc12963b0, 
		#if 2*BIG_MAN_PREC == 205
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xff01eaab, 
		#if 2*BIG_MAN_PREC == 206
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x73e5b5c1, 
		#if 2*BIG_MAN_PREC == 207
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x585318e7, 
		#if 2*BIG_MAN_PREC == 208
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x624f14a5, 
		#if 2*BIG_MAN_PREC == 209
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1a4a026b, 
		#if 2*BIG_MAN_PREC == 210
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x68082920, 
		#if 2*BIG_MAN_PREC == 211
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x57fd99b6, 
		#if 2*BIG_MAN_PREC == 212
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x6dc085a9, 
		#if 2*BIG_MAN_PREC == 213
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8ac8d8ca, 
		#if 2*BIG_MAN_PREC == 214
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf9eeeea9, 
		#if 2*BIG_MAN_PREC == 215
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8a2400ca, 
		#if 2*BIG_MAN_PREC == 216
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc95f260f, 
		#if 2*BIG_MAN_PREC == 217
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd10036f9, 
		#if 2*BIG_MAN_PREC == 218
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xf91096ac, 
		#if 2*BIG_MAN_PREC == 219
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x3195220a, 
		#if 2*BIG_MAN_PREC == 220
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1a356b2a, 
		#if 2*BIG_MAN_PREC == 221
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x73b7eaad, 
		#if 2*BIG_MAN_PREC == 222
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xaf6d6058, 
		#if 2*BIG_MAN_PREC == 223
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x71ef7afb, 
		#if 2*BIG_MAN_PREC == 224
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x80bc4234, 
		#if 2*BIG_MAN_PREC == 225
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x33562e94, 
		#if 2*BIG_MAN_PREC == 226
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xb12dfab4, 
		#if 2*BIG_MAN_PREC == 227
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x14451579, 
		#if 2*BIG_MAN_PREC == 228
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xdf59eae0, 
		#if 2*BIG_MAN_PREC == 229
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x51707062, 
		#if 2*BIG_MAN_PREC == 230
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x4012a829, 
		#if 2*BIG_MAN_PREC == 231
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x62c59cab, 
		#if 2*BIG_MAN_PREC == 232
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x347f8304, 
		#if 2*BIG_MAN_PREC == 233
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd889659e, 
		#if 2*BIG_MAN_PREC == 234
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x5a9139db, 
		#if 2*BIG_MAN_PREC == 235
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x14efcc30, 
		#if 2*BIG_MAN_PREC == 236
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x852be3e8, 
		#if 2*BIG_MAN_PREC == 237
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xfc99f14d, 
		#if 2*BIG_MAN_PREC == 238
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1d822dd6, 
		#if 2*BIG_MAN_PREC == 239
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe2f76797, 
		#if 2*BIG_MAN_PREC == 240
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xe30219c8, 
		#if 2*BIG_MAN_PREC == 241
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xaa9ce884, 
		#if 2*BIG_MAN_PREC == 242
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x8a886eb3, 
		#if 2*BIG_MAN_PREC == 243
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xc87b7295, 
		#if 2*BIG_MAN_PREC == 244
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x988012e8, 
		#if 2*BIG_MAN_PREC == 245
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x314186ed, 
		#if 2*BIG_MAN_PREC == 246
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xbaf86856, 
		#if 2*BIG_MAN_PREC == 247
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xccd3c3b6, 
		#if 2*BIG_MAN_PREC == 248
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xee94e62f, 
		#if 2*BIG_MAN_PREC == 249
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x110a6783, 
		#if 2*BIG_MAN_PREC == 250
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd2aae89c, 
		#if 2*BIG_MAN_PREC == 251
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xcc3b76fc, 
		#if 2*BIG_MAN_PREC == 252
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x435a0ce1, 
		#if 2*BIG_MAN_PREC == 253
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x34c2838f, 
		#if 2*BIG_MAN_PREC == 254
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0xd571ec6c, 
		#if 2*BIG_MAN_PREC == 255
		}; _big_num_static const big_num_strg_t _temp_table[] = {
		#endif
		0x1366a993 // last one was: 0x1366a992
	//0xcbb9ac40, ...
	// (the last word 0x1366a992 was rounded up because the next one is 0xcbb9ac40 -- first bit is one 0xc..)
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};	
	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using LnSurrounding1(2) method
	// which took 4035 iterations
	// (the result was compared with ln(2) taken from http://ja0hxv.calico.jp/pai/estart.html)

	big_uint_set_from_table(_big_num_ref(self->mantissa), temp_table, 2*BIG_MAN_PREC);
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, -_big_num_sstrg_t(self->mantissa.size)*_big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT));
	self->info = 0;
}

/**
 * this method sets NaN flag
 * @param[in, out] self the big num object
 */
void big_float_set_nan(_big_num_inout(big_float_t, self))
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method sets nan flag, also clears mantissa and exponent
 * @param[in, out] self the big num object
 */
void big_float_set_zero_nan(_big_num_inout(big_float_t, self))
{
	big_float_set_zero(self);
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method swappes this for an argument
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the big num to swap with
 */
void big_float_swap(_big_num_inout(big_float_t, self), _big_num_inout(big_float_t, ss2))
{
	big_float_info_t info_temp = self->info;
	self->info = ss2->info;
	ss2->info = info_temp;

	big_int_swap(_big_num_ref(self->exponent), _big_num_ref(ss2->exponent));
	big_uint_swap(_big_num_ref(self->mantissa), _big_num_ref(ss2->mantissa));
}

/**
 * this method sets the maximum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_max(_big_num_inout(big_float_t, self))
{
	self->info = 0;
	big_int_set_max(_big_num_ref(self->mantissa));
	big_uint_set_max(_big_num_ref(self->exponent));
}

/**
 * this method sets the minimum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_min(_big_num_inout(big_float_t, self))
{
	self->info = 0;
	big_int_set_max(_big_num_ref(self->mantissa));
	big_uint_set_max(_big_num_ref(self->exponent));
	big_float_set_sign(self);
}

/**
 * test whether there is a value zero or not
 * @param[in] self the big num object 
 * @return true the big num object is zero
 * @return false the big num object is not zero
 */
bool big_float_is_zero(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_ZERO);
}

/**
 * this method returns true when there's the sign set
 * also we don't check the NaN flag
 * @param[in] self the big num object 
 * @return true the big num object is negative
 * @return false the big num object is positive
 */
bool big_float_is_sign(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * this method returns true when there is not a valid number
 * @param[in] self the big num object 
 * @return true the big num object is nan
 * @return false the big num object is not nan
 */
bool big_float_is_nan(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method clears the sign 
 * (there'll be an absolute value)
 * 
 * samples
 * -  	-1 -> 1
 * -  	2  -> 2
 * @param[in, out] self the big num object 
 */
void big_float_abs(_big_num_inout(big_float_t, self))
{
	_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);	
}

/**
 * this method rounds to the nearest integer value
 * (it returns a carry if it was)
 * 
 * samples:
 * -	2.3 = 2
 * -	2.8 = 3
 * -	-2.3 = -2
 * -	-2.8 = 3
 * @param[in] self the big num object
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_round(_big_num_inout(big_float_t, self))
{
	big_float_t _half;
	big_num_carry_t c;
	if (big_float_is_nan(_big_num_deref(self)))
		return 1;
	if (big_float_is_zero(_big_num_deref(self)))
		return 0;
	
	big_float_init(_big_num_ref(_half), self->mantissa.size, self->exponent.size);
	big_float_set_05(_big_num_ref(_half));

	if (big_float_is_sign(_big_num_deref(self))) {
		// self < 0
		c = big_float_sub(self, _half, true);
	} else {
		// self > 0
		c = big_float_add(self, _half, true);
	}

	_big_float_skip_fraction(self);

	return _big_float_check_carry(self, c);
}

/**
 * this method remains the 'sign' of the value
 * 
 * samples
 * -    -2 = -1
 * -     0 = 0
 * -    10 = 1
 * @param[in, out] self the big num object 
 */
void big_float_sgn(_big_num_inout(big_float_t, self))
{
	if (big_float_is_nan(_big_num_deref(self)) )
		return;
	if (big_float_is_sign(_big_num_deref(self))) {
		big_float_set_one(self);
		big_float_set_sign(self);
	} else if (big_float_is_zero(_big_num_deref(self))) {
		big_float_set_zero(self);
	} else {
		big_float_set_one(self);
	}
}

/**
 * this method sets the sign
 * 
 * samples
 * -  	-1 -> -1
 * -  	2  -> -2
 * 
 * we do not check whether there is a zero or not, if you're using this method
 * you must be sure that the value is (or will be afterwards) different from zero
 * @param[in, out] self the big num object 
 */
void big_float_set_sign(_big_num_inout(big_float_t, self))
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * this method changes the sign
 * when there is a value of zero then the sign is not changed
 * 
 * samples
 * -  	-1 ->  1
 * -  	 2 -> -2
 * @param[in, out] self the big num object 
 */
void big_float_change_sign(_big_num_inout(big_float_t, self))
{
	// we don't have to check the NaN flag here
	if( big_float_is_zero(_big_num_deref(self)) )
		return;

	if( big_float_is_sign(_big_num_deref(self)) )
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);
	else
		_big_float_set_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * Addition self = self + ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round true if round result
 * @return big_num_carry_t >0 if the result is too big
 */
big_num_carry_t big_float_add(_big_num_inout(big_float_t, self), big_float_t ss2, bool round)
{
	return _big_float_add(self, ss2, round, true);
}

/**
 * Subtraction self = self - ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round true if round result
 * @return big_num_carry_t >0 if the result is too big
 */
big_num_carry_t big_float_sub(_big_num_inout(big_float_t, self), big_float_t ss2, bool round)
{
	return _big_float_add(self, ss2, round, false);
}

/**
 * Multiplication this = this * ss2 (ss2 is uint)
 * @param[in, out] self the big num object 
 * @param[in] ss2 ss2 without a sign
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul_uint(_big_num_inout(big_float_t, self), big_num_strg_t ss2)
{
	big_uint_t man_result;
	big_uint_init(_big_num_ref(man_result), 1 + self->mantissa.size);
	size_t i;
	big_num_carry_t c = 0;

	if (big_float_is_nan(_big_num_deref(self)))
		return 1;
	if (big_float_is_zero(_big_num_deref(self)))
		return 0;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	for(i=0 ; i<self->mantissa.size ; ++i)
		man_result.table[i] = self->mantissa.table[i];
	for (; i < 2*BIG_MAN_PREC; ++i)
		man_result.table[i] = 0;
	big_num_carry_t man_c = big_uint_mul_int(_big_num_ref(man_result), ss2);

	big_num_sstrg_t bit = _big_uint_find_leading_bit_in_word(man_result.table[self->mantissa.size]);

	if( bit!=-1 && _big_num_strg_t(bit) > (BIG_NUM_BITS_PER_UNIT/2) ) {
		// 'i' will be from 0 to BIG_NUM_BITS_PER_UNIT
		i = big_uint_compensation_to_left(_big_num_ref(man_result));
		c = big_int_add_int(_big_num_ref(self->exponent), BIG_NUM_BITS_PER_UNIT - i, 0);

		for(i=0 ; i < self->mantissa.size ; ++i)
			self->mantissa.table[i] = man_result.table[i+1];
	} else {
		if( bit != -1 ) {
			big_uint_rcr(_big_num_ref(man_result), bit+1, 0);
			c += big_int_add_int(_big_num_ref(self->exponent), bit+1, 0);
		}

		for(i=0 ; i < self->mantissa.size ; ++i)
			self->mantissa.table[i] = man_result.table[i];
	}

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * Multiplication this = this * ss2 (ss2 is sint)
 * @param[in, out] self the big num object
 * @param[in] ss2 ss2 with a sign
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul_int(_big_num_inout(big_float_t, self), big_num_sstrg_t ss2)
{
	if (big_float_is_nan(_big_num_deref(self)))
		return 1;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}
	if (big_float_is_zero(_big_num_deref(self)))
		return 0;
	if (big_float_is_sign(_big_num_deref(self)) == (ss2<0)) {
		// the signs are the same (both are either - or +), the result is positive
		big_float_abs(self);
	} else {
		// the signs are different, the result is negative
		big_float_set_sign(self);
	}

	if (ss2 < 0)
		ss2 = -ss2;

	return big_float_mul_uint(self, _big_num_strg_t(ss2));
}

/**
 * multiplication this = this * ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num to multiply self by
 * @param[in] round true if result should be rounded
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul(_big_num_inout(big_float_t, self), big_float_t ss2, bool round)
{
	big_uint_t man_result;
	big_uint_init(_big_num_ref(man_result), 2 * self->mantissa.size);
	big_num_carry_t c = 0;
	size_t i;

	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(_big_num_deref(self)) )
		return 0;

	if( big_float_is_zero(ss2) ) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	big_uint_mul_no_carry(_big_num_ref(self->mantissa), ss2.mantissa, _big_num_ref(man_result), BIG_NUM_MUL_DEF);

	// 'i' will be from 0 to man*BIG_NUM_BITS_PER_UNIT
	// because mantissa and ss2.mantissa are standardized 
	// (the highest bit in man_result is set to 1 or
	// if there is a zero value in man_result the method CompensationToLeft()
	// returns 0 but we'll correct this at the end in Standardizing() method)
	i = big_uint_compensation_to_left(_big_num_ref(man_result));
	big_num_strg_t exp_add = self->mantissa.size * BIG_NUM_BITS_PER_UNIT - i;

	if( exp_add != 0 )
		c += big_int_add_int(_big_num_ref(self->exponent), exp_add, 0);

	c += big_int_add(_big_num_ref(self->exponent), ss2.exponent);

	for(i=0 ; i<self->mantissa.size ; ++i)
		self->mantissa.table[i] = man_result.table[i+self->mantissa.size];

	if( round && (man_result.table[self->mantissa.size-1] & BIG_NUM_HIGHEST_BIT) != 0 ) {
		bool is_half = _big_float_check_greater_or_equal_half(self, man_result.table, self->mantissa.size);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(_big_num_deref(self)) == big_float_is_sign(ss2) ) {
		// the signs are the same, the result is positive
		big_float_abs(self);
	} else {
		// the signs are different, the result is negative
		// if the value is zero it will be corrected later in Standardizing method
		big_float_set_sign(self);
	}

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * division this = this / ss2
 * 
 * return value:
 * -  0 - ok
 * -  1 - carry (in a division carry can be as well)
 * -  2 - improper argument (ss2 is zero)
 * @param[in] self the big num object 
 * @param[in] ss2 the big num object to divide self by
 * @param[in] round true if result should be rounded
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_div(_big_num_inout(big_float_t, self), big_float_t ss2, bool round)
{
	big_uint_t man1;
	big_uint_init(_big_num_ref(man1), 2 * self->mantissa.size);
	big_uint_t man2;
	big_uint_init(_big_num_ref(man2), 2 * self->mantissa.size);
	size_t i;
	big_num_carry_t c = 0;
		
	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( big_float_is_zero(_big_num_deref(self)) )
		return 0;

	for(i=0 ; i<self->mantissa.size ; ++i) {
		man1.table[i] 				= 0;
		man1.table[i+self->mantissa.size] 	= self->mantissa.table[i];
		man2.table[i]     			= ss2.mantissa.table[i];
		man2.table[i+self->mantissa.size] 	= 0;
	}

	big_uint_t remainder;
	big_uint_init(_big_num_ref(remainder), self->mantissa.size);
	big_uint_div(_big_num_ref(man1), man2, _big_num_ref(remainder), BIG_NUM_DIV_DEF);

	i = big_uint_compensation_to_left(_big_num_ref(man1));

	if( i != 0)
		c += big_int_sub_int(_big_num_ref(self->exponent), i, 0);

	c += big_int_sub(_big_num_ref(self->exponent), ss2.exponent);
	
	for(i=0 ; i<self->mantissa.size ; ++i)
		self->mantissa.table[i] = man1.table[i+self->mantissa.size];

	if( round && (man1.table[self->mantissa.size-1] & BIG_NUM_HIGHEST_BIT) != 0 )
	{
		bool is_half = _big_float_check_greater_or_equal_half(self, man1.table, self->mantissa.size);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(_big_num_deref(self)) == big_float_is_sign(ss2) )
		big_float_abs(self);
	else
		big_float_set_sign(self); // if there is a zero it will be corrected in Standardizing()

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * caltulate the remainder from a division
 * 
 * samples
 * -   12.6 mod  3 =  0.6   because  12.6 = 3*4 + 0.6
 * -  -12.6 mod  3 = -0.6   bacause -12.6 = 3*(-4) + (-0.6)
 * -   12.6 mod -3 =  0.6
 * -  -12.6 mod -3 = -0.6
 * 
 * in other words: this(old) = ss2 * q + this(new)
 * 
 * return value:
 * -  0 - ok
 * -  1 - carry
 * -  2 - improper argument (ss2 is zero)
 * @param[in] self 
 * @param[in] ss2 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_mod(_big_num_inout(big_float_t, self), big_float_t ss2)
{
	big_num_carry_t c = 0;

	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( !_big_float_smaller_without_sign_than(_big_num_deref(self), ss2) )
	{
		big_float_t temp = _big_num_deref(self);

		c = big_float_div(_big_num_ref(temp), ss2, true);
		_big_float_skip_fraction(_big_num_ref(temp));
		c += big_float_mul(_big_num_ref(temp), ss2, true);
		c += big_float_sub(self, temp, true);

		if( !_big_float_smaller_without_sign_than(_big_num_deref(self), ss2 ) )
			c += 1;
	}

	return _big_float_check_carry(self, c);
}

/**
 * this method returns: 'this' mod 2
 * (either zero or one)
 * 
 * this method is much faster than using Mod( object_with_value_two )
 * @param[in] self 
 * @return big_num_strg_t 
 */
big_num_strg_t big_float_mod2(big_float_t self)
{	
	big_int_t zero, negative_bits;
	big_int_init(_big_num_ref(zero), self.exponent.size);
	big_int_set_zero(_big_num_ref(zero));
	big_int_init_int(_big_num_ref(negative_bits), self.exponent.size, -_big_num_sstrg_t(self.mantissa.size*BIG_NUM_BITS_PER_UNIT));

	if( big_int_cmp_bigger(self.exponent, zero) || big_int_cmp_smaller_equal(self.exponent, negative_bits) )
		return 0;

	big_num_sstrg_t exp_int;
	big_int_to_int(self.exponent, _big_num_ref(exp_int));
	// 'exp_int' is negative (or zero), we set it as positive
	exp_int = -exp_int;

	return big_uint_get_bit(self.mantissa, exp_int) ? 1 : 0;
}

/**
 * power this = this ^ pow
 * (pow without a sign)
 * 
 * binary algorithm (r-to-l)
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments (0^0)
 * @param[in] self the big num object
 * @param[in] _pow the value to raise the big num object by
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_uint(_big_num_inout(big_float_t, self), big_uint_t _pow)
{
	if( big_float_is_nan(_big_num_deref(self)) )
		return 1;

	if( big_float_is_zero(_big_num_deref(self)) ) {
		if( big_uint_is_zero(_pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	big_float_t start = _big_num_deref(self);
	big_float_t result;
	big_float_init(_big_num_ref(result), self->mantissa.size, self->exponent.size);
	big_float_set_one(_big_num_ref(result));
	big_num_carry_t c = 0;

	while( c == 0 ) {
		if( (_pow.table[0] & 1) != 0)
			c += big_float_mul(_big_num_ref(result), start, true);

		big_uint_rcr(_big_num_ref(_pow), 1, 0);

		if( big_uint_is_zero(_pow) )
			break;

		c += big_float_mul(_big_num_ref(start), start, true);
	}

	_big_num_deref(self) = result;

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * p can be negative
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments 0^0 or 0^(-something)
 * @param[in] self 
 * @param[in] _pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_int(_big_num_inout(big_float_t, self), big_int_t _pow)
{
	if( big_float_is_nan(_big_num_deref(self)) )
		return 1;

	if ( !big_int_is_sign(_pow) )
		return big_float_pow_big_uint(self, _pow);
	
	if ( big_float_is_zero(_big_num_deref(self)) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_num_carry_t c = big_int_change_sign(_big_num_ref(_pow));
	big_float_t t = _big_num_deref(self);
	c += big_float_pow_big_uint(_big_num_ref(t), _pow); // here can only be a carry (return:1)

	big_float_set_one(self);
	c += big_float_div(self, t, true);

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * this must be greater than zero (this > 0)
 * pow can be negative and with fraction
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect argument ('this' <= 0)
 * @param[in] self 
 * @param[in] _pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_frac(_big_num_inout(big_float_t, self), big_float_t _pow)
{
	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(_pow) )
		return _big_float_check_carry(self, 1);

	big_float_t temp;
	big_float_init(_big_num_ref(temp), self->mantissa.size, self->exponent.size);
	big_float_set_one(_big_num_ref(temp));
	big_num_carry_t c = big_float_ln(_big_num_ref(temp), _big_num_deref(self));

	if( c != 0 ) { // can be 2 from big_float_ln()
		big_float_set_nan(self);
		return c;
	}

	c += big_float_mul(_big_num_ref(temp), _pow, true);
	c += big_float_exp(self, temp);

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * pow can be negative and with fraction
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect argument ('this' or 'pow')
 * @param[in] self 
 * @param[in] _pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow(_big_num_inout(big_float_t, self), big_float_t _pow)
{
	if (big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(_pow))
		return _big_float_check_carry(self, 1);

	if (big_float_is_zero(_big_num_deref(self))) {
		// 0^pow will be 0 only for pow>0
		if( big_float_is_sign(_pow) || big_float_is_zero(_pow) ) {
			big_float_set_nan(self);
			return 2;
		}

		big_float_set_zero(self);

		return 0;
	}

	big_int_t zero, negative_bits;
	big_int_init(_big_num_ref(zero), _pow.exponent.size);
	big_int_set_zero(_big_num_ref(zero));
	big_int_init_int(_big_num_ref(negative_bits), _pow.exponent.size, -_big_num_sstrg_t(self->mantissa.size*BIG_NUM_BITS_PER_UNIT));

	if ( big_int_cmp_bigger(_pow.exponent, negative_bits) && big_int_cmp_smaller_equal(_pow.exponent, zero)) {
		if ( _big_float_is_integer(_pow) )
			return _big_float_pow_big_float_int(self, _pow);
	}

	return big_float_pow_big_frac(self, _pow);
}

/**
 * this function calculates the square root
 * e.g. let this=9 then this.Sqrt() gives 3
 * 
 * return:
 * -  0 - ok
 * -  1 - carry
 * -  2 - improper argument (this<0 or NaN)
 * @param[in] self 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_sqrt(_big_num_inout(big_float_t, self))
{
	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_sign(_big_num_deref(self))) {
		big_float_set_nan(self);
		return 2;
	}

	if (big_float_is_zero(_big_num_deref(self)))
		return 0;

	big_float_t old = _big_num_deref(self);
	big_float_t ln; // ln get's set in big_float_ln()
	big_float_init(_big_num_ref(ln), self->mantissa.size, self->exponent.size);
	big_num_carry_t c = 0;

	// we're using the formula: sqrt(x) = e ^ (ln(x) / 2)
	c += big_float_ln(_big_num_ref(ln), _big_num_deref(self));
	c += big_int_sub_int(_big_num_ref(ln.exponent), 1, 0); // ln = ln / 2
	c += big_float_exp(self, ln);

	// above formula doesn't give accurate results for some integers
	// e.g. Sqrt(81) would not be 9 but a value very closed to 9
	// we're rounding the result, calculating result*result and comparing
	// with the old value, if they are equal then the result is an integer too


	if ( c == 0 && _big_float_is_integer(old) && !_big_float_is_integer(_big_num_deref(self))) {
		big_float_t temp = _big_num_deref(self);
		c += big_float_round(_big_num_ref(temp));

		big_float_t temp2 = temp;
		c += big_float_mul(_big_num_ref(temp), temp2, true);

		if (big_float_cmp_equal(temp, old))
			_big_num_deref(self) = temp2;
	}

	return _big_float_check_carry(self, c);
}

/**
 * Exponent this = exp(x) = e^x
 * 
 * we're using the fact that our value is stored in form of:
 * 		x = mantissa * 2^exponent
 * then
 * 		e^x = e^(mantissa* 2^exponent) or
 * 		e^x = (e^mantissa)^(2^exponent)
 * 'Exp' returns a carry if we can't count the result ('x' is too big)
 * @param[in] self 
 * @param[in] x 
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_exp(_big_num_inout(big_float_t, self), big_float_t x)
{
	big_num_carry_t c = 0;
	if ( big_float_is_nan(x) )
		return _big_float_check_carry(self, 1);
	
	if ( big_float_is_zero(x) ) {
		big_float_set_one(self);
		return 0;
	}

	// m will be the value of the mantissa in range (-1,1)
	big_float_t m;
	big_float_init_big_float(_big_num_ref(m), self->mantissa.size, self->exponent.size, x);
	big_int_set_int(_big_num_ref(m.exponent), -_big_num_sstrg_t(self->mantissa.size * BIG_NUM_BITS_PER_UNIT));

	// 'e_' will be the value of '2^exponent'
	//   e_.mantissa.table[man-1] = BIG_NUM_HIGHEST_BIT;  and
	//   e_.exponent.Add(1) mean:
	//     e_.mantissa.table[0] = 1;
	//     e_.Standardizing();
	//     e_.exponent.Add(man*BIG_NUM_BITS_PER_UNIT)
	//     (we must add 'man*BIG_NUM_BITS_PER_UNIT' because we've taken it from the mantissa)
	big_float_t e_;
	big_float_init_big_float(_big_num_ref(e_), self->mantissa.size, self->exponent.size, x);
	big_int_set_zero(_big_num_ref(e_.mantissa));
	e_.mantissa.table[self->mantissa.size-1] = BIG_NUM_HIGHEST_BIT;
	c += big_int_add_int(_big_num_ref(e_.exponent), 1, 0);
	big_float_abs(_big_num_ref(e_));

	/*
		now we've got:
		m - the value of the mantissa in range (-1,1)
		e_ - 2^exponent

		e_ can be as:
		...2^-2, 2^-1, 2^0, 2^1, 2^2 ...
		...1/4 , 1/2 , 1  , 2  , 4   ...

		above one e_ is integer

		if e_ is greater than 1 we calculate the exponent as:
			e^(m * e_) = ExpSurrounding0(m) ^ e_
		and if e_ is smaller or equal one we calculate the exponent in this way:
			e^(m * e_) = ExpSurrounding0(m* e_)
		because if e_ is smaller or equal 1 then the product of m*e_ is smaller or equal m
	*/

	big_float_t one;
	big_float_init(_big_num_ref(one), e_.mantissa.size, e_.exponent.size);
	big_float_set_one(_big_num_ref(one));
	size_t steps;
	if (big_float_cmp_smaller_equal(e_, one)) {
		big_float_mul(_big_num_ref(m), e_, true);
		_big_float_exp_surrounding_0(self, m, _big_num_ref(steps));
	} else {
		_big_float_exp_surrounding_0(self, m, _big_num_ref(steps));
		c += _big_float_pow_big_float_uint(self, e_);
	}

	return _big_float_check_carry(self, c);
}

/**
 * Natural logarithm this = ln(x)
 * (a logarithm with the base equal 'e')
 * 
 * we're using the fact that our value is stored in form of:
 * 		x = mantissa * 2^exponent
 * then
 * 		ln(x) = ln (mantissa * 2^exponent) = ln (mantissa) + (exponent * ln (2))
 * 
 * the mantissa we'll show as a value from range <1,2) because the logarithm
 * is decreasing too fast when 'x' is going to 0
 * 
 * return values:
 * -  	0 - ok
 * -  	1 - overflow (carry)
 * -  	2 - incorrect argument (x<=0)
 * @param[in] self 
 * @param[in] x 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_ln(_big_num_inout(big_float_t, self), big_float_t x)
{
	if (big_float_is_nan(x))
		return _big_float_check_carry(self, 1);
	
	if ( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t exponent_temp, mantissa_compensation;
	big_float_init_big_int(_big_num_ref(exponent_temp), self->mantissa.size, self->exponent.size, x.exponent);

	// m will be the value of the mantissa in range <1,2)
	big_float_t m;
	big_float_init_big_float(_big_num_ref(m), self->mantissa.size, self->exponent.size, x);
	big_int_set_int(_big_num_ref(m.exponent), -_big_num_sstrg_t(self->mantissa.size*BIG_NUM_BITS_PER_UNIT - 1));

	// we must add 'self->mantissa.size*BIG_NUM_BITS_PER_UNIT-1' because we've taken it from the mantissa
	big_float_init_uint(_big_num_ref(mantissa_compensation), self->mantissa.size, self->exponent.size, self->mantissa.size*BIG_NUM_BITS_PER_UNIT-1);
	big_num_carry_t c = big_float_add(_big_num_ref(exponent_temp), mantissa_compensation, true);

	size_t steps;
	_big_float_ln_surrounding_1(self, m, _big_num_ref(steps));

	big_float_t ln2;
	big_float_init(_big_num_ref(ln2), self->mantissa.size, self->exponent.size);
	big_float_set_ln2(_big_num_ref(ln2));
	c += big_float_mul(_big_num_ref(exponent_temp), ln2, true);
	c += big_float_add(self, exponent_temp, true);

	return _big_float_check_carry(self, c);
}

/**
 * Logarithm from 'x' with a 'base'
 * we're using the formula:
 * 		Log(x) with 'base' = ln(x) / ln(base)
 * 
 * return values:
 * -  	0 - ok
 * -  	1 - overflow
 * -  	2 - incorrect argument (x<=0)
 * -  	3 - incorrect base (a<=0 or a=1)
 * @param[in] self 
 * @param[in] x 
 * @param[in] base 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_log(_big_num_inout(big_float_t, self), big_float_t x, big_float_t base)
{
	if( big_float_is_nan(x) || big_float_is_nan(base) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t denominator;
	big_float_init(_big_num_ref(denominator), self->mantissa.size, self->exponent.size);
	big_float_set_one(_big_num_ref(denominator));

	if( big_float_is_sign(base) || big_float_is_zero(base) || big_float_cmp_equal(base, denominator) ) {
		big_float_set_nan(self);
		return 3;
	}
	
	if( big_float_cmp_equal(x, denominator) ) { // (this is: if x == 1)
		// log(1) is 0
		big_float_set_zero(self);
		return 0;
	}

	// another error values we've tested at the beginning
	// there can only be a carry
	big_num_carry_t c = big_float_ln(self, x);

	c += big_float_ln(_big_num_ref(denominator), base);
	c += big_float_div(self, denominator, true);

	return _big_float_check_carry(self, c);
}

/**
 * defualt constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 */
void big_float_init(_big_num_inout(big_float_t, self), size_t man, size_t exp)
{
	big_int_init(_big_num_ref(self->exponent), exp);
	big_uint_init(_big_num_ref(self->mantissa), man);
}

/**
 * float constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, float value)
{
	big_float_init_double(self, man, exp, value);
}

/**
 * double constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_double(_big_num_inout(big_float_t, self), size_t man, size_t exp, double value)
{
	big_float_init(self, man, exp);

	#ifndef GL_core_profile
	union 
	{
		double d;
		big_num_strg_t u[2]; // two 32bit words
	} temp;

	temp.d = value;
	
	#else
	struct double_union_u {
		big_num_strg_t u[2]; // two 32bit words
	} temp;

	uvec2 double_vec = unpackDouble2x32(value);
	temp.u[0] = double_vec.x;
	temp.u[1] = double_vec.y;

	#endif


	big_num_sstrg_t e = _big_num_sstrg_t(( temp.u[1] & 0x7FF00000u) >> 20);
	big_num_strg_t m1 = ((temp.u[1] &    0xFFFFFu) << 11) | (temp.u[0] >> 21);
	big_num_strg_t m2 = temp.u[0] << 11;
	
	if( e == 2047 ) {
		// If E=2047 and F is nonzero, then V=NaN ("Not a number")
		// If E=2047 and F is zero and S is 1, then V=-Infinity
		// If E=2047 and F is zero and S is 0, then V=Infinity

		// we do not support -Infinity and +Infinity
		// we assume that there is always NaN 

		big_float_set_nan(self);
	}
	else if( e > 0 ) {
		// If 0<E<2047 then
		// V=(-1)**S * 2 ** (E-1023) * (1.F)
		// where "1.F" is intended to represent the binary number
		// created by prefixing F with an implicit leading 1 and a binary point.
		
		_big_float_init_double_set_exp_and_man(self,
			(temp.u[1] & 0x80000000u) != 0,
			_big_num_sstrg_t(e - 1023 - self->mantissa.size*BIG_NUM_BITS_PER_UNIT + 1), 
			0x80000000u,
			m1, m2);

		// we do not have to call Standardizing() here
		// because the mantissa will have the highest bit set
	} else {
		// e == 0

		if( m1 != 0 || m2 != 0 ) {
			// If E=0 and F is nonzero,
			// then V=(-1)**S * 2 ** (-1022) * (0.F)
			// These are "unnormalized" values.

			big_uint_t m;
			big_uint_init(_big_num_ref(m), 2);
			m.table[1] = m1;
			m.table[0] = m2;
			size_t moved = big_uint_compensation_to_left(_big_num_ref(m));

			_big_float_init_double_set_exp_and_man(self,
				(temp.u[1] & 0x80000000u) != 0,
				_big_num_sstrg_t(e - 1022 - self->mantissa.size*BIG_NUM_BITS_PER_UNIT + 1 - moved), 
				0,
				m.table[1], m.table[0]);
		} else {
			// If E=0 and F is zero and S is 1, then V=-0
			// If E=0 and F is zero and S is 0, then V=0 

			// we do not support -0 or 0, only is one 0
			big_float_set_zero(self);
		}
	}

	// never be a carry
}

/**
 * uint constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_strg_t value)
{
	big_float_init(self, man, exp);

	if (value == 0) {
		big_float_set_zero(self);
		return;
	}

	self->info = 0;

	for (size_t i = 0; i < self->mantissa.size-1; ++i)
		self->mantissa.table[i] = 0;

	self->mantissa.table[self->mantissa.size-1] = value;
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, -_big_num_sstrg_t(self->mantissa.size-1) * _big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT));

	// there shouldn't be a carry because 'value' has the 'uint' type 
	_big_float_standardizing(self);
}

/**
 * int constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_sstrg_t value)
{
	bool is_sign = false;
	if (value < 0) {
		value = -value;
		is_sign = true;
	}

	big_float_init_uint(self, man, exp, _big_num_strg_t(value));

	if (is_sign)
		big_float_set_sign(self);
}

/**
 * big float constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
big_num_carry_t big_float_init_big_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_float_t value)
{
	big_float_init(self, man, exp);
	
	self->info = value.info;
	if (big_float_is_nan(_big_num_deref(self)));
		return 1;
	
	if (0 != big_int_init_big_int(_big_num_ref(self->exponent), self->exponent.size, value.exponent) ) {
		big_float_set_nan(self);
		return 1;
	}

	size_t man_len_min = (self->mantissa.size < value.mantissa.size) ? self->mantissa.size : value.mantissa.size;
	size_t i;
	big_num_carry_t c = 0;

	for ( i = 0; i < man_len_min ; ++i)
		self->mantissa.table[self->mantissa.size-1-i] = value.mantissa.table[value.mantissa.size-1-i];
	for ( ; i < self->mantissa.size ; ++i)
		self->mantissa.table[self->mantissa.size-1-i] = 0;

	if (self->mantissa.size > value.mantissa.size) {
		big_num_strg_t man_diff = (self->mantissa.size - value.mantissa.size) * BIG_NUM_BITS_PER_UNIT;
		c += big_int_sub_int(_big_num_ref(self->exponent), man_diff, 0);
	} else if(self->mantissa.size < value.mantissa.size) {
		big_num_strg_t man_diff =(value.mantissa.size - self->mantissa.size) * BIG_NUM_BITS_PER_UNIT;
		c += big_int_add_int(_big_num_ref(self->exponent), man_diff, 0);
	}

	_big_float_correct_zero(self);

	return _big_float_check_carry(self, c);
}

/**
 * big uint constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_big_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_uint_t value)
{
	big_float_init(self, man, exp);

	self->info = 0;
	big_num_sstrg_t compensation = _big_num_sstrg_t(big_uint_compensation_to_left(_big_num_ref(value)));
	_big_float_init_uint_or_int(self, value, compensation);
}

/**
 * big int constructor
 * @param[in, out] self the big num object
 * @param[in] man the length of the mantissa object
 * @param[in] exp the length of the exponent object
 * @param[in] value the value to set self to
 */
void big_float_init_big_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_int_t value)
{
	big_float_init(self, man, exp);

	self->info = 0;
	bool is_sign = false;
	if (big_int_is_sign(value)) {
		big_int_change_sign(_big_num_ref(value));
		is_sign = true;
	}

	big_num_sstrg_t compensation = _big_num_sstrg_t(big_int_compensation_to_left(_big_num_ref(value)));
	_big_float_init_uint_or_int(self, value, compensation);

	if (is_sign)
		big_float_set_sign(self);
}

/**
 * double converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_double(big_float_t self, _big_num_out(double, result))
{
	if ( big_float_is_zero(self) ) {
		_big_num_deref(result) = 0.0;
		return 0;
	}

	if ( big_float_is_nan(self) ) {
		_big_num_deref(result) = _big_float_to_double_set_double(self, false, 2047, 0, false, true);
		return 0;
	}

	big_num_sstrg_t e_correction = _big_num_sstrg_t((self.mantissa.size*BIG_NUM_BITS_PER_UNIT)) - 1;
	big_int_t _e_correction;
	big_int_init_int(_big_num_ref(_e_correction), self.exponent.size, 1024 - e_correction);
	if (big_int_cmp_bigger_equal(self.exponent, _e_correction) ) {
		// +/- infinity
		_big_num_deref(result) = _big_float_to_double_set_double(self, big_float_is_sign(self), 2047, 0, true, false);
		return 1;
	} else {
		big_int_init_int(_big_num_ref(_e_correction), self.exponent.size, -1023 -52 - e_correction);
		if (big_int_cmp_smaller_equal(self.exponent, _e_correction)) {
			// too small value - we assume that there'll be a zero and return a carry
			_big_num_deref(result) = 0.0;
			return 1;
		}
	}

	big_num_sstrg_t e;
	big_int_to_int(self.exponent, _big_num_ref(e));
	e += e_correction;

	if ( e <= -1023 ) {
		// -1023-52 < e <= -1023  (unnormalized value)
		_big_num_deref(result) = _big_float_to_double_set_double(self, big_float_is_sign(self), 0, -(e + 1023), false, false);
	} else {
		// -1023 < e < 1024
		_big_num_deref(result) = _big_float_to_double_set_double(self, big_float_is_sign(self), (e + 1023), -1, false, false);
	}
}

/**
 * float converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_float(big_float_t self, _big_num_out(float, result))
{
	double result_double;
	big_num_carry_t c = big_float_to_double(self, _big_num_ref(result_double));
	_big_num_deref(result) = _big_num_float(result_double);

	if (_big_num_deref(result) == -0.0f)
		_big_num_deref(result) = 0.0f;
	
	if (c != 0)
		return 1;

	// although the result_double can have a correct value
	// but after converting to float there can be infinity

#ifndef GL_core_profile 
	// check for positive infinity
	if ( _big_num_deref(result) == (float)(1.0 / 0.0) )
		return 1;

	// check for negative infinity
	if ( _big_num_deref(result) == (float)(-1.0 / 0.0) )
		return 1;
#else
	// check for infinity
	if ( isinf(_big_num_deref(result)) )
		return 1;
#endif
	
	if (_big_num_deref(result) == 0.0f && result_double != 0.0)
		return 1;
	
	return 0;
}

/**
 * uint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_uint(big_float_t self, _big_num_out(big_num_strg_t, result))
{
	if ( 0 != _big_float_to_uint_or_int(self, result) )
		return 1;
	if ( big_float_is_sign(self) )
		return 1;
	return 0;
}

/**
 * int converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_int(big_float_t self, _big_num_out(big_num_sstrg_t, result))
{
	big_num_strg_t result_uint;
	big_num_carry_t c = _big_float_to_uint_or_int(self, _big_num_ref(result_uint));
	_big_num_deref(result) = _big_num_sstrg_t(result_uint);

	if ( c != 0 )
		return 1;
	
	big_num_strg_t mask = 0;
	if (big_float_is_sign(self)) {
		mask = BIG_NUM_MAX_VALUE;
		_big_num_deref(result) = -1 * _big_num_deref(result);
	}

	return ((_big_num_deref(result) & BIG_NUM_HIGHEST_BIT) == (mask & BIG_NUM_HIGHEST_BIT)) ? 0 : 1;
}

/**
 * this method returns true if 'self' is smaller than 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_smaller(big_float_t self, big_float_t l)
{
	if (big_float_is_sign(self) && !big_float_is_sign(l)) {
		// self < 0 and l >= 0
		return true;
	}

	if (!big_float_is_sign(self) && big_float_is_sign(l)) {
		// self >= 0 and l < 0
		return false;
	}

	// both signs are the same
	if ( big_float_is_sign(self) )
		return _big_float_smaller_without_sign_than(l, self);
	
	return _big_float_smaller_without_sign_than(self, l);
}

/**
 * this method returns true if 'self' is bigger than 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_bigger(big_float_t self, big_float_t l)
{
	if (big_float_is_sign(self) && !big_float_is_sign(l)) {
		// self < 0 and l >= 0
		return false;
	}

	if (!big_float_is_sign(self) && big_float_is_sign(l)) {
		// self >= 0 and l < 0
		return true;
	}

	// both signs are the same
	if ( big_float_is_sign(self) )
		return _big_float_greater_without_sign_than(l, self);
	
	return _big_float_greater_without_sign_than(self, l);
}

/**
 * this method returns true if 'self' is equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_equal(big_float_t self, big_float_t l)
{
	if ( big_float_is_sign(self) != big_float_is_sign(l) )
		return false;
	
	return _big_float_equal_without_sign(self, l);
}

/**
 * this method returns true if 'self' is smaller or equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_smaller_equal(big_float_t self, big_float_t l)
{
	return !big_float_cmp_bigger(self, l);
}

/**
 * this method returns true if 'self' is bigger or equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_bigger_equal(big_float_t self, big_float_t l)
{
	return !big_float_cmp_smaller(self, l);
}



/**
 * this method sets NaN if there was a carry (and returns 1 in such a case)
 * @param[in, out] self the big num object
 * @param[in] c can be 0, 1 or other value different from zero
 * @return big_num_carry_t 
 */
_big_num_static big_num_carry_t _big_float_check_carry(_big_num_inout(big_float_t, self), big_num_carry_t c)
{
	if ( c != 0) {
		big_float_set_nan(self);
		return 1;
	}
	return 0;
}

/**
 * this method moves all bits from mantissa into its left side
 * (suitably changes the exponent) or if the mantissa is zero
 * it sets the exponent to zero as well
 * (and clears the sign bit and sets the zero bit)
 * 
 * it can return a carry
 * the carry will be when we don't have enough space in the exponent
 * 
 * you don't have to use this method if you don't change the mantissa
 * and exponent directly
 * @param[in, out] self the big num object
 * @return big_num_carry_t 
 */
_big_num_static big_num_carry_t _big_float_standardizing(_big_num_inout(big_float_t, self))
{
	if ( big_uint_is_the_highest_bit_set(self->mantissa) ) {
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_ZERO);
		return 0;
	}

	if ( _big_float_correct_zero(self) )
		return 0;
	
	big_int_t comp;
	big_int_init_int(_big_num_ref(comp), self->exponent.size, _big_num_sstrg_t(big_uint_compensation_to_left(_big_num_ref(self->mantissa))));
	return big_int_sub(_big_num_ref(self->exponent), comp);
}

/**
 * if the mantissa is equal zero this method sets exponent to zero and
 * info without the sign
 * @param[in, out] self the big num object
 * @return true there was a correction
 * @return false there was no correction
 */
_big_num_static bool	_big_float_correct_zero(_big_num_inout(big_float_t, self))
{
	if (big_uint_is_zero(self->mantissa)) {
		_big_float_set_info_bit(self, BIG_FLOAT_INFO_ZERO);
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);
		big_int_set_zero(_big_num_ref(self->exponent));

		return true;
	} else {
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_ZERO);
	}
}

/**
 * this method clears a specific bit in the 'info' variable
 * @param[in, out] self the big num object
 * @param[in] bit the bit to clear
 */
_big_num_static void	_big_float_clear_info_bit(_big_num_inout(big_float_t, self), big_float_info_t bit)
{
	self->info = self->info & (~bit);
}

/**
 * this method sets a specific bit in the 'info' variable
 * @param[in, out] self the big num object
 * @param[in] bit the bit to set
 */
_big_num_static void	_big_float_set_info_bit(_big_num_inout(big_float_t, self), big_float_info_t bit)
{
	self->info = self->info | (bit);
}

/**
 * this method returns true if a specific bit in the 'info' variable is set
 * @param[in, out] self the big num object
 * @param[in] bit the bit to check
 * @return true bit was set
 * @return false bit was not set
 */
_big_num_static bool	_big_float_is_info_bit(big_float_t self, big_float_info_t bit)
{
	return (self.info & bit) != 0;
}

/**
 * this method does the half-to-even rounding (banker's rounding)
 * 
 * if is_half is:
 * -  true  - that means the rest was equal the half (0.5 decimal)
 * -  false - that means the rest was greater than a half (greater than 0.5 decimal)
 * 
 * if the rest was less than a half then don't call this method
 * (the rounding should does nothing then)
 * @param[in, out] self the big num object
 * @param[in] is_half 
 * @param[in] rounding_up 
 * @return big_num_carry_t 
 */
big_num_carry_t _big_float_round_half_to_even(_big_num_inout(big_float_t, self), bool is_half, bool rounding_up)
{
	big_num_carry_t c = 0;

	if( !is_half || big_uint_is_the_lowest_bit_set(self->mantissa) )
	{
		if( rounding_up )
		{
			if( 0 != big_uint_add_uint(_big_num_ref(self->mantissa), 1) )
			{
				big_uint_rcr(_big_num_ref(self->mantissa), 1, 1);
				c = big_int_add_int(_big_num_ref(self->exponent), 1, 0);
			}
		} else {
			big_uint_sub_uint(_big_num_ref(self->mantissa), 1);

			// we're using rounding_up=false in Add() when the mantissas have different signs
			// mantissa can be zero only when previous mantissa was equal to ss2.mantissa
			// but in such a case 'last_bit_set' will not be set and consequently 'do_rounding' will be false
		}
	}

	return c;
}


/**
 * Auxiliary method for adding
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the other big num object
 * @param[in] exp_offset 
 * @param[out] last_bit_set 
 * @param[out] rest_zero 
 * @param[out] do_adding 
 * @param[out] do_rounding 
 */
_big_num_static void _big_float_add_check_exponents(_big_num_inout(big_float_t, self), _big_num_inout(big_float_t, ss2), big_int_t exp_offset, _big_num_out(bool, last_bit_set), _big_num_out(bool, rest_zero), _big_num_out(bool, do_adding), _big_num_out(bool, do_rounding))
{
	big_int_t mantissa_size_in_bits;
	big_int_init_uint(_big_num_ref(mantissa_size_in_bits), self->exponent.size, self->mantissa.size * BIG_NUM_BITS_PER_UNIT);

	if (big_int_cmp_equal(exp_offset, mantissa_size_in_bits)) {
		_big_num_deref(last_bit_set) = big_uint_is_the_highest_bit_set(ss2->mantissa);
		_big_num_deref(rest_zero) = big_uint_are_first_bits_zero(ss2->mantissa, self->mantissa.size * BIG_NUM_BITS_PER_UNIT - 1);
		_big_num_deref(do_rounding) = true;
	} else if (big_int_cmp_smaller(exp_offset, mantissa_size_in_bits)) {
		big_num_strg_t moved;
		big_int_to_int(exp_offset, _big_num_ref(moved)); // how many times we must move ss2.mantissa
		_big_num_deref(rest_zero) = true;

		if ( moved > 0 ) {
			_big_num_deref(last_bit_set) = big_uint_get_bit(ss2->mantissa, moved-1);

			if (moved > 1)
				_big_num_deref(rest_zero) = big_uint_are_first_bits_zero(ss2->mantissa, moved-1);
			
			// (2) moving 'exp_offset' times
			big_uint_rcr(_big_num_ref(ss2->mantissa), moved, 0);
		}

		_big_num_deref(do_adding) = true;
		_big_num_deref(do_rounding) = true;

		// if exp_offset is greater than mantissa_size_in_bits then we do nothing
		// ss2 is too small for taking into consideration in the sum
	}
}

/**
 * an auxiliary method for adding
 * @param[in] self the big num object
 * @param[in] ss2 the other big num object
 * @param[in] last_bit_set 
 * @param[in] rest_zero 
 * @return big_num_carry_t 
 */
_big_num_static big_num_carry_t _big_float_add_mantissas(_big_num_inout(big_float_t, self), big_float_t ss2, _big_num_out(bool, last_bit_set), _big_num_out(bool, rest_zero))
{
	big_num_carry_t c = 0;
	if (big_float_is_sign(_big_num_deref(self)) == big_float_is_sign(ss2)) {
		// values have the same signs
		if (0 != big_uint_add(_big_num_ref(self->mantissa), ss2.mantissa, 0)) {
			// we have one bit more from addition (carry)
			// now rest_zero means the old rest_zero with the old last_bit_set
			_big_num_deref(rest_zero)    = (!_big_num_deref(last_bit_set) && _big_num_deref(rest_zero));
			_big_num_deref(last_bit_set) = 0 != big_uint_rcr(_big_num_ref(self->mantissa), 1,1);
			c += big_int_add_int(_big_num_ref(self->exponent), 1, 0);
		}
	} else {
		// values have different signs
		// there shouldn't be a carry here because
		// (1) (2) guarantee that the mantissa of this
		// is greater than or equal to the mantissa of the ss2

		big_uint_sub(_big_num_ref(self->mantissa), ss2.mantissa, 0);
	}

	return c;
}

/**
 * Addition this = this + ss2
 * @param[in] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round set to true to round
 * @param[in] adding set to true if adding, false if subtracting
 * @return big_num_carry_t carry if the result is too big
 */
_big_num_static big_num_carry_t _big_float_add(_big_num_inout(big_float_t, self), big_float_t ss2, bool round, bool adding)
{
	bool last_bit_set, rest_zero, do_adding, do_rounding, rounding_up;
	big_num_carry_t c = 0;

	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( !adding )
		big_float_change_sign(_big_num_ref(ss2)); // subtracting

	// (1) abs(this) will be >= abs(ss2)
	if( _big_float_smaller_without_sign_than(_big_num_deref(self), ss2) )
		big_float_swap(self, _big_num_ref(ss2));

	if( big_float_is_zero(ss2) )
		return 0;

	big_int_t exp_offset = self->exponent;
	big_int_sub(_big_num_ref(exp_offset), ss2.exponent);

	if( 0 == big_int_abs(_big_num_ref(exp_offset)) ) {
		// if there is a carry in Abs it means the value in exp_offset has only the lowest bit set
		// so the value is the smallest possible integer
		// and its Abs would be greater than mantissa size in bits
		// so the method AddCheckExponents would do nothing


		last_bit_set = rest_zero = do_adding = do_rounding = false;
		rounding_up = (big_float_is_sign(_big_num_deref(self)) == big_float_is_sign(ss2));

		_big_float_add_check_exponents(self, _big_num_ref(ss2), exp_offset, _big_num_ref(last_bit_set), _big_num_ref(rest_zero), _big_num_ref(do_adding), _big_num_ref(do_rounding));
		
		if( do_adding )
			c += _big_float_add_mantissas(self, ss2, _big_num_ref(last_bit_set), _big_num_ref(rest_zero));

		if( !round || !last_bit_set )
			do_rounding = false;

		if( do_rounding )
			c += _big_float_round_half_to_even(self, rest_zero, rounding_up);

		if( do_adding || do_rounding )
			c += _big_float_standardizing(self);
	}

	return _big_float_check_carry(self, c);
}

/**
 * this method checks whether a table pointed by 'tab' and 'len'
 * has the value 0.5 decimal
 * (it is treated as the comma operator would be before the highest bit)
 * call this method only if the highest bit is set - you have to test it beforehand
 * @param[in] self 
 * @param[in] tab 
 * @param[in] len 
 * @return true tab was equal the half (0.5 decimal)
 * @return false tab was greater than a half (greater than 0.5 decimal)
 */
_big_num_static bool _big_float_check_greater_or_equal_half(_big_num_inout(big_float_t, self), big_num_strg_t tab[2*BIG_MAN_PREC], big_num_strg_t len)
{
	size_t i;

	for(i=0 ; i<len-1 ; ++i)
		if( tab[i] != 0 )
			return false;

	if( tab[i] != BIG_NUM_HIGHEST_BIT )
		return false;

	return true;
}

/**
 * power this = this ^ pow
 * (pow without a sign)
 * 
 * binary algorithm (r-to-l)
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments (0^0)
 * @param[in] self the big num object
 * @param[in] _pow the value to raise the big num object by
 * @return big_num_ret_t 
 */
_big_num_static big_num_ret_t _big_float_pow_big_float_uint(_big_num_inout(big_float_t, self), big_float_t _pow)
{
	if( big_float_is_nan(_big_num_deref(self)) || big_float_is_nan(_pow) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(_big_num_deref(self)) ) {
		if( big_float_is_zero(_pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	if (big_float_is_sign(_pow))
		big_float_abs(_big_num_ref(_pow));

	big_float_t start = _big_num_deref(self);
	big_float_t result;
	big_float_t one;
	big_num_carry_t c = 0;
	big_float_init(_big_num_ref(one), self->mantissa.size, self->exponent.size);
	big_float_set_one(_big_num_ref(one));
	result = one;

	while( c == 0 ) {
		if( 0 != big_float_mod2(_pow) )
			c += big_float_mul(_big_num_ref(result), start, true);

		c += big_int_sub_int(_big_num_ref(_pow.exponent), 1, 0);

		if( big_float_cmp_smaller(_pow, one) )
			break;

		c += big_float_mul(_big_num_ref(start), start, true);
	}

	_big_num_deref(self) = result;

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ [pow]
 * pow is treated as a value without a fraction
 * pow can be negative
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments 0^0 or 0^(-something)
 * @param[in] self 
 * @param[in] pow 
 * @return big_num_ret_t 
 */
_big_num_static big_num_ret_t _big_float_pow_big_float_int(_big_num_inout(big_float_t, self), big_float_t pow)
{
	if( big_float_is_nan(_big_num_deref(self)) )
		return 1;

	if ( !big_float_is_sign(pow) )
		return _big_float_pow_big_float_uint(self, pow);
	
	if ( big_float_is_zero(_big_num_deref(self)) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_float_t temp = _big_num_deref(self);
	big_num_carry_t c = _big_float_pow_big_float_uint(_big_num_ref(temp), pow); // here can only be a carry (return:1)

	big_float_set_one(self);
	c += big_float_div(self, temp, true);

	return _big_float_check_carry(self, c);
}

/**
 * Exponent this = exp(x) = e^x where x is in (-1,1)
 * we're using the formula:
 * exp(x) = 1 + (x)/(1!) + (x^2)/(2!) + (x^3)/(3!) + ...
 * @param[in, out] self the big num object
 * @param[in] x 
 * @param[out] steps 
 */
_big_num_static void _big_float_exp_surrounding_0(_big_num_inout(big_float_t, self), big_float_t x, _big_num_out(size_t, steps))
{
	big_float_t denominator, denominator_i;
	big_float_t one, old_value, next_part;
	big_float_t numerator;

	big_float_init(_big_num_ref(one), self->mantissa.size, self->exponent.size);
	big_float_init_big_float(_big_num_ref(numerator), self->mantissa.size, self->exponent.size, x);
	big_float_set_one(_big_num_ref(one));
	_big_num_deref(self) = one;
	denominator = one;
	denominator_i = one;

	size_t i;
	old_value = _big_num_deref(self);
	for (i = 1 ; i <= BIG_NUM_ARITHMETIC_MAX_LOOP ; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'
		next_part = numerator;
		if (0 != big_float_div(_big_num_ref(next_part), denominator, true))
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if ( big_float_cmp_equal(old_value, _big_num_deref(self)) )
				// we've added next few parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = _big_num_deref(self);
		}

		// we set the denominator and the numerator for a next part of the formula
		if (0 != big_float_add(_big_num_ref(denominator_i), one, true))
			// if there is a carry here the result we return as good
			break;

		if (0 != big_float_mul(_big_num_ref(denominator), denominator_i, true))
			break;
		
		if (0 != big_float_mul(_big_num_ref(numerator), x, true))
			break;
	}

	#ifndef GL_core_profile
	if (steps)
	#endif
	{
		_big_num_deref(steps) = i;
	}
}

/**
 * Natural logarithm this = ln(x) where x in range <1,2)
 * we're using the formula:
 * ln(x) = 2 * [ (x-1)/(x+1) + (1/3)((x-1)/(x+1))^3 + (1/5)((x-1)/(x+1))^5 + ... ]
 * @param[in, out] self the big num object
 * @param[in] x 
 * @param[out] steps 
 */
_big_num_static void _big_float_ln_surrounding_1(_big_num_inout(big_float_t, self), big_float_t x, _big_num_out(size_t, steps))
{
	big_float_t old_value, next_part, denominator, one, two, x1, x2;
	big_float_init_big_float(_big_num_ref(x1), self->mantissa.size, self->exponent.size, x);
	big_float_init_big_float(_big_num_ref(x2), self->mantissa.size, self->exponent.size, x);
	big_float_init(_big_num_ref(one), self->mantissa.size, self->exponent.size);
	big_float_set_one(_big_num_ref(one));

	if ( big_float_cmp_equal(x, one) ) {
		// LnSurrounding1(1) is 0
		big_float_set_zero(self);
		return;
	}

	big_float_init_uint(_big_num_ref(two), self->mantissa.size, self->exponent.size, 2);

	big_float_sub(_big_num_ref(x1), one, true);
	big_float_add(_big_num_ref(x2), one, true);

	big_float_div(_big_num_ref(x1), x2, true);
	x2 = x1;
	big_float_mul(_big_num_ref(x2), x1, true);

	denominator = one;
	big_float_set_zero(self);

	old_value = _big_num_deref(self);
	size_t i;

	for (i = 1; i <= BIG_NUM_ARITHMETIC_MAX_LOOP; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'

		next_part = x1;

		if (0 != big_float_div(_big_num_ref(next_part), denominator, true))
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if (big_float_cmp_equal(old_value, _big_num_deref(self)))
				// we've added next (step_test) parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = _big_num_deref(self);
		}

		if (0 != big_float_mul(_big_num_ref(x1), x2, true) )
			// if there is a carry here the result we return as good
			break;
		
		if (0 != big_float_add(_big_num_ref(denominator), two, true))
			break;
	}

	// this = this * 2
	// ( there can't be a carry here because we calculate the logarithm between <1,2) )
	big_int_add_int(_big_num_ref(self->exponent), 1, 0);

	#ifndef GL_core_profile
	if (steps)
	#endif
	{
		_big_num_deref(steps) = i;
	}
}

/**
 * an auxiliary method for converting from UInt and Int
 * we assume that there'll never be a carry here
 * (we have an exponent and the value in Big can be bigger than
 * that one from the UInt)
 * @param[in] self the big num object
 * @param[in] value the big num object to set self to
 * @param[in] compensation 
 */
_big_num_static void _big_float_init_uint_or_int(_big_num_inout(big_float_t, self), big_uint_t value, big_num_sstrg_t compensation)
{
	size_t minimum_size = (value.size < self->mantissa.size) ? value.size : self->mantissa.size;
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, (_big_num_sstrg_t(value.size) - _big_num_sstrg_t(self->mantissa.size)) * _big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT) - compensation);

	// copying the highest words
	size_t i;
	for(i=1 ; i<=minimum_size ; ++i)
		self->mantissa.table[self->mantissa.size-i] = value.table[value.size-i];

	// setting the rest of mantissa.table into zero (if some has left)
	for( ; i<=self->mantissa.size ; ++i)
		self->mantissa.table[self->mantissa.size-i] = 0;

	// the highest bit is either one or zero (when the whole mantissa is zero)
	// we can only call CorrectZero()
	_big_float_correct_zero(self);
}

/**
 * an auxiliary method for converting 'this' into 'result'
 * if the value is too big this method returns a carry (1)
 * @param[in] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
_big_num_static big_num_carry_t _big_float_to_uint_or_int(big_float_t self, _big_num_out(big_num_strg_t, result))
{
	_big_num_deref(result) = 0;

	if ( big_float_is_zero(self) )
		return 0;

	big_num_sstrg_t max_bit = -_big_num_sstrg_t(self.mantissa.size * BIG_NUM_BITS_PER_UNIT);
	big_int_t _max_bit;
	big_int_init_int(_big_num_ref(_max_bit), self.exponent.size, max_bit + _big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT));

	if ( big_int_cmp_bigger(self.exponent, _max_bit) )
		// if exponent > (maxbit + big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT)) the value can't be passed
		// into the 'big_num_sstrg_t' type (it's too big)
		return 1;
	
	big_int_init_int(_big_num_ref(_max_bit), self.exponent.size, max_bit);

	if ( big_int_cmp_smaller_equal(self.exponent, _max_bit) )
		// our value is from the range of (-1,1) and we return zero
		return 0;

	// exponent is from a range of (maxbit, maxbit + big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT) >
	// and [maxbit + big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT] <= 0
	big_num_sstrg_t how_many_bits;
	big_int_to_int(self.exponent, _big_num_ref(how_many_bits));

	// how_many_bits is negative, we'll make it positive
	how_many_bits = -how_many_bits;

	_big_num_deref(result) = (self.mantissa.table[self.mantissa.size-1] >> (how_many_bits % BIG_NUM_BITS_PER_UNIT));

	return 0;
}

/**
 * 
 * @param[in] self the big num object
 * @param[in] is_sign 
 * @param[in] e 
 * @param[in] mhighest 
 * @param[in] m1 
 * @param[in] m2 
 */
_big_num_static void _big_float_init_double_set_exp_and_man(_big_num_inout(big_float_t, self), bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2)
{
	big_int_init_int(_big_num_ref(self->exponent), self->exponent.size, e);

	if( self->mantissa.size > 1 ) {
		self->mantissa.table[self->mantissa.size-1] = m1 | mhighest;
		self->mantissa.table[_big_num_sstrg_t(self->mantissa.size-2)] = m2;
		// although man>1 we're using casting into sint
		// to get rid from a warning which generates Microsoft Visual:
		// warning C4307: '*' : integral constant overflow

		for(size_t i=0 ; i<self->mantissa.size-2 ; ++i)
			self->mantissa.table[i] = 0;
	} else {
		self->mantissa.table[0] = m1 | mhighest;
	}

	self->info = 0;

	// the value should be different from zero

	if( is_sign )
		big_float_set_sign(self);
}

/**
 * 
 * @param[in] self the big num object
 * @param[in] is_sign 
 * @param[in] e 
 * @param[in] move 
 * @param[in] infinity 
 * @param[in] nan 
 * @return double 
 */
_big_num_static double _big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan)
{
	#ifndef GL_core_profile
	union {
		double d;
		big_num_strg_t u[2]; // two 32bit words
	} temp;
	#else
	struct {
		uvec2 u; // two 32bit words
	} temp;
	#endif
	
	temp.u[0] = temp.u[1] = 0;

	if (is_sign)
		temp.u[1] |= 0x80000000u;

	temp.u[1] |= (e << 20) & 0x7FF00000u;

	if ( nan ) {
		temp.u[0] |= 1;
		#ifndef GL_core_profile
		return temp.d;
		#else
		return packDouble2x32(temp.u);
		#endif
	}

	if ( infinity ) {
		#ifndef GL_core_profile
		return temp.d;
		#else
		return packDouble2x32(temp.u);
		#endif
	}
	
	big_uint_t m;
	big_uint_init(_big_num_ref(m), 2);
	m.table[1] = self.mantissa.table[self.mantissa.size-1];
	m.table[0] = (self.mantissa.size > 1) ? self.mantissa.table[self.mantissa.size-2] : 0;
	
	big_uint_rcr(_big_num_ref(m), 12 + move, 0);
	
	m.table[1] &= 0xFFFFFu; // cutting the 20 bit (when 'move' was -1)
	temp.u[1] |= m.table[1];
	temp.u[0] |= m.table[0];

	#ifndef GL_core_profile
	return temp.d;
	#else
	return packDouble2x32(temp.u);
	#endif
}

/**
 * this method performs the formula 'abs(this) < abs(ss2)'
 * and returns the result
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
_big_num_static bool _big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return false; // got 2 zeroes
		else
			return true; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return false; // self != 0 & ss2 == 0
	}

	// we're using the fact that all bits in mantissa are pushed
	// into the left side -- Standardizing()
	if( big_int_cmp_equal(self.exponent, ss2.exponent) )
		return big_uint_cmp_smaller(self.mantissa, ss2.mantissa, -1);

	return big_int_cmp_smaller(self.exponent, ss2.exponent);
}

/**
 * this method performs the formula 'abs(this) > abs(ss2)'
 * and returns the result
 * 
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
_big_num_static bool	_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return false; // got 2 zeroes
		else
			return false; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return true; // self != 0 & ss2 == 0
	}

	// we're using the fact that all bits in mantissa are pushed
	// into the left side -- Standardizing()
	if( big_int_cmp_equal(self.exponent, ss2.exponent) )
		return big_uint_cmp_bigger(self.mantissa, ss2.mantissa, -1);

	return big_int_cmp_bigger(self.exponent, ss2.exponent);
}

/**
 * this method performs the formula 'abs(this) == abs(ss2)'
 * and returns the result
 * 
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
_big_num_static bool	_big_float_equal_without_sign(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return true; // got 2 zeroes
		else
			return false; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return false; // self != 0 & ss2 == 0
	}

	if( big_int_cmp_equal(self.exponent, ss2.exponent) && big_uint_cmp_equal(self.mantissa, ss2.mantissa, -1) )
		return true;
	
	return false;
}

/**
 * this method makes an integer value by skipping any fractions
 * 
 * samples:
 * -	10.7 will be 10
 * -	12.1  -- 12
 * -	-20.2 -- 20
 * -	-20.9 -- 20
 * -	-0.7  -- 0
 * -	0.8   -- 0
 * @param[in] self the big num object
 */
_big_num_static void _big_float_skip_fraction(_big_num_inout(big_float_t, self)) {
	if (big_float_is_nan(_big_num_deref(self)) || big_float_is_zero(_big_num_deref(self)) )
		return;
	
	if ( !big_int_is_sign(self->exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return;
	
	big_int_t negative_bits;
	big_int_init_int(_big_num_ref(negative_bits), self->exponent.size, -_big_num_sstrg_t(self->mantissa.size * BIG_NUM_BITS_PER_UNIT));
	if (big_int_cmp_smaller_equal(self->exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		big_float_set_zero(self);
		return;
	}

	// exponent is in range (-self->mantissa.size * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self->exponent, _big_num_ref(e));
	big_uint_clear_first_bits(_big_num_ref(self->mantissa), -e);

	// we don't have to standardize 'Standardizing()' the value because
	// there's at least one bit in the mantissa
	// (the highest bit which we didn't touch)
}

/**
 * this method returns true if the value is integer
 * (there is no a fraction) ( we don't check NaN)
 * @param[in] self 
 * @return true 
 * @return false 
 */
_big_num_static bool _big_float_is_integer(big_float_t self)
{
	if (big_float_is_zero(self))
		return true;
	
	if (!big_int_is_sign(self.exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return true;

	big_int_t negative_bits;
	big_int_init_int(_big_num_ref(negative_bits), self.exponent.size, -_big_num_sstrg_t(self.mantissa.size * BIG_NUM_BITS_PER_UNIT));

	if (big_int_cmp_smaller_equal(self.exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		return false;
	}

	// exponent is in range (-self->mantissa.size * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self.exponent, _big_num_ref(e));
	e = -e; // e means how many bits we must check

	big_num_strg_t len  = e / BIG_NUM_BITS_PER_UNIT;
	big_num_strg_t rest = e % BIG_NUM_BITS_PER_UNIT;
	size_t i = 0;

	for ( ; i < len; ++i)
		if (self.mantissa.table[i] != 0)
			return false;
	
	if (rest > 0) {
		big_num_strg_t rest_mask = BIG_NUM_MAX_VALUE >> (BIG_NUM_BITS_PER_UNIT - rest);
		if( (self.mantissa.table[i] & rest_mask) != 0)
			return false;
	}

	return true;
}

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif