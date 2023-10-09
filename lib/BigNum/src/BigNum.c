/**
 * @file BigNum.c
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */


#include "BigNum/BigNum.h"
#include "BigNum/BigUInt.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigFloat.h"

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

struct big_float_conv_s {
	size_t base;
	bool scient;
	ssize_t scient_from;
	ssize_t round_index;
	bool trim_zeroes;
	char sep;
};

double 	_big_uint_to_string_log2(size_t x);
void 	_big_uint_to_string_base(big_uint_t self, char * result, size_t result_len, size_t b, bool negative);
void	_big_float_to_string_base(big_float_t self, char * result, size_t result_len, struct big_float_conv_s conv);
void 	_big_float_skip_fraction(big_float_t* self);

/**
 * Convert uint to string
 * @param[in] self the big num object
 * @param[out] result the string to hold the value in
 * @param[out] result_len allocated memory length of the result buffer
 * @param[in] b the base to convert to, range: [2, 16]
 */
void big_uint_to_string(big_uint_t * self, char * result, size_t result_len, size_t b)
{
	_big_uint_to_string_base(*self, result, result_len, b, false);
}

/**
 * Convert int to string
 * @param[in] self the big num object
 * @param[out] result the string to hold the value in
 * @param[out] result_len allocated memory length of the result buffer
 * @param[in] b the base to convert to, range: [2, 16]
 */
void big_int_to_string(big_int_t * self, char * result, size_t result_len, size_t b)
{
	big_int_t temp = *self;
	if (big_int_is_sign(temp)) {
		big_int_abs(&temp);
		_big_uint_to_string_base(temp, result, result_len, b, true);
	} else {
		_big_uint_to_string_base(*self, result, result_len, b, false);
	}
}

/**
 * an auxiliary method for converting into the string
 * it returns the log (with the base 2) from x
 * where x is in <2;16>
 * @param[in] x 
 * @return double 
 */
double _big_uint_to_string_log2(size_t x)
{
	static const double log_tab[] = {
		1.000000000000000000,
		0.630929753571457437,
		0.500000000000000000,
		0.430676558073393050,
		0.386852807234541586,
		0.356207187108022176,
		0.333333333333333333,
		0.315464876785728718,
		0.301029995663981195,
		0.289064826317887859,
		0.278942945651129843,
		0.270238154427319741,
		0.262649535037193547,
		0.255958024809815489,
		0.250000000000000000
	};

	if( x<2 || x>16 )
		return 0;

	return log_tab[x-2];
}

/**
 * an auxiliary method for converting to a string
 * @param[in] self the big num object
 * @param[out] result the string to hold the value in
 * @param[in] result_len allocated memory length of result buffer 
 * @param[in] b the base
 * @param[in] negative set to true for big_int_t
 */
void _big_uint_to_string_base(big_uint_t self, char * result, size_t result_len, size_t b, bool negative)
{
	if (!result || !result_len)
		return;

	big_uint_t temp = self;
	big_num_strg_t rest, digits;
	size_t table_id, index;
	double digits_d;
	char character;
	size_t result_index = 0;
	
	memset(result, 0, result_len);

	if (b < 2 || b > 16)
		return;
	

	if (!big_uint_find_leading_bit(self, &table_id, &index)) {
		result[result_index++] = '0';
		return;
	}

	if (negative)
		result[result_index++] = '-';
	
	digits_d  = (double)(table_id); // for not making an overflow in uint type
	digits_d *= BIG_NUM_BITS_PER_UNIT;
	digits_d += index + 1;
	digits_d *= _big_uint_to_string_log2(b);
	digits = (big_num_strg_t)(digits_d) + 1 + result_index; // plus some epsilon

	printf("digits: %d\n", digits);
	
	result_index = digits;
	do {
		big_uint_div_int(&temp, b, &rest);
		if (--result_index < result_len)
			result[result_index] = (char)((rest < 10) ? (rest + '0') : (rest - 10 + 'A'));
	} while (!big_uint_is_zero(temp));
}

#ifdef __cplusplus
}
#endif