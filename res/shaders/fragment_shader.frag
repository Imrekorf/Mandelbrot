#version 460 core
#extension GL_ARB_gpu_shader_int64 : require
precision highp float;

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

void big_uint_to_string(big_uint_t * self, char * result, size_t result_len, size_t b);
void big_int_to_string(big_int_t * self, char * result, size_t result_len, size_t b);
extern void big_float_to_string(big_float_t * self, char * result, size_t result_len, size_t b,
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
#define 			big_uint_set_uint(ptr, value) 		big_uint_init_uint(ptr, (ptr)->size, value)
#define 			big_uint_set_ulint(ptr, value) 		big_uint_init_ulint(ptr, (ptr)->size, value)
#define 			big_uint_set_big_uint(ptr, value) 	big_uint_init_big_uint(ptr, (ptr)->size, value)
#define 			big_uint_set_int(ptr, value) 		big_uint_init_int(ptr, (ptr)->size, value)
#define 			big_uint_set_lint(ptr, value) 		big_uint_init_lint(ptr, (ptr)->size, value)
big_num_carry_t		big_uint_to_uint(big_uint_t self, _big_num_out(big_num_strg_t, result));
big_num_carry_t		big_uint_to_int(big_uint_t self, _big_num_out(big_num_sstrg_t, result));
big_num_carry_t		big_uint_to_luint(big_uint_t self, _big_num_out(big_num_lstrg_t, result));
big_num_carry_t		big_uint_to_lint(big_uint_t self, _big_num_out(big_num_lsstrg_t, result));

bool 				big_uint_cmp_smaller(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_smaller_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger_equal(big_uint_t self, _big_num_const_param big_uint_t l, ssize_t index);

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
bool				big_int_is_sign(big_int_t self);
big_num_strg_t		big_int_abs(_big_num_inout(big_int_t, self));

big_num_carry_t 	big_int_add(_big_num_inout(big_int_t, self), big_int_t ss2);
big_num_strg_t		big_int_add_int(_big_num_inout(big_int_t, self), big_num_strg_t value, size_t index);
big_num_carry_t 	big_int_sub(_big_num_inout(big_int_t, self), big_int_t ss2);
big_num_strg_t		big_int_sub_int(_big_num_inout(big_int_t, self), big_num_strg_t value, size_t index);

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
#define 			big_int_set_uint(ptr, value) 		big_int_init_uint(ptr, (ptr)->size, value)
#define 			big_int_set_ulint(ptr, value) 		big_int_init_ulint(ptr, (ptr)->size, value)
#define 			big_int_set_big_uint(ptr, value) 	big_int_init_big_uint(ptr, (ptr)->size, value)
#define 			big_int_set_int(ptr, value) 		big_int_init_int(ptr, (ptr)->size, value)
#define 			big_int_set_lint(ptr, value) 		big_int_init_lint(ptr, (ptr)->size, value)
#define 			big_int_set_big_int(ptr, value) 	big_int_init_big_int(ptr, (ptr)->size, value)
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

#endif // _BIG_INT_H_



#define PRECISION 	(3)
#define ARRAY_SIZE 	(PRECISION+1)

uniform float 	AP_BASE 		= 4294967296.0;
uniform uint 	AP_HALF_BASE 	= 2147483648u;

void ap_assign(inout uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE]);					// a = b
void ap_zero(inout uint a[ARRAY_SIZE]);												// a = 0
void ap_load(inout uint a[ARRAY_SIZE], in float load_value);									// a = v
void ap_shift(inout uint a[ARRAY_SIZE], in uint shift_n);									// a << n
void ap_negate(inout uint a[ARRAY_SIZE]);											// a = -a
void ap_add(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]);	// r = a + b
void ap_mul(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]);	// r = a * b

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform uint u_zoom[ARRAY_SIZE];
uniform uint u_offset_r[ARRAY_SIZE];
uniform uint u_offset_i[ARRAY_SIZE];

#define PI 				3.1415926538

#define MAX_ITTERATIONS (256)
#define COLOR_REPEAT	3
#define DEBUG_SQUARE

const float ln_max_ittr = log(MAX_ITTERATIONS+1);

vec4 	mandelbrot(in dvec2 c);
dvec2 	step_mandelbrot(in dvec2 z, in dvec2 c);
vec4 	integerToColor(in float i);

