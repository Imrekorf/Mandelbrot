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