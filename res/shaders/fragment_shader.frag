#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

/**
 * @file BigNum.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#ifndef _BIG_NUM_H_
#define _BIG_NUM_H_

#ifndef GL_core_profile
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
extern "C" {
#endif

//===============
// Config
//===============

/**
 * during some kind of calculating when we're making any long formulas
 * (for example Taylor series)
 * 
 * it's used in ExpSurrounding0(...), LnSurrounding1(...), Sin0pi05(...), etc.
 * 
 * note! there'll not be so many iterations, iterations are stopped when
 * there is no sense to continue calculating (for example when the result
 * still remains unchanged after adding next series and we know that the next
 * series are smaller than previous ones)
 */
#define BIG_NUM_ARITHMETIC_MAX_LOOP	1000

#define BIG_NUM_PREC	 	2 // amount of word's to allocate for uint and int, float using 2x this amount. In reality uses twice as much to allow for non-carry operations

#define BIG_NUM_MUL_DEF		BIG_NUM_MUL1
#define BIG_NUM_DIV_DEF		BIG_NUM_DIV1

//===============
// Don't touch
//===============

#ifndef GL_core_profile
#define BIG_NUM_MAX_VALUE 			(big_num_strg_t  )((1ULL<<( CHAR_BIT*sizeof(big_num_strg_t  )      ))-1ULL)
#define BIG_NUM_HIGHEST_BIT			(big_num_strg_t  )((1ULL<<( CHAR_BIT*sizeof(big_num_strg_t  ) -1ULL)))
#define BIG_NUM_BITS_PER_UNIT		(CHAR_BIT*sizeof(big_num_strg_t))
#else
#define BIG_NUM_MAX_VALUE 			(uint((1UL<<( 32UL       ))-1UL))
#define BIG_NUM_HIGHEST_BIT			(uint((1UL<<( 32UL - 1UL ))    ))
#define BIG_NUM_BITS_PER_UNIT		(32)
#endif

// macro's for GLSL compatibility
#ifndef GL_core_profile
#define _big_num_inout(type, x)		 	type * x
#define _big_num_out(type, x)		 	type * x

#define _big_num_strg_t(x) 				((big_num_strg_t)(x))
#define _big_num_sstrg_t(x) 			((big_num_sstrg_t)(x))
#define _big_num_lstrg_t(x)				((big_num_lstrg_t)(x))
#define _big_num_lsstrg_t(x)			((big_num_lsstrg_t)(x))
#define _big_num_ssize_t(x)				((ssize_t)(x))
#define _big_num_size_t(x)				(size_t(x))
#define _big_num_float(x)				((float)(x))
#define _big_num_ref(x)					(&x)
#define _big_num_deref(x)				(*x)

#define _big_num_static					static
#define _big_num_const_param			const
#else
#define _big_num_static
#define _big_num_const_param

// macro's for GLSL compatibility
#define _big_num_inout(type, x)		 	inout type x
#define _big_num_out(type, x)		 	out type x

#define _big_num_strg_t(x) 				(big_num_strg_t(x))
#define _big_num_sstrg_t(x) 			(big_num_sstrg_t(x))
#define _big_num_lstrg_t(x)				(big_num_lstrg_t(x))
#define _big_num_lsstrg_t(x)			(big_num_lsstrg_t(x))
#define _big_num_ssize_t(x)				(ssize_t(x))
#define _big_num_size_t(x)				(size_t(x))
#define _big_num_float(x)				(float(x))
#define _big_num_ref(x)					(x)
#define _big_num_deref(x)				(x)

#endif

/**
 * Multiplication algorithm selector and
 * Division algorithm selector
 */
