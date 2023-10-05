/**
 * @file BigFloat.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef __BIG_FLOAT_H__
#define __BIG_FLOAT_H__

#include "BigNum/BigNum.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigUInt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	big_int_t exponent;
	big_uint_t mantissa;
	unsigned char info;
} big_float_t;

#ifdef __cplusplus
}
#endif

#endif // __BIG_FLOAT_H__