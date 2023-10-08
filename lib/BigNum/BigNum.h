/**
 * @file BigNum.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#ifndef __BIG_NUM_H__
#define __BIG_NUM_H__

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BIG_NUM_MAX_VALUE 			(big_num_strg_t  )((1ULL<<( CHAR_BIT*sizeof(big_num_strg_t  )      ))-1ULL)
#define BIG_NUM_HIGHEST_BIT			(big_num_strg_t  )((1ULL<<( CHAR_BIT*sizeof(big_num_strg_t  ) -1ULL)))
#define BIG_NUM_BITS_PER_UNIT		(CHAR_BIT*sizeof(big_num_strg_t))

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

#define BIG_NUM_PREC	 	2 // amount of word's to allocate for uint and int, float using 2x this amount

#define BIG_NUM_MUL_DEF		BIG_NUM_MUL1
#define BIG_NUM_DIV_DEF		BIG_NUM_DIV1

/**
 * Multiplication algorithm selector and
 * Division algorithm selector
 */
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

/**
 * return results for big_uint_div
 */
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

/**
 * return results for big_*type*_pow
 */
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


/**
 * The type used to store the big num values in
 */
typedef uint32_t big_num_strg_t;

/**
 * Signed version of the big_num_strg_t
 */
typedef int32_t  big_num_sstrg_t;

/**
 * Signed version of the big_num_strg_t
 */
typedef uint64_t  big_num_lstrg_t;

/**
 * Signed version of the big_num_strg_t
 */
typedef int64_t  big_num_lsstrg_t;

/**
 * Carry type, either 1 or 0
 */
typedef big_num_strg_t  big_num_carry_t;

#ifdef __cplusplus
}
#endif

#endif // __BIG_NUM_H__