#ifndef GL_core_profile
typedef enum {
	/**
	 * Continues addition
	 */
	BIG_NUM_MUL1 = 0,
	/**
	 * No carry algorithm
	 */
	BIG_NUM_MUL2 = 1,
	/**
	 * 
	 */
	BIG_NUM_DIV1 = 0,
	/**
	 * 
	 */
	BIG_NUM_DIV2 = 1,
} big_num_algo_t;
#else
#define BIG_NUM_MUL1 				0
#define BIG_NUM_MUL2 				1
#define BIG_NUM_DIV1				0
#define BIG_NUM_DIV2				1
#define big_num_algo_t				uint
#endif

/**
 * return results for big_uint_div
 */
#ifndef GL_core_profile
typedef enum {
	/**
	 * ok
	 */
	BIG_NUM_DIV_OK = 0,
	/**
	 * division by zero
	 */
	BIG_NUM_DIV_ZERO = 1,
	/**
	 * division calculating ( used internally )
	 */
	BIG_NUM_DIV_BUSY = 2,
} big_num_div_ret_t;
#else
#define BIG_NUM_DIV_OK				0
#define BIG_NUM_DIV_ZERO			1
#define BIG_NUM_DIV_BUSY			2
#define big_num_div_ret_t			uint
#endif

/**
 * return results for big_*type*_pow
 */
#ifndef GL_core_profile
typedef enum {
	/**
	 * ok
	 */
	BIG_NUM_OK = 0,
	/**
	 * carry
	 */
	BIG_NUM_OVERFLOW = 1,
	/**
	 * incorrect argument
	 * (a^b) 	: a = 0 and b = 0
	 * log(b) 	: b <= 0
	 * a / b 	: b = 0
	 * a % b	: b = 0
	 */
	BIG_NUM_INVALID_ARG = 2,
	/**
	 * incorrect base
	 * log(x, base): (base <= 0 or base = 1)
	 */
	BIG_NUM_LOG_INVALID_BASE = 3,
} big_num_ret_t;
#else
#define BIG_NUM_OK					0
#define BIG_NUM_OVERFLOW			1
#define BIG_NUM_INVALID_ARG			2
#define BIG_NUM_LOG_INVALID_BASE	3
#define big_num_ret_t				uint
#endif

/**
 * The type used to store the big num values in
 */
#ifndef GL_core_profile
typedef uint32_t big_num_strg_t;
#else
#define big_num_strg_t				uint
#endif

/**
 * Signed version of the big_num_strg_t
 */
#ifndef GL_core_profile
typedef int32_t  big_num_sstrg_t;
#else
#define big_num_sstrg_t				int
#endif

/**
 * Signed version of the big_num_strg_t
 */
#ifndef GL_core_profile
typedef uint64_t  big_num_lstrg_t;
#else
#define big_num_lstrg_t				uint64_t
#endif

/**
 * Signed version of the big_num_strg_t
 */
#ifndef GL_core_profile
typedef int64_t  big_num_lsstrg_t;
#else
#define big_num_lsstrg_t			int64_t
#endif

/**
 * Carry type, either 1 or 0
 */
#ifndef GL_core_profile
typedef big_num_strg_t  big_num_carry_t;
#else
#define big_num_carry_t				big_num_strg_t
#endif

// let glsl know about some C types to make C code a bit easier to port
#ifdef GL_core_profile
#define size_t						uint
#define ssize_t						int
#endif

#ifndef GL_core_profile
typedef struct big_uint_s 	big_uint_t;
typedef big_uint_t		 	big_int_t;
typedef struct big_float_s 	big_float_t;

void big_uint_to_string(const big_uint_t * self, char * result, size_t result_len, size_t b);
void big_int_to_string(const big_int_t * self, char * result, size_t result_len, size_t b);
extern void big_float_to_string(const big_float_t * self, char * result, size_t result_len, size_t b,
	bool scient, ssize_t scient_from, ssize_t round_index, bool trim_zeroes, char comma);
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif

#endif // _BIG_NUM_H_

