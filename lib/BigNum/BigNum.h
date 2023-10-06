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

#ifdef __cplusplus
extern "C" {
#endif

#define BIG_NUM_MAX_STRG 			(big_num_strg_t  )((1ULL<<( 8ULL*sizeof(big_num_strg_t  )      ))-1ULL)
#define BIG_NUM_MAX_LSTRG 			(big_num_lstrg_t )((1ULL<<( 8ULL*sizeof(big_num_lstrg_t )      ))-1ULL)
#define BIG_NUM_MAX_SSTRG 			(big_num_sstrg_t )((1ULL<<((8ULL*sizeof(big_num_strg_t  ))-1ULL))-1ULL)
#define BIG_NUM_MAX_LSSTRG 			(big_num_lsstrg_t)((1ULL<<((8ULL*sizeof(big_num_lsstrg_t))-1ULL))-1ULL)
#define BIG_NUM_MIN_SSTRG 			(big_num_sstrg_t )((1ULL<<((8ULL*sizeof(big_num_strg_t  ))-1ULL)))
#define BIG_NUM_MIN_LSSTRG 			(big_num_lsstrg_t)((1ULL<<((8ULL*sizeof(big_num_lsstrg_t))-1ULL)))
#define BIG_NUM_HIGHEST_BIT			(big_num_strg_t  )((1ULL<<( 8ULL*sizeof(big_num_strg_t  )      )))
#define BIG_NUM_BITS_PER_STRG		(8ULL*sizeof(big_num_strg_t))
#define BIG_NUM_BITS_PER_LSTRG		(8ULL*sizeof(big_num_lstrg_t))
#define BIG_NUM_BITS_PER_SSTRG		(8ULL*sizeof(big_num_sstrg_t))
#define BIG_NUM_BITS_PER_LSSTRG		(8ULL*sizeof(big_num_lsstrg_t))

#define BIG_NUM_PREC_EXP 	3 // integer part precision
#define BIG_NUM_PREC_MAN 	3 // floating part precision

#define BIG_NUM_PREC_INT 	BIG_NUM_PREC_EXP
#define BIG_NUM_PREC_UINT 	BIG_NUM_PREC_MAN

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
	BIG_NUM_POW_OK = 0,
	/**
	 * carry
	 */
	BIG_NUM_POW_CARRY = 1,
	/**
	 * incorrect argument (0^0)
	 */
	BIG_NUM_POW_INVALID = 2,
} big_num_pow_ret_t;

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