/**
 * @file BigFloat.h
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf
 */

#include "BigNum/BigNum.h"
#include "BigNum/BigInt.h"
#include "BigNum/BigFloat.h"

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BIG_FLOAT_INFO_SIGN 	(big_float_info_t)(1 << 7)
#define BIG_FLOAT_INFO_NAN		(big_float_info_t)(1 << 6)
#define BIG_FLOAT_INFO_ZERO		(big_float_info_t)(1 << 5)

extern big_num_sstrg_t  _big_uint_find_leading_bit_in_word(big_num_strg_t x);

static big_num_carry_t 	_big_float_check_carry(big_float_t * self, big_num_carry_t c);
static big_num_carry_t 	_big_float_standardizing(big_float_t * self);
static bool				_big_float_correct_zero(big_float_t * self);
static void				_big_float_clear_info_bit(big_float_t * self, big_float_info_t bit);
static void				_big_float_set_info_bit(big_float_t * self, big_float_info_t bit);
static bool				_big_float_is_info_bit(big_float_t self, big_float_info_t bit);

static big_num_carry_t 	_big_float_round_half_to_even(big_float_t * self, bool is_half, bool rounding_up);
static void 			_big_float_add_check_exponents(big_float_t* ss2, big_int_t exp_offset, bool * last_bit_set, bool * rest_zero, bool * do_adding, bool * do_rounding);
static big_num_carry_t 	_big_float_add_mantissas(big_float_t* self, big_float_t* ss2, bool * last_bit_set, bool * rest_zero);
big_num_carry_t 		_big_float_add(big_float_t * self, big_float_t ss2, bool round, bool adding);
bool 					_big_float_check_greater_or_equal_half(big_float_t * self, big_num_strg_t * tab, big_num_strg_t len);
big_num_ret_t 			_big_float_pow_big_float_uint(big_float_t * self, big_float_t pow);
big_num_ret_t 			_big_float_pow_big_float_int(big_float_t * self, big_float_t pow);
static void 			_big_float_exp_surrounding_0(big_float_t * self, big_float_t x, size_t * steps);
static void 			_big_float_ln_surrounding_1(big_float_t * self, big_float_t x, size_t * steps);
static void 			_big_float_init_uint_or_int(big_float_t * self, big_uint_t value, big_num_sstrg_t compensation);
static big_num_carry_t 	_big_float_to_uint_or_int(big_float_t self, big_num_strg_t * result);
static void 			_big_float_init_double_set_exp_and_man(big_float_t * self, bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2);
static double 			_big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan);
static bool 			_big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2);
static bool				_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2);
static bool				_big_float_equal_without_sign(big_float_t self, big_float_t ss2);
static void 			_big_float_skip_fraction(big_float_t* self);
static bool 			_big_float_is_integer(big_float_t self);
/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_float_set_zero(big_float_t * self)
{
	self->info = BIG_FLOAT_INFO_ZERO;
	big_int_set_zero(&self->exponent);
	big_uint_set_zero(&self->mantissa);
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_float_set_one(big_float_t * self)
{
	self->info = 0;
	big_uint_set_zero(&self->mantissa);
	self->mantissa.table[BIG_NUM_PREC-1] = BIG_NUM_HIGHEST_BIT;
	big_int_init_int(&self->exponent, -(big_num_sstrg_t)(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - 1));
}

/**
 * this method sets 0.5
 * @param[in, out] self the big num object
 */
void big_float_set_05(big_float_t * self)
{
	big_float_set_one(self);
	big_int_sub_int(&self->exponent, 1);
}

/**
 * this method sets the value of e
 * (the base of the natural logarithm)
 * @param[in] self the big num object
 */
void big_float_set_e(big_float_t * self)
{
	static const unsigned int temp_table[] = {
		0xadf85458, 0xa2bb4a9a, 0xafdc5620, 0x273d3cf1, 0xd8b9c583, 0xce2d3695, 0xa9e13641, 0x146433fb, 
		0xcc939dce, 0x249b3ef9, 0x7d2fe363, 0x630c75d8, 0xf681b202, 0xaec4617a, 0xd3df1ed5, 0xd5fd6561, 
		0x2433f51f, 0x5f066ed0, 0x85636555, 0x3ded1af3, 0xb557135e, 0x7f57c935, 0x984f0c70, 0xe0e68b77, 
		0xe2a689da, 0xf3efe872, 0x1df158a1, 0x36ade735, 0x30acca4f, 0x483a797a, 0xbc0ab182, 0xb324fb61, 
		0xd108a94b, 0xb2c8e3fb, 0xb96adab7, 0x60d7f468, 0x1d4f42a3, 0xde394df4, 0xae56ede7, 0x6372bb19, 
		0x0b07a7c8, 0xee0a6d70, 0x9e02fce1, 0xcdf7e2ec, 0xc03404cd, 0x28342f61, 0x9172fe9c, 0xe98583ff, 
		0x8e4f1232, 0xeef28183, 0xc3fe3b1b, 0x4c6fad73, 0x3bb5fcbc, 0x2ec22005, 0xc58ef183, 0x7d1683b2, 
		0xc6f34a26, 0xc1b2effa, 0x886b4238, 0x611fcfdc, 0xde355b3b, 0x6519035b, 0xbc34f4de, 0xf99c0238, 
		0x61b46fc9, 0xd6e6c907, 0x7ad91d26, 0x91f7f7ee, 0x598cb0fa, 0xc186d91c, 0xaefe1309, 0x85139270, 
		0xb4130c93, 0xbc437944, 0xf4fd4452, 0xe2d74dd3, 0x64f2e21e, 0x71f54bff, 0x5cae82ab, 0x9c9df69e, 
		0xe86d2bc5, 0x22363a0d, 0xabc52197, 0x9b0deada, 0x1dbf9a42, 0xd5c4484e, 0x0abcd06b, 0xfa53ddef, 
		0x3c1b20ee, 0x3fd59d7c, 0x25e41d2b, 0x669e1ef1, 0x6e6f52c3, 0x164df4fb, 0x7930e9e4, 0xe58857b6, 
		0xac7d5f42, 0xd69f6d18, 0x7763cf1d, 0x55034004, 0x87f55ba5, 0x7e31cc7a, 0x7135c886, 0xefb4318a, 
		0xed6a1e01, 0x2d9e6832, 0xa907600a, 0x918130c4, 0x6dc778f9, 0x71ad0038, 0x092999a3, 0x33cb8b7a, 
		0x1a1db93d, 0x7140003c, 0x2a4ecea9, 0xf98d0acc, 0x0a8291cd, 0xcec97dcf, 0x8ec9b55a, 0x7f88a46b, 
		0x4db5a851, 0xf44182e1, 0xc68a007e, 0x5e0dd902, 0x0bfd64b6, 0x45036c7a, 0x4e677d2c, 0x38532a3a, 
		0x23ba4442, 0xcaf53ea6, 0x3bb45432, 0x9b7624c8, 0x917bdd64, 0xb1c0fd4c, 0xb38e8c33, 0x4c701c3a, 
		0xcdad0657, 0xfccfec71, 0x9b1f5c3e, 0x4e46041f, 0x388147fb, 0x4cfdb477, 0xa52471f7, 0xa9a96910, 
		0xb855322e, 0xdb6340d8, 0xa00ef092, 0x350511e3, 0x0abec1ff, 0xf9e3a26e, 0x7fb29f8c, 0x183023c3, 
		0x587e38da, 0x0077d9b4, 0x763e4e4b, 0x94b2bbc1, 0x94c6651e, 0x77caf992, 0xeeaac023, 0x2a281bf6, 
		0xb3a739c1, 0x22611682, 0x0ae8db58, 0x47a67cbe, 0xf9c9091b, 0x462d538c, 0xd72b0374, 0x6ae77f5e, 
		0x62292c31, 0x1562a846, 0x505dc82d, 0xb854338a, 0xe49f5235, 0xc95b9117, 0x8ccf2dd5, 0xcacef403, 
		0xec9d1810, 0xc6272b04, 0x5b3b71f9, 0xdc6b80d6, 0x3fdd4a8e, 0x9adb1e69, 0x62a69526, 0xd43161c1, 
		0xa41d570d, 0x7938dad4, 0xa40e329c, 0xcff46aaa, 0x36ad004c, 0xf600c838, 0x1e425a31, 0xd951ae64, 
		0xfdb23fce, 0xc9509d43, 0x687feb69, 0xedd1cc5e, 0x0b8cc3bd, 0xf64b10ef, 0x86b63142, 0xa3ab8829, 
		0x555b2f74, 0x7c932665, 0xcb2c0f1c, 0xc01bd702, 0x29388839, 0xd2af05e4, 0x54504ac7, 0x8b758282, 
		0x2846c0ba, 0x35c35f5c, 0x59160cc0, 0x46fd8251, 0x541fc68c, 0x9c86b022, 0xbb709987, 0x6a460e74, 
		0x51a8a931, 0x09703fee, 0x1c217e6c, 0x3826e52c, 0x51aa691e, 0x0e423cfc, 0x99e9e316, 0x50c1217b, 
		0x624816cd, 0xad9a95f9, 0xd5b80194, 0x88d9c0a0, 0xa1fe3075, 0xa577e231, 0x83f81d4a, 0x3f2fa457, 
		0x1efc8ce0, 0xba8a4fe8, 0xb6855dfe, 0x72b0a66e, 0xded2fbab, 0xfbe58a30, 0xfafabe1c, 0x5d71a87e, 
		0x2f741ef8, 0xc1fe86fe, 0xa6bbfde5, 0x30677f0d, 0x97d11d49, 0xf7a8443d, 0x0822e506, 0xa9f4614e, 
		0x011e2a94, 0x838ff88c, 0xd68c8bb7, 0xc51eef6d, 0x49ea8ab4, 0xf2c3df5b, 0xb4e0735a, 0xb0d68749
	// 0x2fe26dd4, ...
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};

	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using ExpSurrounding0(1) method
	// which took 1420 iterations
	// (the result was compared with e taken from http://antwrp.gsfc.nasa.gov/htmltest/gifcity/e.2mil)

	big_uint_set_from_table(&self->mantissa, temp_table, sizeof(temp_table) / sizeof(int));
	big_int_init_int(&self->exponent, -(big_num_sstrg_t)(BIG_NUM_PREC)*(big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT) + 2);
	self->info = 0;
}

/**
 * this method sets the value of ln(2)
 * the natural logarithm from 2
 * @param[in] self the big num object
 */
void big_float_set_ln2(big_float_t * self)
{
	static const big_num_strg_t temp_table[] = {
		0xb17217f7, 0xd1cf79ab, 0xc9e3b398, 0x03f2f6af, 0x40f34326, 0x7298b62d, 0x8a0d175b, 0x8baafa2b, 
		0xe7b87620, 0x6debac98, 0x559552fb, 0x4afa1b10, 0xed2eae35, 0xc1382144, 0x27573b29, 0x1169b825, 
		0x3e96ca16, 0x224ae8c5, 0x1acbda11, 0x317c387e, 0xb9ea9bc3, 0xb136603b, 0x256fa0ec, 0x7657f74b, 
		0x72ce87b1, 0x9d6548ca, 0xf5dfa6bd, 0x38303248, 0x655fa187, 0x2f20e3a2, 0xda2d97c5, 0x0f3fd5c6, 
		0x07f4ca11, 0xfb5bfb90, 0x610d30f8, 0x8fe551a2, 0xee569d6d, 0xfc1efa15, 0x7d2e23de, 0x1400b396, 
		0x17460775, 0xdb8990e5, 0xc943e732, 0xb479cd33, 0xcccc4e65, 0x9393514c, 0x4c1a1e0b, 0xd1d6095d, 
		0x25669b33, 0x3564a337, 0x6a9c7f8a, 0x5e148e82, 0x074db601, 0x5cfe7aa3, 0x0c480a54, 0x17350d2c, 
		0x955d5179, 0xb1e17b9d, 0xae313cdb, 0x6c606cb1, 0x078f735d, 0x1b2db31b, 0x5f50b518, 0x5064c18b, 
		0x4d162db3, 0xb365853d, 0x7598a195, 0x1ae273ee, 0x5570b6c6, 0x8f969834, 0x96d4e6d3, 0x30af889b, 
		0x44a02554, 0x731cdc8e, 0xa17293d1, 0x228a4ef9, 0x8d6f5177, 0xfbcf0755, 0x268a5c1f, 0x9538b982, 
		0x61affd44, 0x6b1ca3cf, 0x5e9222b8, 0x8c66d3c5, 0x422183ed, 0xc9942109, 0x0bbb16fa, 0xf3d949f2, 
		0x36e02b20, 0xcee886b9, 0x05c128d5, 0x3d0bd2f9, 0x62136319, 0x6af50302, 0x0060e499, 0x08391a0c, 
		0x57339ba2, 0xbeba7d05, 0x2ac5b61c, 0xc4e9207c, 0xef2f0ce2, 0xd7373958, 0xd7622658, 0x901e646a, 
		0x95184460, 0xdc4e7487, 0x156e0c29, 0x2413d5e3, 0x61c1696d, 0xd24aaebd, 0x473826fd, 0xa0c238b9, 
		0x0ab111bb, 0xbd67c724, 0x972cd18b, 0xfbbd9d42, 0x6c472096, 0xe76115c0, 0x5f6f7ceb, 0xac9f45ae, 
		0xcecb72f1, 0x9c38339d, 0x8f682625, 0x0dea891e, 0xf07afff3, 0xa892374e, 0x175eb4af, 0xc8daadd8, 
		0x85db6ab0, 0x3a49bd0d, 0xc0b1b31d, 0x8a0e23fa, 0xc5e5767d, 0xf95884e0, 0x6425a415, 0x26fac51c, 
		0x3ea8449f, 0xe8f70edd, 0x062b1a63, 0xa6c4c60c, 0x52ab3316, 0x1e238438, 0x897a39ce, 0x78b63c9f, 
		0x364f5b8a, 0xef22ec2f, 0xee6e0850, 0xeca42d06, 0xfb0c75df, 0x5497e00c, 0x554b03d7, 0xd2874a00, 
		0x0ca8f58d, 0x94f0341c, 0xbe2ec921, 0x56c9f949, 0xdb4a9316, 0xf281501e, 0x53daec3f, 0x64f1b783, 
		0x154c6032, 0x0e2ff793, 0x33ce3573, 0xfacc5fdc, 0xf1178590, 0x3155bbd9, 0x0f023b22, 0x0224fcd8, 
		0x471bf4f4, 0x45f0a88a, 0x14f0cd97, 0x6ea354bb, 0x20cdb5cc, 0xb3db2392, 0x88d58655, 0x4e2a0e8a, 
		0x6fe51a8c, 0xfaa72ef2, 0xad8a43dc, 0x4212b210, 0xb779dfe4, 0x9d7307cc, 0x846532e4, 0xb9694eda, 
		0xd162af05, 0x3b1751f3, 0xa3d091f6, 0x56658154, 0x12b5e8c2, 0x02461069, 0xac14b958, 0x784934b8, 
		0xd6cce1da, 0xa5053701, 0x1aa4fb42, 0xb9a3def4, 0x1bda1f85, 0xef6fdbf2, 0xf2d89d2a, 0x4b183527, 
		0x8fd94057, 0x89f45681, 0x2b552879, 0xa6168695, 0xc12963b0, 0xff01eaab, 0x73e5b5c1, 0x585318e7, 
		0x624f14a5, 0x1a4a026b, 0x68082920, 0x57fd99b6, 0x6dc085a9, 0x8ac8d8ca, 0xf9eeeea9, 0x8a2400ca, 
		0xc95f260f, 0xd10036f9, 0xf91096ac, 0x3195220a, 0x1a356b2a, 0x73b7eaad, 0xaf6d6058, 0x71ef7afb, 
		0x80bc4234, 0x33562e94, 0xb12dfab4, 0x14451579, 0xdf59eae0, 0x51707062, 0x4012a829, 0x62c59cab, 
		0x347f8304, 0xd889659e, 0x5a9139db, 0x14efcc30, 0x852be3e8, 0xfc99f14d, 0x1d822dd6, 0xe2f76797, 
		0xe30219c8, 0xaa9ce884, 0x8a886eb3, 0xc87b7295, 0x988012e8, 0x314186ed, 0xbaf86856, 0xccd3c3b6, 
		0xee94e62f, 0x110a6783, 0xd2aae89c, 0xcc3b76fc, 0x435a0ce1, 0x34c2838f, 0xd571ec6c, 0x1366a993 // last one was: 0x1366a992
	//0xcbb9ac40, ...
	// (the last word 0x1366a992 was rounded up because the next one is 0xcbb9ac40 -- first bit is one 0xc..)
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};	

	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using LnSurrounding1(2) method
	// which took 4035 iterations
	// (the result was compared with ln(2) taken from http://ja0hxv.calico.jp/pai/estart.html)

	big_uint_set_from_table(&self->mantissa, temp_table, sizeof(temp_table) / sizeof(big_num_sstrg_t));
	big_int_init_int(&self->exponent, -(big_num_sstrg_t)(BIG_NUM_PREC)*(big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT));
	self->info = 0;
}

/**
 * this method sets NaN flag
 * @param[in, out] self the big num object
 */
void big_float_set_nan(big_float_t * self)
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method sets nan flag, also clears mantissa and exponent
 * @param[in, out] self the big num object
 */
void big_float_set_zero_nan(big_float_t * self)
{
	big_float_set_zero(self);
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method swappes this for an argument
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the big num to swap with
 */
void big_float_swap(big_float_t * self, big_float_t * ss2)
{
	big_float_info_t info_temp = self->info;
	self->info = ss2->info;
	ss2->info = info_temp;

	big_int_swap(&self->exponent, &ss2->exponent);
	big_uint_swap(&self->mantissa, &ss2->mantissa);
}

/**
 * this method sets the maximum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_max(big_float_t* self)
{
	self->info = 0;
	big_int_set_max(&self->mantissa);
	big_uint_set_max(&self->exponent);
}

/**
 * this method sets the minimum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_min(big_float_t* self)
{
	self->info = 0;
	big_int_set_max(&self->mantissa);
	big_uint_set_max(&self->exponent);
	big_float_set_sign(self);
}

/**
 * test whether there is a value zero or not
 * @param[in] self the big num object 
 * @return true the big num object is zero
 * @return false the big num object is not zero
 */
bool big_float_is_zero(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_ZERO);
}

/**
 * this method returns true when there's the sign set
 * also we don't check the NaN flag
 * @param[in] self the big num object 
 * @return true the big num object is negative
 * @return false the big num object is positive
 */
bool big_float_is_sign(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * this method returns true when there is not a valid number
 * @param[in] self the big num object 
 * @return true the big num object is nan
 * @return false the big num object is not nan
 */
bool big_float_is_nan(big_float_t self)
{
	return _big_float_is_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method clears the sign 
 * (there'll be an absolute value)
 * 
 * samples
 * -  	-1 -> 1
 * -  	2  -> 2
 * @param[in, out] self the big num object 
 */
void big_float_abs(big_float_t * self)
{
	_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);	
}

/**
 * this method rounds to the nearest integer value
 * (it returns a carry if it was)
 * 
 * samples:
 * -	2.3 = 2
 * -	2.8 = 3
 * -	-2.3 = -2
 * -	-2.8 = 3
 * @param[in] self the big num object
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_round(big_float_t * self)
{
	big_float_t half;
	big_num_carry_t c;
	if (big_float_is_nan(*self))
		return 1;
	if (big_float_is_zero(*self))
		return 0;
	
	big_float_set_05(&half);

	if (big_float_is_sign(*self)) {
		// self < 0
		c = big_float_sub(self, half, true);
	} else {
		// self > 0
		c = big_float_add(self, half, true);
	}

	_big_float_skip_fraction(self);

	return _big_float_check_carry(self, c);
}

/**
 * this method remains the 'sign' of the value
 * 
 * samples
 * -    -2 = -1
 * -     0 = 0
 * -    10 = 1
 * @param[in, out] self the big num object 
 */
void big_float_sgn(big_float_t * self)
{
	if (big_float_is_nan(*self) )
		return;
	if (big_float_is_sign(*self)) {
		big_float_set_one(self);
		big_float_set_sign(self);
	} else if (big_float_is_zero(*self)) {
		big_float_set_zero(self);
	} else {
		big_float_set_one(self);
	}
}

/**
 * this method sets the sign
 * 
 * samples
 * -  	-1 -> -1
 * -  	2  -> -2
 * 
 * we do not check whether there is a zero or not, if you're using this method
 * you must be sure that the value is (or will be afterwards) different from zero
 * @param[in, out] self the big num object 
 */
void big_float_set_sign(big_float_t * self)
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * this method changes the sign
 * when there is a value of zero then the sign is not changed
 * 
 * samples
 * -  	-1 ->  1
 * -  	 2 -> -2
 * @param[in, out] self the big num object 
 */
void big_float_change_sign(big_float_t * self)
{
	// we don't have to check the NaN flag here
	if( big_float_is_zero(*self) )
		return;

	if( big_float_is_sign(*self) )
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);
	else
		_big_float_set_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * Addition self = self + ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round true if round result
 * @return big_num_carry_t >0 if the result is too big
 */
big_num_carry_t big_float_add(big_float_t * self, big_float_t ss2, bool round)
{
	return _big_float_add(self, ss2, round, true);
}

/**
 * Subtraction self = self - ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round true if round result
 * @return big_num_carry_t >0 if the result is too big
 */
big_num_carry_t big_float_sub(big_float_t * self, big_float_t ss2, bool round)
{
	return _big_float_add(self, ss2, round, false);
}

/**
 * Multiplication this = this * ss2 (ss2 is uint)
 * @param[in, out] self the big num object 
 * @param[in] ss2 ss2 without a sign
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul_uint(big_float_t * self, big_num_strg_t ss2)
{
	big_big_uint_t man_result;
	size_t i;
	big_num_carry_t c = 0;

	if (big_float_is_nan(*self))
		return 1;
	if (big_float_is_zero(*self))
		return 0;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		man_result.table[i] = self->mantissa.table[i];
	for (; i < 2*BIG_NUM_PREC; ++i)
		man_result.table[i] = 0;
	big_num_carry_t man_c = big_uint_mul_int_big(&man_result, ss2);

	big_num_sstrg_t bit = _big_uint_find_leading_bit_in_word(man_result.table[BIG_NUM_PREC]);

	if( bit!=-1 && (big_num_strg_t)(bit) > (BIG_NUM_BITS_PER_UNIT/2) ) {
		// 'i' will be from 0 to BIG_NUM_BITS_PER_UNIT
		i = big_uint_compensation_to_left_big(&man_result);
		c = big_int_add_int(&self->exponent, BIG_NUM_BITS_PER_UNIT - i);

		for(i=0 ; i < BIG_NUM_PREC ; ++i)
			self->mantissa.table[i] = man_result.table[i+1];
	} else {
		if( bit != -1 ) {
			big_uint_rcr_big(&man_result, bit+1, 0);
			c += big_int_add_int(&self->exponent, bit+1);
		}

		for(i=0 ; i < BIG_NUM_PREC ; ++i)
			self->mantissa.table[i] = man_result.table[i];
	}

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * Multiplication this = this * ss2 (ss2 is sint)
 * @param[in, out] self the big num object
 * @param[in] ss2 ss2 with a sign
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul_int(big_float_t * self, big_num_sstrg_t ss2)
{
	if (big_float_is_nan(*self))
		return 1;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}
	if (big_float_is_zero(*self))
		return 0;
	if (big_float_is_sign(*self) == (ss2<0)) {
		// the signs are the same (both are either - or +), the result is positive
		big_float_abs(self);
	} else {
		// the signs are different, the result is negative
		big_float_set_sign(self);
	}

	if (ss2 < 0)
		ss2 = -ss2;

	return big_float_mul_uint(self, (big_num_strg_t)(ss2));
}

/**
 * multiplication this = this * ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num to multiply self by
 * @param[in] round true if result should be rounded
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul(big_float_t * self, big_float_t ss2, bool round)
{
	big_big_uint_t man_result;
	big_num_carry_t c = 0;
	size_t i;

	if( big_float_is_nan(*self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(*self) )
		return 0;

	if( big_float_is_zero(ss2) ) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	big_uint_mul_no_carry(&self->mantissa, ss2.mantissa, &man_result, BIG_NUM_MUL_DEF);

	// 'i' will be from 0 to man*BIG_NUM_BITS_PER_UNIT
	// because mantissa and ss2.mantissa are standardized 
	// (the highest bit in man_result is set to 1 or
	// if there is a zero value in man_result the method CompensationToLeft()
	// returns 0 but we'll correct this at the end in Standardizing() method)
	i = big_uint_compensation_to_left_big(&man_result);
	big_num_sstrg_t exp_add = BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - i;

	if( exp_add )
		c += big_int_add_int(&self->exponent, exp_add );

	c += big_int_add(&self->exponent, ss2.exponent);

	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		self->mantissa.table[i] = man_result.table[i+BIG_NUM_PREC];

	if( round && (man_result.table[BIG_NUM_PREC-1] & BIG_NUM_HIGHEST_BIT) != 0 ) {
		bool is_half = _big_float_check_greater_or_equal_half(self, man_result.table, BIG_NUM_PREC);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(*self) == big_float_is_sign(ss2) ) {
		// the signs are the same, the result is positive
		big_float_abs(self);
	} else {
		// the signs are different, the result is negative
		// if the value is zero it will be corrected later in Standardizing method
		big_float_set_sign(self);
	}

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * division this = this / ss2
 * 
 * return value:
 * -  0 - ok
 * -  1 - carry (in a division carry can be as well)
 * -  2 - improper argument (ss2 is zero)
 * @param[in] self the big num object 
 * @param[in] ss2 the big num object to divide self by
 * @param[in] round true if result should be rounded
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_div(big_float_t * self, big_float_t ss2, bool round)
{
	big_big_uint_t man1;
	big_big_uint_t man2;
	size_t i;
	big_num_carry_t c = 0;
		
	if( big_float_is_nan(*self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( big_float_is_zero(*self) )
		return 0;

	for(i=0 ; i<BIG_NUM_PREC ; ++i) {
		man1.table[i] 				= 0;
		man1.table[i+BIG_NUM_PREC] 	= self->mantissa.table[i];
		man2.table[i]     			= ss2.mantissa.table[i];
		man2.table[i+BIG_NUM_PREC] 	= 0;
	}

	big_big_uint_t remainder;
	big_uint_div_big(&man1, man2, &remainder, BIG_NUM_DIV_DEF);

	i = big_uint_compensation_to_left_big(&man1);

	if( i )
		c += big_int_sub_int(&self->exponent, i);

	c += big_int_sub(&self->exponent, ss2.exponent);
	
	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		self->mantissa.table[i] = man1.table[i+BIG_NUM_PREC];

	if( round && (man1.table[BIG_NUM_PREC-1] & BIG_NUM_HIGHEST_BIT) != 0 )
	{
		bool is_half = _big_float_check_greater_or_equal_half(self, man1.table, BIG_NUM_PREC);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(*self) == big_float_is_sign(ss2) )
		big_float_abs(self);
	else
		big_float_set_sign(self); // if there is a zero it will be corrected in Standardizing()

	c += _big_float_standardizing(self);

	return _big_float_check_carry(self, c);
}

/**
 * caltulate the remainder from a division
 * 
 * samples
 * -   12.6 mod  3 =  0.6   because  12.6 = 3*4 + 0.6
 * -  -12.6 mod  3 = -0.6   bacause -12.6 = 3*(-4) + (-0.6)
 * -   12.6 mod -3 =  0.6
 * -  -12.6 mod -3 = -0.6
 * 
 * in other words: this(old) = ss2 * q + this(new)
 * 
 * return value:
 * -  0 - ok
 * -  1 - carry
 * -  2 - improper argument (ss2 is zero)
 * @param[in] self 
 * @param[in] ss2 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_mod(big_float_t * self, big_float_t ss2)
{
	big_num_carry_t c = 0;

	if( big_float_is_nan(*self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( !_big_float_smaller_without_sign_than(*self, ss2) )
	{
		big_float_t temp = *self;

		c = big_float_div(&temp, ss2, true);
		_big_float_skip_fraction(&temp);
		c += big_float_mul(&temp, ss2, true);
		c += big_float_sub(self, temp, true);

		if( !_big_float_smaller_without_sign_than(*self, ss2 ) )
			c += 1;
	}

	return _big_float_check_carry(self, c);
}

/**
 * this method returns: 'this' mod 2
 * (either zero or one)
 * 
 * this method is much faster than using Mod( object_with_value_two )
 * @param[in] self 
 * @return big_num_strg_t 
 */
big_num_strg_t big_float_mod2(big_float_t self)
{	
	big_int_t zero, negative_bits;
	big_int_set_zero(&zero);
	big_int_init_int(&negative_bits, -(big_num_sstrg_t)(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT));

	if( big_int_cmp_bigger(self.exponent, zero) || big_int_cmp_smaller_equal(self.exponent, negative_bits) )
		return 0;

	big_num_sstrg_t exp_int;
	big_int_to_int(self.exponent, &exp_int);
	// 'exp_int' is negative (or zero), we set it as positive
	exp_int = -exp_int;

	return big_uint_get_bit(self.mantissa, exp_int);
}

/**
 * power this = this ^ pow
 * (pow without a sign)
 * 
 * binary algorithm (r-to-l)
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments (0^0)
 * @param[in] self the big num object
 * @param[in] pow the value to raise the big num object by
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_uint(big_float_t * self, big_uint_t pow)
{
	if( big_float_is_nan(*self) )
		return 1;

	if( big_float_is_zero(*self) ) {
		if( big_uint_is_zero(pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	big_float_t start = *self;
	big_float_t result;
	big_float_set_one(&result);
	big_num_carry_t c = 0;

	while( !c ) {
		if( pow.table[0] & 1 )
			c += big_float_mul(&result, start, true);

		big_uint_rcr(&pow, 1, 0);

		if( big_uint_is_zero(pow) )
			break;

		c += big_float_mul(&start, start, true);
	}

	*self = result;

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * p can be negative
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments 0^0 or 0^(-something)
 * @param[in] self 
 * @param[in] pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_int(big_float_t * self, big_int_t pow)
{
	if( big_float_is_nan(*self) )
		return 1;

	if ( !big_int_is_sign(pow) )
		return big_float_pow_big_uint(self, pow);
	
	if ( big_float_is_zero(*self) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_num_carry_t c = big_int_change_sign(&pow);
	big_float_t t = *self;
	c += big_float_pow_big_uint(&t, pow); // here can only be a carry (return:1)

	big_float_set_one(self);
	c += big_float_div(self, t, true);

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * this must be greater than zero (this > 0)
 * pow can be negative and with fraction
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect argument ('this' <= 0)
 * @param[in] self 
 * @param[in] pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow_big_frac(big_float_t * self, big_float_t pow)
{
	if( big_float_is_nan(*self) || big_float_is_nan(pow) )
		return _big_float_check_carry(self, 1);

	big_float_t temp;
	big_float_set_one(&temp);
	big_num_carry_t c = big_float_ln(&temp, *self);

	if( c != 0 ) { // can be 2 from big_float_ln()
		big_float_set_nan(self);
		return c;
	}

	c += big_float_mul(&temp, pow, true);
	c += big_float_exp(self, temp);

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ pow
 * pow can be negative and with fraction
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect argument ('this' or 'pow')
 * @param[in] self 
 * @param[in] pow 
 * @return big_num_ret_t 
 */
big_num_ret_t big_float_pow(big_float_t * self, big_float_t pow)
{
	if (big_float_is_nan(*self) || big_float_is_nan(pow))
		return _big_float_check_carry(self, 1);

	if (big_float_is_zero(*self)) {
		// 0^pow will be 0 only for pow>0
		if( big_float_is_sign(pow) || big_float_is_zero(pow) ) {
			big_float_set_nan(self);
			return 2;
		}

		big_float_set_zero(self);

		return 0;
	}

	big_int_t zero, negative_bits;
	big_int_set_zero(&zero);
	big_int_init_int(&negative_bits, -(big_num_sstrg_t)(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT));

	if ( big_int_cmp_bigger(pow.exponent, negative_bits) && big_int_cmp_smaller_equal(pow.exponent, zero)) {
		if ( _big_float_is_integer(pow) )
			return _big_float_pow_big_float_int(self, pow);
	}

	return big_float_pow_big_frac(self, pow);
}

/**
 * this function calculates the square root
 * e.g. let this=9 then this.Sqrt() gives 3
 * 
 * return:
 * -  0 - ok
 * -  1 - carry
 * -  2 - improper argument (this<0 or NaN)
 * @param[in] self 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_sqrt(big_float_t * self)
{
	if( big_float_is_nan(*self) || big_float_is_sign(*self)) {
		big_float_set_nan(self);
		return 2;
	}

	if (big_float_is_zero(*self))
		return 0;

	big_float_t old = *self;
	big_float_t ln; // ln get's set in big_float_ln()
	big_num_carry_t c = 0;

	// we're using the formula: sqrt(x) = e ^ (ln(x) / 2)
	c += big_float_ln(&ln, *self);
	c += big_int_sub_int(&ln.exponent, 1); // ln = ln / 2
	c += big_float_exp(self, ln);

	// above formula doesn't give accurate results for some integers
	// e.g. Sqrt(81) would not be 9 but a value very closed to 9
	// we're rounding the result, calculating result*result and comparing
	// with the old value, if they are equal then the result is an integer too


	if ( !c && _big_float_is_integer(old) && !_big_float_is_integer(*self)) {
		big_float_t temp = *self;
		c += big_float_round(&temp);

		big_float_t temp2 = temp;
		c += big_float_mul(&temp, temp2, true);

		if (big_float_cmp_equal(temp, old))
			*self = temp2;
	}

	return _big_float_check_carry(self, c);
}

/**
 * Exponent this = exp(x) = e^x
 * 
 * we're using the fact that our value is stored in form of:
 * 		x = mantissa * 2^exponent
 * then
 * 		e^x = e^(mantissa* 2^exponent) or
 * 		e^x = (e^mantissa)^(2^exponent)
 * 'Exp' returns a carry if we can't count the result ('x' is too big)
 * @param[in] self 
 * @param[in] x 
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_exp(big_float_t * self, big_float_t x)
{
	big_num_carry_t c = 0;
	if ( big_float_is_nan(x) )
		return _big_float_check_carry(self, 1);
	
	if ( big_float_is_zero(x) ) {
		big_float_set_one(self);
		return 0;
	}

	// m will be the value of the mantissa in range (-1,1)
	big_float_t m = x;
	big_int_init_int(&m.exponent, -(big_num_sstrg_t)(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));

	// 'e_' will be the value of '2^exponent'
	//   e_.mantissa.table[man-1] = TTMATH_UINT_HIGHEST_BIT;  and
	//   e_.exponent.Add(1) mean:
	//     e_.mantissa.table[0] = 1;
	//     e_.Standardizing();
	//     e_.exponent.Add(man*TTMATH_BITS_PER_UINT)
	//     (we must add 'man*TTMATH_BITS_PER_UINT' because we've taken it from the mantissa)
	big_float_t e_ = x;
	big_int_set_zero(&e_.mantissa);
	e_.mantissa.table[BIG_NUM_PREC-1] = BIG_NUM_HIGHEST_BIT;
	c += big_int_add_int(&e_.exponent, 1);
	big_float_abs(&e_);

	/*
		now we've got:
		m - the value of the mantissa in range (-1,1)
		e_ - 2^exponent

		e_ can be as:
		...2^-2, 2^-1, 2^0, 2^1, 2^2 ...
		...1/4 , 1/2 , 1  , 2  , 4   ...

		above one e_ is integer

		if e_ is greater than 1 we calculate the exponent as:
			e^(m * e_) = ExpSurrounding0(m) ^ e_
		and if e_ is smaller or equal one we calculate the exponent in this way:
			e^(m * e_) = ExpSurrounding0(m* e_)
		because if e_ is smaller or equal 1 then the product of m*e_ is smaller or equal m
	*/

	big_float_t one;
	big_float_set_one(&one);
	if (big_float_cmp_smaller_equal(e_, one)) {
		big_float_mul(&m, e_, true);
		_big_float_exp_surrounding_0(self, m, NULL);
	} else {
		_big_float_exp_surrounding_0(self, m, NULL);
		c += _big_float_pow_big_float_uint(self, e_);
	}

	return _big_float_check_carry(self, c);
}

/**
 * Natural logarithm this = ln(x)
 * (a logarithm with the base equal 'e')
 * 
 * we're using the fact that our value is stored in form of:
 * 		x = mantissa * 2^exponent
 * then
 * 		ln(x) = ln (mantissa * 2^exponent) = ln (mantissa) + (exponent * ln (2))
 * 
 * the mantissa we'll show as a value from range <1,2) because the logarithm
 * is decreasing too fast when 'x' is going to 0
 * 
 * return values:
 * -  	0 - ok
 * -  	1 - overflow (carry)
 * -  	2 - incorrect argument (x<=0)
 * @param[in] self 
 * @param[in] x 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_ln(big_float_t * self, big_float_t x)
{
	if (big_float_is_nan(x))
		return _big_float_check_carry(self, 1);
	
	if ( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t exponent_temp, mantissa_compensation;
	big_float_init_big_int(&exponent_temp, x.exponent);

	// m will be the value of the mantissa in range <1,2)
	big_float_t m = x;
	big_int_init_int(&m.exponent, -(big_num_sstrg_t)(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT - 1));

	// we must add 'BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT-1' because we've taken it from the mantissa
	big_float_init_int(&mantissa_compensation, BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT-1);
	big_num_carry_t c = big_float_add(&exponent_temp, mantissa_compensation, true);

	_big_float_ln_surrounding_1(self, m, NULL);

	big_float_t ln2;
	big_float_set_ln2(&ln2);
	c += big_float_mul(&exponent_temp, ln2, true);
	c += big_float_add(self, exponent_temp, true);

	return _big_float_check_carry(self, c);
}

/**
 * Logarithm from 'x' with a 'base'
 * we're using the formula:
 * 		Log(x) with 'base' = ln(x) / ln(base)
 * 
 * return values:
 * -  	0 - ok
 * -  	1 - overflow
 * -  	2 - incorrect argument (x<=0)
 * -  	3 - incorrect base (a<=0 or a=1)
 * @param[in] self 
 * @param[in] x 
 * @param[in] base 
 * @return big_num_carry_t 
 */
big_num_ret_t big_float_log(big_float_t * self, big_float_t x, big_float_t base)
{
	if( big_float_is_nan(x) || big_float_is_nan(base) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t denominator;
	big_float_set_one(&denominator);

	if( big_float_is_sign(base) || big_float_is_zero(base) || big_float_cmp_equal(base, denominator) ) {
		big_float_set_nan(self);
		return 3;
	}
	
	if( big_float_cmp_equal(x, denominator) ) { // (this is: if x == 1)
		// log(1) is 0
		big_float_set_zero(self);
		return 0;
	}

	// another error values we've tested at the beginning
	// there can only be a carry
	big_num_carry_t c = big_float_ln(self, x);

	c += big_float_ln(&denominator, base);
	c += big_float_div(self, denominator, true);

	return _big_float_check_carry(self, c);
}

/**
 * float constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_float(big_float_t * self, float value)
{
	big_float_init_double(self, value);
}

/**
 * double constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_double(big_float_t * self, double value)
{
	union 
	{
		double d;
		big_num_strg_t u[2]; // two 32bit words
	} temp;

	temp.d = value;

	big_num_sstrg_t e = ( temp.u[1] & 0x7FF00000u) >> 20;
	big_num_strg_t m1 = ((temp.u[1] &    0xFFFFFu) << 11) | (temp.u[0] >> 21);
	big_num_strg_t m2 = temp.u[0] << 11;
	
	if( e == 2047 ) {
		// If E=2047 and F is nonzero, then V=NaN ("Not a number")
		// If E=2047 and F is zero and S is 1, then V=-Infinity
		// If E=2047 and F is zero and S is 0, then V=Infinity

		// we do not support -Infinity and +Infinity
		// we assume that there is always NaN 

		big_float_set_nan(self);
	}
	else if( e > 0 ) {
		// If 0<E<2047 then
		// V=(-1)**S * 2 ** (E-1023) * (1.F)
		// where "1.F" is intended to represent the binary number
		// created by prefixing F with an implicit leading 1 and a binary point.
		
		_big_float_init_double_set_exp_and_man(self,
			(temp.u[1] & 0x80000000u) != 0,
			e - 1023 - BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT + 1, 0x80000000u,
			m1, m2);

		// we do not have to call Standardizing() here
		// because the mantissa will have the highest bit set
	} else {
		// e == 0

		if( m1 != 0 || m2 != 0 ) {
			// If E=0 and F is nonzero,
			// then V=(-1)**S * 2 ** (-1022) * (0.F)
			// These are "unnormalized" values.

			big_num_strg_t m[2];
			m[1] = m1;
			m[0] = m2;
			// size_t moved = big_uint_compensation_to_left(m);
			size_t moved;
			{
				size_t moving = 0;

				// a - index a last word which is different from zero
				ssize_t a;
				for(a=1 ; a>=0 && m[a]==0 ; --a);

				if( a < 0 ) {
					moved = moving;
					// return moving; // all words in table have zero
				} else {
					if( a != 1 ) {
						moving += ( 1 - a ) * BIG_NUM_BITS_PER_UNIT;

						// moving all words
						ssize_t i;
						for(i=2-1 ; a>=0 ; --i, --a)
							m[i] = m[a];

						// setting the rest word to zero
						for(; i>=0 ; --i)
							m[i] = 0;
					}

					size_t moving2 = _big_uint_find_leading_bit_in_word( m[1] );
					// moving2 is different from -1 because the value table[2-1]
					// is not zero

					moving2 = BIG_NUM_BITS_PER_UNIT - moving2 - 1;
					// big_uint_rcl(m, moving2, 0);
					{
						size_t bits				= moving2;
						big_num_strg_t last_c   = 0;
						size_t rest_bits 		= bits;

						if( bits == 0 ) {
							// return 0;
						} else {
							if( bits >= BIG_NUM_BITS_PER_UNIT ) {
								// _big_uint_rcl_move_all_words(m, &rest_bits, &last_c, bits, 0);
								{
									rest_bits      		= bits % BIG_NUM_BITS_PER_UNIT;
									size_t all_words 	= bits / BIG_NUM_BITS_PER_UNIT;
									big_num_strg_t mask = 0;


									if( all_words >= 2 ) {
										if( all_words == 2 && rest_bits == 0 )
											last_c = m[0] & 1;
										// else: last_c is default set to 0

										// clearing
										for(size_t i = 0 ; i<2 ; ++i)
											m[i] = mask;

										rest_bits = 0;
									}
									else
									if( all_words > 0 ) {
										// 0 < all_words < 2
										ssize_t first, second;
										last_c = m[2 - all_words] & 1; // all_words is greater than 0

										// copying the first part of the value
										for(first = 1, second=first-all_words ; second>=0 ; --first, --second)
											m[first] = m[second];

										// setting the rest to '0'
										for( ; first>=0 ; --first )
											m[first] = mask;
									}
								}
							}

							if( rest_bits == 0 ) {
								// return last_c;
							} else {

								// rest_bits is from 1 to BIG_NUM_BITS_PER_UNIT-1 now
								if( rest_bits == 1 ) {
									// last_c = _big_uint_rcl2_one(m, 0);
									{	
										big_num_carry_t c = 0;
										size_t i;
										big_num_carry_t new_c;

										for(i=0 ; i<2 ; ++i) {
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
											m[i] 			= (m[i] << 1) | c;
											c        		= new_c;
										}

										last_c = c;
										// return c;
									}
								} else if( rest_bits == 2 ) {
									// performance tests showed that for rest_bits==2 it's better to use _big_uint_rcl2_one twice instead of _big_uint_rcl2(2,c)
									// _big_uint_rcl2_one(m, 0);
									{
										// big_num_carry_t c = 0;
										size_t i;
										big_num_carry_t new_c;

										for(i=0 ; i<2 ; ++i) {
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
											m[i] 			= (m[i] << 1); // | c
											// c        		= new_c;
										}

										// last_c = c;
									}
									// last_c = _big_uint_rcl2_one(m, 0);
									{
										big_num_carry_t c = 0;
										size_t i;
										big_num_carry_t new_c;

										if( c != 0 )
											c = 1;

										for(i=0 ; i<2 ; ++i) {
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
											m[i] 			= (m[i] << 1) | c;
											c        		= new_c;
										}

										last_c = c;
									}
								}
								else {
									// last_c = _big_uint_rcl2(m, rest_bits, 0);
									{
										big_num_carry_t c = 0;
										size_t move = BIG_NUM_BITS_PER_UNIT - rest_bits;
										size_t i;
										big_num_carry_t new_c;

										if( c != 0 )
											c = BIG_NUM_MAX_VALUE >> move;

										for(i=0 ; i<2 ; ++i) {
											new_c    		= m[i] >> move;
											m[i] 			= (m[i] << rest_bits) | c;
											c        		= new_c;
										}

										last_c = c & 1;
										// return (c & 1);
									}
								}
								
								// return last_c;
							}
						}
					}

					moved = moving + moving2;
					// return moving + moving2;
				}
			}

			_big_float_init_double_set_exp_and_man(self,
				(temp.u[1] & 0x80000000u) != 0,
				e - 1022 - BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT + 1 - moved, 0,
				m[1], m[0]);
		} else {
			// If E=0 and F is zero and S is 1, then V=-0
			// If E=0 and F is zero and S is 0, then V=0 

			// we do not support -0 or 0, only is one 0
			big_float_set_zero(self);
		}
	}

	return; // never be a carry
}

/**
 * uint constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_uint(big_float_t * self, big_num_strg_t value)
{
	if (value == 0) {
		big_float_set_zero(self);
		return;
	}

	self->info = 0;

	for (size_t i = 0; i < BIG_NUM_PREC-1; ++i)
		self->mantissa.table[i] = 0;
	self->mantissa.table[BIG_NUM_PREC-1] = value;
	big_int_init_int(&self->exponent, -(big_num_sstrg_t)(BIG_NUM_PREC-1) * (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT));

	// there shouldn't be a carry because 'value' has the 'uint' type 
	_big_float_standardizing(self);
}

/**
 * int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_int(big_float_t * self, big_num_sstrg_t value)
{
	bool is_sign = false;
	if (value < 0) {
		value = -value;
		is_sign = true;
	}

	big_float_init_uint(self, (big_num_strg_t)value);

	if (is_sign)
		big_float_set_sign(self);
}

/**
 * big float constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_float(big_float_t * self, big_float_t value)
{
	*self = value;
}

/**
 * big uint constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_uint(big_float_t * self, big_uint_t value)
{
	self->info = 0;
	big_num_sstrg_t compensation = (big_num_sstrg_t)big_uint_compensation_to_left(&value);
	_big_float_init_uint_or_int(self, value, compensation);
}

/**
 * big int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_int(big_float_t * self, big_int_t value)
{
	self->info = 0;
	bool is_sign = false;
	if (big_int_is_sign(value)) {
		big_int_change_sign(&value);
		is_sign = true;
	}

	big_num_sstrg_t compensation = (big_num_sstrg_t)big_int_compensation_to_left(&value);
	_big_float_init_uint_or_int(self, value, compensation);

	if (is_sign)
		big_float_set_sign(self);
}

/**
 * double converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_double(big_float_t self, double * result)
{
	if ( big_float_is_zero(self) ) {
		*result = 0.0;
		return 0;
	}

	if ( big_float_is_nan(self) ) {
		*result = _big_float_to_double_set_double(self, false, 2047, 0, false, true);
		return 0;
	}

	big_num_sstrg_t e_correction = (big_num_sstrg_t)(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT) - 1;
	big_int_t _e_correction;
	big_int_init_int(&_e_correction, 1024 - e_correction);
	if (big_int_cmp_bigger_equal(self.exponent, _e_correction) ) {
		// +/- infinity
		*result = _big_float_to_double_set_double(self, big_float_is_sign(self), 2047, 0, true, false);
		return 1;
	} else {
		big_int_init_int(&_e_correction, -1023 -52 - e_correction);
		if (big_int_cmp_smaller_equal(self.exponent, _e_correction)) {
			// too small value - we assume that there'll be a zero and return a carry
			*result = 0.0;
			return 1;
		}
	}

	big_num_sstrg_t e;
	big_int_to_int(self.exponent, &e);
	e += e_correction;

	if ( e <= -1023 ) {
		// -1023-52 < e <= -1023  (unnormalized value)
		*result = _big_float_to_double_set_double(self, big_float_is_sign(self), 0, -(e + 1023), false, false);
	} else {
		// -1023 < e < 1024
		*result = _big_float_to_double_set_double(self, big_float_is_sign(self), (e + 1023), -1, false, false);
	}
}

/**
 * float converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_float(big_float_t self, float * result)
{
	double result_double;
	big_num_carry_t c = big_float_to_double(self, &result_double);
	*result = (float)result_double;

	if (*result == -0.0f)
		*result = 0.0f;
	
	if (c)
		return 1;

	// although the result_double can have a correct value
	// but after converting to float there can be infinity
	
	// check for positive infinity
	if ( *result == (float)(1.0 / 0.0) )
		return 1;

	// check for negative infinity
	if ( *result == (float)(-1.0 / 0.0) )
		return 1;
	
	if (*result == 0.0f && result_double != 0.0)
		return 1;
	
	return 0;
}

/**
 * uint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_uint(big_float_t self, big_num_strg_t * result)
{
	if ( _big_float_to_uint_or_int(self, result) )
		return 1;
	if ( big_float_is_sign(self) )
		return 1;
	return 0;
}

/**
 * int converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_int(big_float_t self, big_num_sstrg_t * result)
{
	big_num_strg_t result_uint;
	big_num_carry_t c = _big_float_to_uint_or_int(self, &result_uint);
	*result = (big_num_sstrg_t)(result_uint);

	if ( c )
		return 1;
	
	big_num_strg_t mask = 0;
	if (big_float_is_sign(self)) {
		mask = BIG_NUM_MAX_VALUE;
		*result = -1 * (*result);
	}

	return ((*result & BIG_NUM_HIGHEST_BIT) == (mask & BIG_NUM_HIGHEST_BIT)) ? 0 : 1;
}

/**
 * this method returns true if 'self' is smaller than 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_smaller(big_float_t self, big_float_t l)
{
	if (big_float_is_sign(self) && !big_float_is_sign(l)) {
		// self < 0 and l >= 0
		return true;
	}

	if (!big_float_is_sign(self) && big_float_is_sign(l)) {
		// self >= 0 and l < 0
		return false;
	}

	// both signs are the same
	if ( big_float_is_sign(self) )
		return _big_float_smaller_without_sign_than(l, self);
	
	return _big_float_smaller_without_sign_than(self, l);
}

/**
 * this method returns true if 'self' is bigger than 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_bigger(big_float_t self, big_float_t l)
{
	if (big_float_is_sign(self) && !big_float_is_sign(l)) {
		// self < 0 and l >= 0
		return false;
	}

	if (!big_float_is_sign(self) && big_float_is_sign(l)) {
		// self >= 0 and l < 0
		return true;
	}

	// both signs are the same
	if ( big_float_is_sign(self) )
		return _big_float_greater_without_sign_than(l, self);
	
	return _big_float_greater_without_sign_than(self, l);
}

/**
 * this method returns true if 'self' is equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_equal(big_float_t self, big_float_t l)
{
	if ( big_float_is_sign(self) != big_float_is_sign(l) )
		return false;
	
	return _big_float_equal_without_sign(self, l);
}

/**
 * this method returns true if 'self' is smaller or equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_smaller_equal(big_float_t self, big_float_t l)
{
	return !big_float_cmp_bigger(self, l);
}

/**
 * this method returns true if 'self' is bigger or equal to 'l'
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_float_cmp_bigger_equal(big_float_t self, big_float_t l)
{
	return !big_float_cmp_smaller(self, l);
}

void big_float_print(big_float_t self) {
	double val;
	big_float_to_double(self, &val);
	printf("%lf", val);
}



/**
 * this method sets NaN if there was a carry (and returns 1 in such a case)
 * @param[in, out] self the big num object
 * @param[in] c can be 0, 1 or other value different from zero
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_float_check_carry(big_float_t * self, big_num_carry_t c)
{
	if ( c != 0) {
		big_float_set_nan(self);
		return 1;
	}
	return 0;
}

/**
 * this method moves all bits from mantissa into its left side
 * (suitably changes the exponent) or if the mantissa is zero
 * it sets the exponent to zero as well
 * (and clears the sign bit and sets the zero bit)
 * 
 * it can return a carry
 * the carry will be when we don't have enough space in the exponent
 * 
 * you don't have to use this method if you don't change the mantissa
 * and exponent directly
 * @param[in, out] self the big num object
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_float_standardizing(big_float_t * self)
{
	if ( big_uint_is_the_highest_bit_set(self->mantissa) ) {
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_ZERO);
		return 0;
	}

	if ( _big_float_correct_zero(self) )
		return 0;
	
	big_int_t comp;
	big_int_init_int(&comp, (big_num_sstrg_t)big_uint_compensation_to_left(&self->mantissa));
	return big_int_sub(&self->exponent, comp);
}

/**
 * if the mantissa is equal zero this method sets exponent to zero and
 * info without the sign
 * @param[in, out] self the big num object
 * @return true there was a correction
 * @return false there was no correction
 */
static bool	_big_float_correct_zero(big_float_t * self)
{
	if (big_uint_is_zero(self->mantissa)) {
		_big_float_set_info_bit(self, BIG_FLOAT_INFO_ZERO);
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);
		big_int_set_zero(&self->exponent);

		return true;
	} else {
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_ZERO);
	}
}

/**
 * this method clears a specific bit in the 'info' variable
 * @param[in, out] self the big num object
 * @param[in] bit the bit to clear
 */
static void	_big_float_clear_info_bit(big_float_t * self, big_float_info_t bit)
{
	self->info = self->info & (~bit);
}

/**
 * this method sets a specific bit in the 'info' variable
 * @param[in, out] self the big num object
 * @param[in] bit the bit to set
 */
static void	_big_float_set_info_bit(big_float_t * self, big_float_info_t bit)
{
	self->info = self->info | (bit);
}

/**
 * this method returns true if a specific bit in the 'info' variable is set
 * @param[in, out] self the big num object
 * @param[in] bit the bit to check
 * @return true bit was set
 * @return false bit was not set
 */
static bool	_big_float_is_info_bit(big_float_t self, big_float_info_t bit)
{
	return (self.info & bit) != 0;
}

/**
 * this method does the half-to-even rounding (banker's rounding)
 * 
 * if is_half is:
 * -  true  - that means the rest was equal the half (0.5 decimal)
 * -  false - that means the rest was greater than a half (greater than 0.5 decimal)
 * 
 * if the rest was less than a half then don't call this method
 * (the rounding should does nothing then)
 * @param[in, out] self the big num object
 * @param[in] is_half 
 * @param[in] rounding_up 
 * @return big_num_carry_t 
 */
big_num_carry_t _big_float_round_half_to_even(big_float_t * self, bool is_half, bool rounding_up)
{
	big_num_carry_t c = 0;

	if( !is_half || big_uint_is_the_lowest_bit_set(self->mantissa) )
	{
		if( rounding_up )
		{
			if( big_uint_add_uint(&self->mantissa, 1) )
			{
				big_uint_rcr(&self->mantissa, 1, 1);
				c = big_int_add_int(&self->exponent, 1);
			}
		} else {
			big_uint_sub_uint(&self->mantissa, 1);

			// we're using rounding_up=false in Add() when the mantissas have different signs
			// mantissa can be zero only when previous mantissa was equal to ss2.mantissa
			// but in such a case 'last_bit_set' will not be set and consequently 'do_rounding' will be false
		}
	}

	return c;
}


/**
 * Auxiliary method for adding
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the other big num object
 * @param[in] exp_offset 
 * @param[out] last_bit_set 
 * @param[out] rest_zero 
 * @param[out] do_adding 
 * @param[out] do_rounding 
 */
static void _big_float_add_check_exponents(big_float_t* ss2, big_int_t exp_offset, bool * last_bit_set, bool * rest_zero, bool * do_adding, bool * do_rounding)
{
	big_int_t mantissa_size_in_bits;
	big_int_init_uint(&mantissa_size_in_bits, BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT);

	if (big_int_cmp_equal(exp_offset, mantissa_size_in_bits)) {
		*last_bit_set = big_uint_is_the_highest_bit_set(ss2->mantissa);
		*rest_zero = big_uint_are_first_bits_zero(ss2->mantissa, BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - 1);
		*do_rounding = true;
	} else if (big_int_cmp_smaller(exp_offset, mantissa_size_in_bits)) {
		big_num_strg_t moved;
		big_int_to_int(exp_offset, &moved); // how many times we must move ss2.mantissa
		*rest_zero = true;

		if ( moved > 0 ) {
			*last_bit_set = (bool)(big_uint_get_bit(ss2->mantissa, moved-1) );

			if (moved > 1)
				*rest_zero = big_uint_are_first_bits_zero(ss2->mantissa, moved-1);
			
			// (2) moving 'exp_offset' times
			big_uint_rcr(&ss2->mantissa, moved, 0);
		}

		*do_adding = true;
		*do_rounding = true;

		// if exp_offset is greater than mantissa_size_in_bits then we do nothing
		// ss2 is too small for taking into consideration in the sum
	}
}

/**
 * an auxiliary method for adding
 * @param[in] self the big num object
 * @param[in] ss2 the other big num object
 * @param[in] last_bit_set 
 * @param[in] rest_zero 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_float_add_mantissas(big_float_t* self, big_float_t* ss2, bool * last_bit_set, bool * rest_zero)
{
	big_num_carry_t c = 0;
	if (big_float_is_sign(*self) == big_float_is_sign(*ss2)) {
		// values have the same signs
		if (big_uint_add(&self->mantissa, ss2->mantissa, 0)) {
			// we have one bit more from addition (carry)
			// now rest_zero means the old rest_zero with the old last_bit_set
			*rest_zero    = (!last_bit_set && rest_zero);
			*last_bit_set = big_uint_rcr(&self->mantissa, 1,1);
			c += big_int_add_int(&self->exponent, 1);
		}
	} else {
		// values have different signs
		// there shouldn't be a carry here because
		// (1) (2) guarantee that the mantissa of this
		// is greater than or equal to the mantissa of the ss2

		big_uint_sub(&self->mantissa, ss2->mantissa, 0);
	}

	return c;
}

/**
 * Subtraction this = this - ss2
 * @param[in] self the big num object 
 * @param[in] ss2 the big num object to add
 * @param[in] round set to true to round
 * @param[in] adding set to true if adding, false if subtracting
 * @return big_num_carry_t carry if the result is too big
 */
big_num_carry_t _big_float_add(big_float_t * self, big_float_t ss2, bool round, bool adding)
{
	bool last_bit_set, rest_zero, do_adding, do_rounding, rounding_up;
	big_num_carry_t c = 0;

	if( big_float_is_nan(*self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( !adding )
		big_float_change_sign(&ss2); // subtracting

	// (1) abs(this) will be >= abs(ss2)
	if( _big_float_smaller_without_sign_than(*self, ss2) )
		big_float_swap(self, &ss2);

	if( big_float_is_zero(ss2) )
		return 0;

	big_int_t exp_offset = self->exponent;
	big_int_sub(&exp_offset, ss2.exponent);

	if( !big_int_abs(&exp_offset) ) {
		// if there is a carry in Abs it means the value in exp_offset has only the lowest bit set
		// so the value is the smallest possible integer
		// and its Abs would be greater than mantissa size in bits
		// so the method AddCheckExponents would do nothing


		last_bit_set = rest_zero = do_adding = do_rounding = false;
		rounding_up = (big_float_is_sign(*self) == big_float_is_sign(ss2));

		_big_float_add_check_exponents(&ss2, exp_offset, &last_bit_set, &rest_zero, &do_adding, &do_rounding);
		
		if( do_adding )
			c += _big_float_add_mantissas(self, &ss2, &last_bit_set, &rest_zero);

		if( !round || !last_bit_set )
			do_rounding = false;

		if( do_rounding )
			c += _big_float_round_half_to_even(self, rest_zero, rounding_up);

		if( do_adding || do_rounding )
			c += _big_float_standardizing(self);
	}

	return _big_float_check_carry(self, c);
}

/**
 * this method checks whether a table pointed by 'tab' and 'len'
 * has the value 0.5 decimal
 * (it is treated as the comma operator would be before the highest bit)
 * call this method only if the highest bit is set - you have to test it beforehand
 * @param[in] self 
 * @param[in] tab 
 * @param[in] len 
 * @return true tab was equal the half (0.5 decimal)
 * @return false tab was greater than a half (greater than 0.5 decimal)
 */
bool _big_float_check_greater_or_equal_half(big_float_t * self, big_num_strg_t * tab, big_num_strg_t len)
{
	size_t i;

	for(i=0 ; i<len-1 ; ++i)
		if( tab[i] != 0 )
			return false;

	if( tab[i] != BIG_NUM_HIGHEST_BIT )
		return false;

	return true;
}

/**
 * power this = this ^ pow
 * (pow without a sign)
 * 
 * binary algorithm (r-to-l)
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments (0^0)
 * @param[in] self the big num object
 * @param[in] pow the value to raise the big num object by
 * @return big_num_ret_t 
 */
big_num_ret_t _big_float_pow_big_float_uint(big_float_t * self, big_float_t pow)
{
	if( big_float_is_nan(*self) || big_float_is_nan(pow) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(*self) ) {
		if( big_float_is_zero(pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	if (big_float_is_sign(pow))
		big_float_abs(&pow);

	big_float_t start = *self;
	big_float_t result;
	big_float_t one;
	big_num_carry_t c = 0;
	big_float_set_one(&one);
	result = one;

	while( !c ) {
		if( big_float_mod2(pow) )
			c += big_float_mul(&result, start, true);

		c += big_int_sub_int(&pow.exponent, 1);

		if( big_float_cmp_smaller(pow, one) )
			break;

		c += big_float_mul(&start, start, true);
	}

	*self = result;

	return _big_float_check_carry(self, c);
}

/**
 * power this = this ^ [pow]
 * pow is treated as a value without a fraction
 * pow can be negative
 * 
 * return values:
 * -  0 - ok
 * -  1 - carry
 * -  2 - incorrect arguments 0^0 or 0^(-something)
 * @param[in] self 
 * @param[in] pow 
 * @return big_num_ret_t 
 */
big_num_ret_t _big_float_pow_big_float_int(big_float_t * self, big_float_t pow)
{
	if( big_float_is_nan(*self) )
		return 1;

	if ( !big_float_is_sign(pow) )
		return _big_float_pow_big_float_uint(self, pow);
	
	if ( big_float_is_zero(*self) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_float_t temp = *self;
	big_num_carry_t c = _big_float_pow_big_float_uint(&temp, pow); // here can only be a carry (return:1)

	big_float_set_one(self);
	c += big_float_div(self, temp, true);

	return _big_float_check_carry(self, c);
}

/**
 * Exponent this = exp(x) = e^x where x is in (-1,1)
 * we're using the formula:
 * exp(x) = 1 + (x)/(1!) + (x^2)/(2!) + (x^3)/(3!) + ...
 * @param[in, out] self the big num object
 * @param[in] x 
 * @param[out] steps 
 */
static void _big_float_exp_surrounding_0(big_float_t * self, big_float_t x, size_t * steps)
{
	big_float_t denominator, denominator_i;
	big_float_t one, old_value, next_part;
	big_float_t numerator = x;

	big_float_set_one(&one);
	*self = one;
	denominator = one;
	denominator_i = one;

	size_t i;
	old_value = *self;
	for (i = 1 ; i <= BIG_NUM_ARITHMETIC_MAX_LOOP ; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'
		next_part = numerator;
		if (big_float_div(&next_part, denominator, true))
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if ( big_float_cmp_equal(old_value, *self) )
				// we've added next few parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = *self;
		}

		// we set the denominator and the numerator for a next part of the formula
		if (big_float_add(&denominator_i, one, true))
			// if there is a carry here the result we return as good
			break;

		if (big_float_mul(&denominator, denominator_i, true))
			break;
		
		if (big_float_mul(&numerator, x, true))
			break;
	}

	if (steps)
		*steps = i;
}

/**
 * Natural logarithm this = ln(x) where x in range <1,2)
 * we're using the formula:
 * ln(x) = 2 * [ (x-1)/(x+1) + (1/3)((x-1)/(x+1))^3 + (1/5)((x-1)/(x+1))^5 + ... ]
 * @param[in, out] self the big num object
 * @param[in] x 
 * @param[out] steps 
 */
static void _big_float_ln_surrounding_1(big_float_t * self, big_float_t x, size_t * steps)
{
	big_float_t old_value, next_part, denominator, one, two, x1, x2;
	x1 = x;
	x2 = x;
	big_float_set_one(&one);

	if ( big_float_cmp_equal(x, one) ) {
		// LnSurrounding1(1) is 0
		big_float_set_zero(self);
		return;
	}

	big_float_init_uint(&two, 2);

	big_float_sub(&x1, one, true);
	big_float_add(&x2, one, true);

	big_float_div(&x1, x2, true);
	x2 = x1;
	big_float_mul(&x2, x1, true);

	denominator = one;
	big_float_set_zero(self);

	old_value = *self;
	size_t i;

	for (i = 1; i <= BIG_NUM_ARITHMETIC_MAX_LOOP; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'

		next_part = x1;

		if (big_float_div(&next_part, denominator, true))
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if (big_float_cmp_equal(old_value, *self))
				// we've added next (step_test) parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = *self;
		}

		if (big_float_mul(&x1, x2, true) )
			// if there is a carry here the result we return as good
			break;
		
		if (big_float_add(&denominator, one, true))
			break;
	}

	// this = this * 2
	// ( there can't be a carry here because we calculate the logarithm between <1,2) )
	big_int_add_int(&self->exponent, 1);

	if (steps)
		*steps = i;
}

/**
 * an auxiliary method for converting from UInt and Int
 * we assume that there'll never be a carry here
 * (we have an exponent and the value in Big can be bigger than
 * that one from the UInt)
 * @param[in] self the big num object
 * @param[in] value the big num object to set self to
 * @param[in] compensation 
 */
static void _big_float_init_uint_or_int(big_float_t * self, big_uint_t value, big_num_sstrg_t compensation)
{
	big_int_init_int(&self->exponent, - compensation);

	// copying the highest words
	size_t i;
	for(i=1 ; i<=BIG_NUM_PREC ; ++i)
		self->mantissa.table[BIG_NUM_PREC-i] = value.table[BIG_NUM_PREC-i];

	// the highest bit is either one or zero (when the whole mantissa is zero)
	// we can only call CorrectZero()
	_big_float_correct_zero(self);
}

/**
 * an auxiliary method for converting 'this' into 'result'
 * if the value is too big this method returns a carry (1)
 * @param[in] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_float_to_uint_or_int(big_float_t self, big_num_strg_t * result)
{
	*result = 0;

	if ( big_float_is_zero(self) )
		return 0;

	big_num_sstrg_t max_bit = -(big_num_sstrg_t)(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT);
	big_int_t _max_bit;
	big_int_init_int(&_max_bit, max_bit + BIG_NUM_BITS_PER_UNIT);

	if ( big_int_cmp_bigger(self.exponent, _max_bit) )
		// if exponent > (maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT)) the value can't be passed
		// into the 'big_num_sstrg_t' type (it's too big)
		return 1;
	
	big_int_init_int(&_max_bit, max_bit);

	if ( big_int_cmp_smaller_equal(self.exponent, _max_bit) )
		// our value is from the range of (-1,1) and we return zero
		return 0;

	// exponent is from a range of (maxbit, maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT) >
	// and [maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT] <= 0
	big_num_sstrg_t how_many_bits;
	big_int_to_int(self.exponent, &how_many_bits);

	// how_many_bits is negative, we'll make it positive
	how_many_bits = -how_many_bits;

	*result = (self.mantissa.table[BIG_NUM_PREC-1] >> (how_many_bits % BIG_NUM_BITS_PER_UNIT));

	return 0;
}

/**
 * 
 * @param[in] self the big num object
 * @param[in] is_sign 
 * @param[in] e 
 * @param[in] mhighest 
 * @param[in] m1 
 * @param[in] m2 
 */
static void _big_float_init_double_set_exp_and_man(big_float_t * self, bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2)
{
	self->exponent;
	big_int_init_int(&self->exponent, e);

	if( BIG_NUM_PREC > 1 ) {
		self->mantissa.table[BIG_NUM_PREC-1] = m1 | mhighest;
		self->mantissa.table[(big_num_sstrg_t)(BIG_NUM_PREC-2)] = m2;
		// although man>1 we're using casting into sint
		// to get rid from a warning which generates Microsoft Visual:
		// warning C4307: '*' : integral constant overflow

		for(size_t i=0 ; i<BIG_NUM_PREC-2 ; ++i)
			self->mantissa.table[i] = 0;
	} else {
		self->mantissa.table[0] = m1 | mhighest;
	}

	self->info = 0;

	// the value should be different from zero

	if( is_sign )
		big_float_set_sign(self);
}

/**
 * 
 * @param[in] self the big num object
 * @param[in] is_sign 
 * @param[in] e 
 * @param[in] move 
 * @param[in] infinity 
 * @param[in] nan 
 * @return double 
 */
static double _big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan)
{
	union {
		double d;
		big_num_strg_t u[2]; // two 32bit words
	} temp;
	
	temp.u[0] = temp.u[1] = 0;

	if (is_sign)
		temp.u[1] |= 0x80000000u;

	temp.u[1] |= (e << 20) & 0x7FF00000u;

	if ( nan ) {
		temp.u[0] |= 1;
		return temp.d;
	}

	if ( infinity )
		return temp.d;
	
	big_num_strg_t m[2];
	m[1] = self.mantissa.table[BIG_NUM_PREC-1];
	m[0] = (BIG_NUM_PREC > 1) ? self.mantissa.table[BIG_NUM_PREC-2] : 0;
	
	// big_uint_rcr(m, 12 + move, 0)
	{
		size_t bits 			 = (12 + move);
		big_num_carry_t last_c   = 0;
		size_t rest_bits		 = bits;
		
		if( bits == 0 ) {
			// return 0;
		} else {
			if( bits >= BIG_NUM_BITS_PER_UNIT ) {
				// _big_uint_rcr_move_all_words(&m, &rest_bits, &last_c, bits, 0);
				{
					rest_bits      		= bits % BIG_NUM_BITS_PER_UNIT;
					size_t all_words 	= bits / BIG_NUM_BITS_PER_UNIT;
					big_num_strg_t mask = 0;


					if( all_words >= 2 ) {
						if( all_words == 2 && rest_bits == 0 )
							last_c = (m[1] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
						// else: last_c is default set to 0

						// clearing
						for(size_t i = 0 ; i<2 ; ++i)
							m[i] = mask;

						rest_bits = 0;
					} else if( all_words > 0 ) {
						// 0 < all_words < 2

						ssize_t first, second;
						last_c = (m[all_words - 1] & BIG_NUM_HIGHEST_BIT) ? 1 : 0; // all_words is > 0

						// copying the first part of the value
						for(first=0, second=all_words ; second<2 ; ++first, ++second)
							m[first] = m[second];

						// setting the rest to '0'
						for( ; first<2 ; ++first )
							m[first] = mask;
					}
				}
			}

			if( rest_bits == 0 ) {
				// return last_c;
			} else {
				// rest_bits is from 1 to BIG_NUM_BITS_PER_UNIT-1 now
				if( rest_bits == 1 ) {
					// last_c = _big_uint_rcr2_one(&m, 0);
					{
						big_num_carry_t c = 0;
						big_num_sstrg_t i; // signed i
						big_num_carry_t new_c;

						for(i=(big_num_sstrg_t)2-1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) ? BIG_NUM_HIGHEST_BIT : 0;
							m[i] 			= (m[i] >> 1) | c;
							c        		= new_c;
						}

						c = (c != 0) ? 1 : 0;

						// return c;
						last_c = c;
					}
				} else if( rest_bits == 2 ) {
					// performance tests showed that for rest_bits==2 it's better to use Rcr2_one twice instead of Rcr2(2,c)
					// _big_uint_rcr2_one(&m, 0);
					{
						// big_num_carry_t c = 0;
						big_num_sstrg_t i; // signed i
						big_num_carry_t new_c;

						for(i=(big_num_sstrg_t)2-1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) ? BIG_NUM_HIGHEST_BIT : 0;
							m[i] 			= (m[i] >> 1); // | c;
							// c				= new_c;
						}

						// c = (c != 0) ? 1 : 0;

						// return c;
					}
					// last_c = _big_uint_rcr2_one(&m, 0);
					{
						big_num_carry_t c = 0;
						big_num_sstrg_t i; // signed i
						big_num_carry_t new_c;

						for(i=(big_num_sstrg_t)2-1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) ? BIG_NUM_HIGHEST_BIT : 0;
							m[i] 			= (m[i] >> 1) | c;
							c        		= new_c;
						}

						c = (c != 0) ? 1 : 0;

						// return c;
						last_c = c;
					}
				} else {
					// last_c = _big_uint_rcr2(&m, rest_bits, 0);
					{
						size_t bits			= rest_bits;
						big_num_carry_t c 	= 0;
						size_t move			= BIG_NUM_BITS_PER_UNIT - bits;
						ssize_t i; // signed
						big_num_carry_t new_c;

						if( c != 0 )
							c = BIG_NUM_MAX_VALUE << move;

						for(i=1 ; i>=0 ; --i) {
							new_c    		= m[i] << move;
							m[i] 			= (m[i] >> bits) | c;
							c        		= new_c;
						}

						c = (c & BIG_NUM_HIGHEST_BIT) ? 1 : 0;

						// return c;
						last_c = c;
					}
				}

				// return last_c;
			}
		}
	}

	m[1] &= 0xFFFFFu; // cutting the 20 bit (when 'move' was -1)
	temp.u[1] |= m[1];
	temp.u[0] |= m[0];

	return temp.d;
}

/**
 * this method performs the formula 'abs(this) < abs(ss2)'
 * and returns the result
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
static bool _big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return false; // got 2 zeroes
		else
			return true; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return false; // self != 0 & ss2 == 0
	}

	// we're using the fact that all bits in mantissa are pushed
	// into the left side -- Standardizing()
	if( big_int_cmp_equal(self.exponent, ss2.exponent) )
		return big_uint_cmp_smaller(self.mantissa, ss2.mantissa, -1);

	return big_int_cmp_smaller(self.exponent, ss2.exponent);
}

/**
 * this method performs the formula 'abs(this) > abs(ss2)'
 * and returns the result
 * 
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
static bool	_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return false; // got 2 zeroes
		else
			return false; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return true; // self != 0 & ss2 == 0
	}

	// we're using the fact that all bits in mantissa are pushed
	// into the left side -- Standardizing()
	if( big_int_cmp_equal(self.exponent, ss2.exponent) )
		return big_uint_cmp_bigger(self.mantissa, ss2.mantissa, -1);

	return big_int_cmp_bigger(self.exponent, ss2.exponent);
}

/**
 * this method performs the formula 'abs(this) == abs(ss2)'
 * and returns the result
 * 
 * (in other words it treats 'this' and 'ss2' as values without a sign)
 * we don't check the NaN flag
 * @param[in] self the big num object
 * @param[in] ss2 the big num object to compare to
 * @return true 
 * @return false 
 */
static bool	_big_float_equal_without_sign(big_float_t self, big_float_t ss2)
{
	if (big_float_is_zero(self)) {
		if (big_float_is_zero(ss2))
			return true; // got 2 zeroes
		else
			return false; // self == 0 & ss2 != 0
	}

	if (big_float_is_zero(ss2)) {
		return false; // self != 0 & ss2 == 0
	}

	if( big_int_cmp_equal(self.exponent, ss2.exponent) && big_uint_cmp_equal(self.mantissa, ss2.mantissa, -1) )
		return true;
	
	return false;
}

/**
 * this method makes an integer value by skipping any fractions
 * 
 * samples:
 * -	10.7 will be 10
 * -	12.1  -- 12
 * -	-20.2 -- 20
 * -	-20.9 -- 20
 * -	-0.7  -- 0
 * -	0.8   -- 0
 * @param[in] self the big num object
 */
static void _big_float_skip_fraction(big_float_t* self) {
	if (big_float_is_nan(*self) || big_float_is_zero(*self) )
		return;
	
	if ( !big_int_is_sign(self->exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return;
	
	big_int_t negative_bits;
	big_int_init_int(&negative_bits, -(big_num_sstrg_t)(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));
	if (big_int_cmp_smaller_equal(self->exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		big_float_set_zero(self);
		return;
	}

	// exponent is in range (-BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self->exponent, &e);
	big_uint_clear_first_bits(&self->mantissa, -e);

	// we don't have to standardize 'Standardizing()' the value because
	// there's at least one bit in the mantissa
	// (the highest bit which we didn't touch)
}

/**
 * this method returns true if the value is integer
 * (there is no a fraction) ( we don't check NaN)
 * @param[in] self 
 * @return true 
 * @return false 
 */
static bool _big_float_is_integer(big_float_t self)
{
	if (big_float_is_zero(self))
		return true;
	
	if (!big_int_is_sign(self.exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return true;

	big_int_t negative_bits;
	big_int_init_int(&negative_bits, -(big_num_sstrg_t)(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));

	if (big_int_cmp_smaller_equal(self.exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		return false;
	}

	// exponent is in range (-BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self.exponent, &e);
	e = -e; // e means how many bits we must check

	big_num_strg_t len  = e / BIG_NUM_BITS_PER_UNIT;
	big_num_strg_t rest = e % BIG_NUM_BITS_PER_UNIT;
	size_t i = 0;

	for ( ; i < len; ++i)
		if (self.mantissa.table[i] != 0)
			return false;
	
	if (rest > 0) {
		big_num_strg_t rest_mask = BIG_NUM_MAX_VALUE >> (BIG_NUM_BITS_PER_UNIT - rest);
		if( (self.mantissa.table[i] & rest_mask) != 0)
			return false;
	}

	return true;
}

#ifdef __cplusplus
}
#endif