/**
 * @file BigUInt.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef _BIG_UINT_H_
#define _BIG_UINT_H_

#ifndef GL_core_profile
#include <stdint.h>
#include <stdbool.h>
#include "BigNum/BigNum.h"
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
extern "C" {
#endif

#define UINT_PREC BIG_NUM_PREC

struct big_uint_s {
	big_num_strg_t	size;
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
big_num_carry_t 	big_uint_add(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_carry_t c);
big_num_carry_t		big_uint_sub_uint(_big_num_inout(big_uint_t, self), big_num_strg_t val);
big_num_carry_t 	big_uint_sub(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_carry_t c);
big_num_strg_t 		big_uint_rcl(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c);
size_t	 			big_uint_compensation_to_left(_big_num_inout(big_uint_t, self));
bool 				big_uint_find_leading_bit(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(size_t, table_id), _big_num_out(size_t, index));
bool 				big_uint_find_lowest_bit(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(size_t, table_id), _big_num_out(size_t, index));
bool	 			big_uint_get_bit(_big_num_const_param _big_num_inout(big_uint_t, self), size_t bit_index);
bool	 			big_uint_set_bit(_big_num_inout(big_uint_t, self), size_t bit_index);
void 				big_uint_bit_and(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2));
void 				big_uint_bit_or(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2));
void 				big_uint_bit_xor(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2));
void 				big_uint_bit_not(_big_num_inout(big_uint_t, self));
void				big_uint_bit_not2(_big_num_inout(big_uint_t, self));


big_num_carry_t		big_uint_mul_int(_big_num_inout(big_uint_t, self), big_num_strg_t ss2);
big_num_carry_t		big_uint_mul(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_algo_t algorithm);
void				big_uint_mul_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), _big_num_out(big_uint_t, result), big_num_algo_t algorithm);

big_num_div_ret_t 	big_uint_div_int(_big_num_inout(big_uint_t, self), big_num_strg_t divisor, _big_num_out(big_num_strg_t, remainder));
big_num_div_ret_t 	big_uint_div(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), big_num_algo_t algorithm);

big_num_ret_t		big_uint_pow(_big_num_inout(big_uint_t, self), big_uint_t _pow);
void 				big_uint_sqrt(_big_num_inout(big_uint_t, self));


void 				big_uint_clear_first_bits(_big_num_inout(big_uint_t, self), size_t n);
bool 				big_uint_is_the_highest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self));
bool 				big_uint_is_the_lowest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self));
bool 				big_uint_is_only_the_highest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self));
bool 				big_uint_is_only_the_lowest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self));
bool 				big_uint_is_zero(_big_num_const_param _big_num_inout(big_uint_t, self));
bool 				big_uint_are_first_bits_zero(_big_num_const_param _big_num_inout(big_uint_t, self), size_t bits);

void				big_uint_init(_big_num_inout(big_uint_t, self), size_t size);
void 				big_uint_init_uint(_big_num_inout(big_uint_t, self), size_t size, big_num_strg_t value);
big_num_carry_t		big_uint_init_ulint(_big_num_inout(big_uint_t, self), size_t size, big_num_lstrg_t value);
big_num_carry_t		big_uint_init_big_uint(_big_num_inout(big_uint_t, self), size_t size, _big_num_const_param _big_num_inout(big_uint_t, value));
big_num_carry_t		big_uint_init_int(_big_num_inout(big_uint_t, self), size_t size, big_num_sstrg_t value);
big_num_carry_t		big_uint_init_lint(_big_num_inout(big_uint_t, self), size_t size, big_num_lsstrg_t value);
#define 			big_uint_set_uint(ptr, value) 		big_uint_init_uint(ptr, (ptr).size, value)
#define 			big_uint_set_ulint(ptr, value) 		big_uint_init_ulint(ptr, (ptr).size, value)
#define 			big_uint_set_big_uint(ptr, value) 	big_uint_init_big_uint(ptr, (ptr).size, value)
#define 			big_uint_set_int(ptr, value) 		big_uint_init_int(ptr, (ptr).size, value)
#define 			big_uint_set_lint(ptr, value) 		big_uint_init_lint(ptr, (ptr).size, value)
big_num_carry_t		big_uint_to_uint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_uint_to_int(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_sstrg_t, result));
big_num_carry_t		big_uint_to_luint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_lstrg_t, result));
big_num_carry_t		big_uint_to_lint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_lsstrg_t, result));

bool 				big_uint_cmp_smaller(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index);
bool 				big_uint_cmp_bigger(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index);
bool 				big_uint_cmp_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index);
bool 				big_uint_cmp_smaller_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index);
bool 				big_uint_cmp_bigger_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index);

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif

#endif // _BIG_UINT_H_

/**
 * @file BigInt.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#ifndef _BIG_INT_H_
#define _BIG_INT_H_

#ifndef GL_core_profile
#include "BigNum/BigNum.h"
#include "BigNum/BigUInt.h"
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
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
bool				big_int_is_sign(_big_num_const_param _big_num_inout(big_int_t, self));
big_num_strg_t		big_int_abs(_big_num_inout(big_int_t, self));

big_num_carry_t 	big_int_add(_big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, ss2));
big_num_strg_t		big_int_add_int(_big_num_inout(big_int_t, self), big_num_strg_t value, size_t index);
big_num_carry_t 	big_int_sub(_big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, ss2));
big_num_strg_t		big_int_sub_int(_big_num_inout(big_int_t, self), big_num_strg_t value, size_t index);

size_t	 			big_int_compensation_to_left(_big_num_inout(big_int_t, self));

big_num_carry_t		big_int_mul_int(_big_num_inout(big_int_t, self), big_num_sstrg_t ss2);
big_num_carry_t		big_int_mul(_big_num_inout(big_int_t, self), big_int_t ss2);

big_num_div_ret_t 	big_int_div_int(_big_num_inout(big_int_t, self), big_num_sstrg_t divisor, _big_num_out(big_num_sstrg_t, remainder));
big_num_div_ret_t 	big_int_div(_big_num_inout(big_int_t, self), big_int_t divisor, _big_num_out(big_int_t, remainder));

big_num_ret_t		big_int_pow(_big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, pow));

void				big_int_init(_big_num_inout(big_int_t, self), size_t size);
big_num_carry_t		big_int_init_uint(_big_num_inout(big_int_t, self), size_t size, big_num_strg_t value);
big_num_carry_t		big_int_init_ulint(_big_num_inout(big_int_t, self), size_t size, big_num_lstrg_t value);
big_num_carry_t		big_int_init_big_uint(_big_num_inout(big_int_t, self), size_t size, _big_num_const_param _big_num_inout(big_uint_t, value));
void				big_int_init_int(_big_num_inout(big_int_t, self), size_t size, big_num_sstrg_t value);
big_num_carry_t		big_int_init_lint(_big_num_inout(big_int_t, self), size_t size, big_num_lsstrg_t value);
big_num_carry_t		big_int_init_big_int(_big_num_inout(big_int_t, self), size_t size, _big_num_const_param _big_num_inout(big_int_t, value));
#define 			big_int_set_uint(ptr, value) 		big_int_init_uint(ptr, (ptr).size, value)
#define 			big_int_set_ulint(ptr, value) 		big_int_init_ulint(ptr, (ptr).size, value)
#define 			big_int_set_big_uint(ptr, value) 	big_int_init_big_uint(ptr, (ptr).size, value)
#define 			big_int_set_int(ptr, value) 		big_int_init_int(ptr, (ptr).size, value)
#define 			big_int_set_lint(ptr, value) 		big_int_init_lint(ptr, (ptr).size, value)
#define 			big_int_set_big_int(ptr, value) 	big_int_init_big_int(ptr, (ptr).size, value)
big_num_carry_t		big_int_to_uint(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_int_to_int(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_out(big_num_sstrg_t, result));
big_num_carry_t		big_int_to_luint(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_out(big_num_lstrg_t, result));
big_num_carry_t		big_int_to_lint(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_out(big_num_lsstrg_t, result));

bool 				big_int_cmp_smaller(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, l));
bool 				big_int_cmp_bigger(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, l));
bool 				big_int_cmp_equal(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, l));
bool 				big_int_cmp_smaller_equal(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, l));
bool 				big_int_cmp_bigger_equal(_big_num_const_param _big_num_inout(big_int_t, self), _big_num_const_param _big_num_inout(big_int_t, l));

#ifdef __cplusplus
}
#endif

#endif // _BIG_INT_H_

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

bool 				big_float_is_zero(_big_num_const_param _big_num_inout(big_float_t, self));
bool 				big_float_is_sign(_big_num_const_param _big_num_inout(big_float_t, self));
bool 				big_float_is_nan(_big_num_const_param _big_num_inout(big_float_t, self));

void 				big_float_abs(_big_num_inout(big_float_t, self));
big_num_carry_t		big_float_round(_big_num_inout(big_float_t, self));
void 				big_float_sgn(_big_num_inout(big_float_t, self));
void 				big_float_set_sign(_big_num_inout(big_float_t, self));

big_num_carry_t 	big_float_add(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, ss2), bool round);
big_num_carry_t 	big_float_sub(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, ss2), bool round);
big_num_carry_t 	big_float_mul_uint(_big_num_inout(big_float_t, self), big_num_strg_t ss2);
big_num_carry_t 	big_float_mul_int(_big_num_inout(big_float_t, self), big_num_sstrg_t ss2);
big_num_carry_t 	big_float_mul(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, ss2), bool round);

big_num_ret_t 		big_float_div(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, ss2), bool round);
big_num_ret_t 		big_float_mod(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, ss2));
big_num_strg_t 		big_float_mod2(_big_num_const_param _big_num_inout(big_float_t, self));

big_num_ret_t 		big_float_pow_big_uint(_big_num_inout(big_float_t, self), big_uint_t _pow);
big_num_ret_t 		big_float_pow_big_int(_big_num_inout(big_float_t, self), big_int_t _pow);
big_num_ret_t 		big_float_pow_big_frac(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, pow));
big_num_ret_t		big_float_pow(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, pow));
big_num_ret_t 		big_float_sqrt(_big_num_inout(big_float_t, self));
big_num_carry_t		big_float_exp(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, x));
big_num_ret_t 		big_float_ln(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, x));
big_num_ret_t 		big_float_log(_big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, x), _big_num_const_param _big_num_inout(big_float_t, base));

void				big_float_init(_big_num_inout(big_float_t, self), size_t man, size_t exp);
void 				big_float_init_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, float value);
void				big_float_init_double(_big_num_inout(big_float_t, self), size_t man, size_t exp, double value);
void 				big_float_init_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_strg_t value);
void 				big_float_init_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_num_sstrg_t value);
big_num_carry_t		big_float_init_big_float(_big_num_inout(big_float_t, self), size_t man, size_t exp, _big_num_const_param _big_num_inout(big_float_t, value));
void				big_float_init_big_uint(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_uint_t value);
void				big_float_init_big_int(_big_num_inout(big_float_t, self), size_t man, size_t exp, big_int_t value);
#define				big_float_set_double(ptr, value)		big_float_init_double(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
#define				big_float_set_uint(ptr, value)			big_float_init_uint(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
#define				big_float_set_int(ptr, value)			big_float_init_int(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
#define				big_float_set_big_float(ptr, value)		big_float_init_big_float(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
#define				big_float_set_big_uint(ptr, value)		big_float_init_big_uint(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
#define				big_float_set_big_int(ptr, value)		big_float_init_big_int(ptr, (ptr).mantissa.size, (ptr).exponent.size, value)
big_num_carry_t		big_float_to_double(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_out(double, result));
big_num_carry_t		big_float_to_float(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_out(float, result));
big_num_carry_t		big_float_to_uint(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_float_to_int(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_out(big_num_sstrg_t, result));

bool 				big_float_cmp_smaller(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, l));
bool 				big_float_cmp_bigger(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, l));
bool 				big_float_cmp_equal(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, l));
bool 				big_float_cmp_smaller_equal(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, l));
bool 				big_float_cmp_bigger_equal(_big_num_const_param _big_num_inout(big_float_t, self), _big_num_const_param _big_num_inout(big_float_t, l));

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif

#endif // _BIG_FLOAT_H_

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform big_float_t u_zoom;
uniform big_float_t u_offset_r;
uniform big_float_t u_offset_i;

#define PI 				3.1415926538

#define MAN_PREC		1
#define EXP_PREC		2

#define MAX_ITTERATIONS (10)
#define COLOR_REPEAT	1
#define DEBUG_SQUARE

const float ln_max_ittr = log(MAX_ITTERATIONS+1);

vec4 mandelbrot(double c_x, double c_y, double offset_r, double offset_i, double zoom);
void step_mandelbrot( inout double z_r_ptr, inout double z_i_ptr, double c_r, double c_i);
vec4 	integerToColor(in float i);


vec4 	mandelbrot_bignum(in vec2 c, in big_float_t offset_r, in big_float_t offset_i, in big_float_t zoom);
void 	step_mandelbrot_big_num(
			inout big_float_t z_r, inout big_float_t z_i, 
			in big_float_t c_r, in big_float_t c_i);

void main()
{
	vec2 translated = vec2((gl_FragCoord.x / u_resolution.x) - 0.5, (gl_FragCoord.y / u_resolution.y) - 0.5);

#ifdef DEBUG_SQUARE
	if (		all(greaterThan(translated.xy, vec2( 0.00,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.01)))){
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2( 0.00, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.00)))) {
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2(-0.01,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.01)))) {
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2(-0.01, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.00)))) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
#endif

	big_float_t zoom, offset_r, offset_i;

	big_float_init(zoom, MAN_PREC, EXP_PREC);
	big_float_set_double(zoom, pow(2, 2));

	big_float_init(offset_r, MAN_PREC, EXP_PREC);
	big_float_set_double(offset_r, 0.2);
	big_float_init(offset_i, MAN_PREC, EXP_PREC);
	big_float_set_double(offset_i, 0.0);

	// FragColor = mandelbrot_bignum(translated, offset_r, offset_i, zoom);
	double _zoom, _offset_r, _offset_i;
	zoom = u_zoom;
	offset_r = u_offset_r;
	offset_i = u_offset_i;
	big_float_to_double(zoom, _zoom);
	big_float_to_double(offset_r, _offset_r);
	big_float_to_double(offset_i, _offset_i);
	FragColor = mandelbrot(translated.x, translated.y, _offset_r, _offset_i, _zoom);
}

vec4 integerToColor(in float i)
{
	float angle = log(i+1.0) / log(256.0); // reduce to value between 0.0-1.0
	angle = angle * COLOR_REPEAT;
	angle = fract(angle);

	return vec4(
		0.5 * (1.0 + (cos((2.0*PI) * (angle            )))),
		0.5 * (1.0 - (cos((2.0*PI) * (angle - (1.0/3.0))))),
		0.5 * (1.0 + (cos((2.0*PI) * (angle + (1.0/3.0))))),
		1.0);
}

vec4 mandelbrot_bignum(in vec2 c, in big_float_t offset_r, in big_float_t offset_i, in big_float_t zoom)
{
	big_float_t c_r, c_i, z_r, z_i;
	// C = (x, y)
	big_float_init_double(c_r, MAN_PREC, EXP_PREC, c.x);
	big_float_init_double(c_i, MAN_PREC, EXP_PREC, c.y);
	big_float_init(z_r, MAN_PREC, EXP_PREC);
	big_float_init(z_i, MAN_PREC, EXP_PREC);
	// Z = (0, 0)
	big_float_set_zero(z_r);
	big_float_set_zero(z_i);

	// apply translation
	big_float_mul(c_r, zoom, true);
	big_float_mul(c_i, zoom, true);
	
	big_float_sub(c_r, offset_r, true);
	big_float_sub(c_i, offset_i, true);

	int itterations = 0;
	for (; itterations <= MAX_ITTERATIONS; itterations++) {
		big_float_t a_sqr, b_sqr, four;
		big_float_init_big_float(a_sqr, MAN_PREC, EXP_PREC, z_r);
		big_float_init_big_float(b_sqr, MAN_PREC, EXP_PREC, z_i);
        big_float_init_uint(four, MAN_PREC, EXP_PREC, 4U);
		big_float_mul(a_sqr, z_r, true); // a^2 = z_r^2
        big_float_mul(b_sqr, z_i, true); // b^2 = z_i^2
        big_float_add(a_sqr, b_sqr, true); // a^2 + b^2 = c^2
        
        if (big_float_cmp_bigger(a_sqr, four)) { // pretend a_sqr here is the length of z^2
            return integerToColor(itterations);
        }
        
		step_mandelbrot_big_num(z_r, z_i, c_r, c_i);
	}

	return vec4(0.0, 0.0, 0.0, 1.0);
}

void step_mandelbrot_big_num(
		inout big_float_t z_r, inout big_float_t z_i,
		in big_float_t c_r, in big_float_t c_i)
{
	big_float_t z_r_t, z_i_t, z_r_t2, z_i_t2;
	big_float_init_big_float(z_r_t, MAN_PREC, EXP_PREC, z_r);
	big_float_init_big_float(z_i_t, MAN_PREC, EXP_PREC, z_i);
	z_r_t2 = z_r_t;
	z_i_t2 = z_i_t;	
	// calculate 'z.real
	big_float_mul(z_r_t, z_r_t2, true); // z_r_t = z.real^2
	big_float_mul(z_i_t, z_i_t2, true);	// z_i_t = z.imag^2
	big_float_sub(z_r_t, z_i_t, true);	// z_r_t = z.real^2 - z.imag^2
	big_float_add(z_r_t, c_r, true);	// z_r_t = z.real^2 - z.imag^2 + c.real

	// calculate 'z.imag
	big_float_set_uint(z_i_t, 2);
	big_float_mul(z_i_t, z_r, true);	// 2 * z.real
	big_float_mul(z_i_t, z_i, true);	// 2 * z.real * z.imag
	big_float_add(z_i_t, c_i, true);	// 2 * z.real * z.imag + c.imag

	big_float_set_big_float(z_r, z_r_t);
	big_float_set_big_float(z_i, z_i_t);
}

void step_mandelbrot(
	inout double z_r_ptr, inout double z_i_ptr,
    double c_r, double c_i)
{
	double z_r, z_i, z_r_t, z_i_t;
    z_r = z_r_ptr;
    z_i = z_i_ptr;

    z_r_t = z_r * z_r - z_i * z_i + c_r;
    z_i_t = 2 * z_r * z_i + c_i;

    z_r_ptr = z_r_t;
    z_i_ptr = z_i_t;
}

vec4 mandelbrot(double c_x, double c_y, double offset_r, double offset_i, double zoom)
{
    double c_r = c_x, c_i = c_y, z_r = 0, z_i = 0;
    c_r *= zoom;
    c_i *= zoom;
    c_r -= offset_r;
    c_i -= offset_i;

    int itterations = 0;
    for (; itterations <= MAX_ITTERATIONS; itterations++) {
        if ((z_r*z_r + z_i*z_i) > 4.0) {
            return integerToColor(itterations);
        }
        step_mandelbrot(z_r, z_i, c_r, c_i);
    }

    return vec4(0.0, 0.0, 0.0, 1.0);
}