vec4 	mandelbrot_arbprec(in vec2 c, in uint offset_r[ARRAY_SIZE], in uint offset_i[ARRAY_SIZE], in uint zoom[ARRAY_SIZE]);
void 	step_mandelbrot_arb_prec(
			in uint z_r[ARRAY_SIZE], in uint z_i[ARRAY_SIZE], 
			in uint c_r[ARRAY_SIZE], in uint c_i[ARRAY_SIZE], 
			out uint nz_r[ARRAY_SIZE], out uint nz_i[ARRAY_SIZE]);

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

	FragColor = mandelbrot_arbprec(translated, u_offset_r, u_offset_i, u_zoom);
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

vec4 mandelbrot_arbprec(in vec2 c, in uint offset_r[ARRAY_SIZE], in uint offset_i[ARRAY_SIZE], in uint zoom[ARRAY_SIZE])
{
	uint c_r[ARRAY_SIZE];
    uint c_i[ARRAY_SIZE];
    uint z_r[ARRAY_SIZE];
    uint z_i[ARRAY_SIZE];
	
	uint nz_r[ARRAY_SIZE];
    uint nz_i[ARRAY_SIZE];

	ap_load(c_r, c.x);
	ap_load(c_i, c.y);
	
	ap_mul(c_r, zoom, c_r);
	ap_mul(c_i, zoom, c_i);
	
	ap_negate(offset_r);
	ap_negate(offset_i);
	ap_add(c_r, offset_r, c_r);
	ap_add(c_i, offset_i, c_i);

	ap_zero(z_r);
	ap_zero(z_i);

	int itterations = 0;
	for (; itterations < MAX_ITTERATIONS; itterations++) {
		uint a_sqr[ARRAY_SIZE];
        uint b_sqr[ARRAY_SIZE];
        ap_mul(z_r, z_r, a_sqr);
        ap_mul(z_i, z_i, b_sqr);
        ap_add(a_sqr, b_sqr, a_sqr); // pretend a_sqr here is r_sqr
        
        if (a_sqr[1] > 4) { // pretend a_sqr here is r_sqr
            return integerToColor(itterations);
        }
        
		step_mandelbrot_arb_prec(z_r, z_i, c_r, c_i, nz_r, nz_i);
		ap_assign(z_r, nz_r);
		ap_assign(z_i, nz_i);
	}

	return vec4(0.0, 0.0, 0.0, 1.0);
}

void step_mandelbrot_arb_prec(
	in uint z_r[ARRAY_SIZE], in uint z_i[ARRAY_SIZE], 
	in uint c_r[ARRAY_SIZE], in uint c_i[ARRAY_SIZE], 
	out uint nz_r[ARRAY_SIZE], out uint nz_i[ARRAY_SIZE])
{
	uint tmp1[ARRAY_SIZE];
	uint tmp2[ARRAY_SIZE];
	// calculate 'z.real
	ap_mul(z_r, z_r, tmp1); 			// z.real^2
	ap_mul(z_i, z_i, tmp2); 			// z.imag^2
	ap_negate(tmp2);					// z.imag^2 * -1
	ap_add(tmp1, tmp2, tmp1);			// z.real^2 - z.imag^2
	ap_add(tmp1, c_r, nz_r);			// z.real^2 - z.imag^2 + c.real

	// calculate 'z.imag
	ap_load(tmp2, 2.0);
	ap_mul(z_r, tmp2, tmp1); 			// 2 * z.real
	ap_mul(tmp1, z_i, tmp2); 			// 2 * z.real * z.imag
	ap_add(tmp2, c_i, nz_i);			// 2 * z.real * z.imag + c.imag
}


// Arbitrary precision
// source: https://github.com/RohanFredriksson/glsl-arbitrary-precision

void ap_assign(inout uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE]) {
	for(int assign_i = 0; assign_i <= PRECISION; assign_i++)
		a[assign_i]=b[assign_i];
}

void ap_zero(inout uint a[ARRAY_SIZE]) {
	for(int zero_i = 0; zero_i <= PRECISION; zero_i++)
		a[zero_i] = 0u;
}

void ap_load(inout uint a[ARRAY_SIZE], in float load_value) {
	a[0] = int(load_value < 0.0);
	load_value *= load_value < 0.0 ? -1.0 : 1.0;

	for(int load_i = 1; load_i <= PRECISION; load_i++) {
		a[load_i] = uint(load_value);
		load_value -= a[load_i];
		load_value *= AP_BASE;
	}
}

void ap_shift(inout uint a[ARRAY_SIZE], in uint shift_n) {
	for(uint shift_i = shift_n+1; shift_i <= PRECISION; shift_i++)
		a[shift_i] = a[shift_i-shift_n];

	for(uint shift_i = 1; shift_i<=shift_n; shift_i++)
		a[shift_i] = 0u;
}

