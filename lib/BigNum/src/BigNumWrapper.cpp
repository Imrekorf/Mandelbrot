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

#define BIG_NUM_EXP_PREC	2
#define BIG_NUM_MAN_PREC	1

/**
 * Convert float to string
 * @param[in] self the big num object
 * @param[out] result the string to hold the value in
 * @param[out] result_len allocated memory length of the result buffer
 * @param[in] b the base to convert to, range: [2, 16], default 10
 * @param[in] scient if true the value will always be shown in scientific notation, default false
 * @param[in] scient_from if scient is false then the value will be printed in scientific notation mode only if exponent is greater than scient_from, default 15
 * @param[in] round_index indicates how many digits after comma are possible, -1 will print all digits
 * @param[in] trim_zeroes if true, 1234,78000 -> 1234,78, default true
 * @param[in] comma the main comma operator, default '.'
 */
void big_float_to_string(const big_float_t * self, char * result, size_t result_len, size_t b,
	bool scient, ssize_t scient_from, ssize_t round_index, bool trim_zeroes, char comma)
{
	memset(result, 0, result_len);

	ttmath::Big<BIG_NUM_EXP_PREC, BIG_NUM_MAN_PREC> _self;
	for (size_t i = 0; i < BIG_NUM_EXP_PREC; i++)
		_self.exponent.table[i] = self->exponent.table[i];
	for (size_t i = 0; i < BIG_NUM_MAN_PREC; i++)
		_self.mantissa.table[i] = self->mantissa.table[i];
	
	_self.info = self->info;
	
	std::string _result;
	_self.ToString(_result, b, scient, scient_from, round_index, trim_zeroes, comma);
	memcpy(result, _result.c_str(), result_len < _result.length() ? result_len : _result.length());
}

#ifdef __cplusplus
}
#endif

std::ostream& operator<<(std::ostream& os, const big_int_t& n)
{
	char buffer[128];
	if (big_int_is_sign(&n))
		big_uint_to_string(&n, buffer, 128, 10);
	else
		big_int_to_string(&n, buffer, 128, 10);
    os << buffer;
    return os;
}

std::ostream& operator<<(std::ostream& os, const big_float_t& n)
{
	char buffer[128];
	big_float_to_string(&n, buffer, 128, 10, false, 15, -1, true, '.');
    os << buffer;
    return os;
}