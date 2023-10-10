#include "BigNum/BigNum.h"
#include "BigNum/BigUint.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigFloat.h"

#include "BigNum/BigNumWrapper.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
#include "ttmath.h"
#pragma GCC diagnostic pop

#include <cstring>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Convert float to string
 * @param[in] self the big num object
 * @param[out] result the string to hold the value in
 * @param[out] result_len allocated memory length of the result buffer
 * @param[in] b the base to convert to, range: [2, 16]
 */
void big_float_to_string(big_float_t * self, char * result, size_t result_len, size_t b,
	bool scient, ssize_t scient_from, ssize_t round_index, bool trim_zeroes, char comma)
{
	memset(result, 0, result_len);

	ttmath::Big<BIG_NUM_PREC, BIG_NUM_PREC> _self;
	for (size_t i = 0; i < BIG_NUM_PREC; i++) {
		_self.mantissa.table[i] = self->mantissa.table[i];
		_self.exponent.table[i] = self->exponent.table[i];
	}
	_self.info = self->info;
	
	std::string _result;
	_self.ToString(_result, b, scient, scient_from, round_index, trim_zeroes, comma);
	memcpy(result, _result.c_str(), result_len < _result.length() ? result_len : _result.length());
}

#ifdef __cplusplus
}
#endif