void ap_negate(inout uint a[ARRAY_SIZE]) {
	a[0] = a[0] == 0u ? 1u : 0u;
}

void ap_add(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]) {
	uint add_buffer[PRECISION+1]; 
	bool add_pa = a[0] == 0u; 
	bool add_pb = b[0] == 0u; 
	
	if(add_pa == add_pb) {
		uint add_carry = 0u;

		for(int add_i=  PRECISION; add_i > 0; add_i--) {
			uint add_next = uint(a[add_i] + b[add_i] < a[add_i]);
			add_buffer[add_i] = a[add_i] + b[add_i] + add_carry;
			add_carry = add_next;
		}
		add_buffer[0] = uint(!add_pa);

	} else {
		bool add_flip=false;

		for(int add_i = 1; add_i <= PRECISION; add_i++) {
			if(b[add_i] > a[add_i]) {
				add_flip=true; 
				break;
			} 
			if(a[add_i] > b[add_i])
				break;
		}

		uint add_borrow = 0u;
		if(add_flip) {
			for(int add_i = PRECISION; add_i > 0; add_i--) {
				add_buffer[add_i] = b[add_i] - a[add_i] - add_borrow; 
				add_borrow = uint(b[add_i] < a[add_i] + add_borrow);
			}
		} else {
			for(int add_i = PRECISION; add_i > 0; add_i--) {
				add_buffer[add_i] = a[add_i] - b[add_i] - add_borrow; 
				add_borrow = uint(a[add_i] < b[add_i] || a[add_i] < b[add_i] + add_borrow);
			}
		}

		add_buffer[0] = uint(add_pa == add_flip);
	}

	ap_assign(r, add_buffer);
}

void ap_mul(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]) {
	uint mul_buffer[PRECISION+1];
	uint mul_product[2*PRECISION-1];

	ap_zero(mul_buffer);
	for (uint mul_i = 0; mul_i < 2*PRECISION-1; mul_i++)
		mul_product[mul_i] = 0u;

	for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
		uint mul_carry = 0u; 
		for(int mul_j = 0; mul_j < PRECISION; mul_j++) {
			uint mul_next = 0; 
			uint mul_value = a[PRECISION-mul_i] * b[PRECISION-mul_j]; 
			if (mul_product[mul_i+mul_j] + mul_value < mul_product[mul_i+mul_j])
				mul_next++;
			
			mul_product[mul_i+mul_j] += mul_value; 
			if(mul_product[mul_i+mul_j] + mul_carry < mul_product[mul_i+mul_j])
				mul_next++;
			
			mul_product[mul_i+mul_j] += mul_carry; 
			uint mul_lower_a = a[PRECISION-mul_i] & 0xFFFF; 
			uint mul_upper_a = a[PRECISION-mul_i] >> 16; 
			uint mul_lower_b = b[PRECISION-mul_j] & 0xFFFF; 
			uint mul_upper_b = b[PRECISION-mul_j] >> 16; 
			uint mul_lower = mul_lower_a * mul_lower_b; 
			uint mul_upper = mul_upper_a * mul_upper_b; 
			uint mul_mid = mul_lower_a * mul_upper_b; 
			mul_upper += mul_mid >> 16;
			mul_mid = mul_mid << 16;

			if (mul_lower+mul_mid<mul_lower)
				mul_upper++;

			mul_lower += mul_mid; 
			mul_mid = mul_lower_b * mul_upper_a;
			mul_upper += mul_mid >> 16;
			mul_mid = mul_mid << 16;
			
			if(mul_lower + mul_mid < mul_lower)
				mul_upper++;
			
			mul_carry = mul_upper + mul_next;
		}
		
		if(mul_i + PRECISION < 2*PRECISION-1)
			mul_product[mul_i+PRECISION] += mul_carry;

	}
	if(mul_product[PRECISION-2] >= AP_HALF_BASE) {
		for(int mul_i = PRECISION-1; mul_i < 2*PRECISION-1; mul_i++) {
			if(mul_product[mul_i] + 1 > mul_product[mul_i]) {
				mul_product[mul_i]++;
				break;
			}
			mul_product[mul_i]++;
		}
	}
	for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
		mul_buffer[mul_i+1] = mul_product[2*PRECISION-2-mul_i];
	} if((a[0] == 0u) != (b[0] == 0u)) {
		mul_buffer[0] = 1u;
	}

	ap_assign(r, mul_buffer);
}