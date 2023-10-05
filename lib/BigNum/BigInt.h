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

typedef struct {
	/*!
		buffer for the integer value
		  table[0] - the lowest word of the value
	*/
	unsigned int table[BIG_NUM_PREC_INT];
} big_int_t;


/*!
	this method converts an big_int_t type to this class

	this operation has mainly sense if the value from p is 
	equal or smaller than that one which is returned from big_uint_t::SetMax()

	it returns a carry if the value 'p' is too big
*/
uint32_t big_uint_from_big_int(const big_int_t * p);

#ifdef __cplusplus
}
#endif

#endif // __BIG_INT_H__