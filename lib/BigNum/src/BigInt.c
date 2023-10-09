/**
 * @file BigInt.c
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#include "BigNum/BigInt.h"

#include "stdio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TABLE_SIZE 	BIG_NUM_PREC
#define BIG_TABLE_SIZE (2*TABLE_SIZE)

// protected import functions from big_uint
extern big_num_carry_t		_big_uint_sub_uint(big_uint_t* self, big_num_strg_t value, size_t index);
extern big_num_carry_t		_big_uint_add_uint(big_uint_t* self, big_num_strg_t value, size_t index);
extern big_num_carry_t 		_big_uint_add_two_uints(big_uint_t* self, big_num_strg_t x2, big_num_strg_t x1, size_t index);

// private functions
static big_num_carry_t		_big_int_correct_carry_after_adding(big_int_t* self, bool p1_is_sign, bool p2_is_sign);
static big_num_carry_t		_big_int_add_int(big_int_t* self, big_num_strg_t value, size_t index);
static big_num_carry_t 		_big_int_add_two_ints(big_int_t* self, big_num_strg_t x2, big_num_strg_t x1, size_t index);

static big_num_carry_t		_big_int_correct_carry_after_subtracting(big_int_t* self, bool p1_is_sign, bool p2_is_sign);
static big_num_carry_t		_big_int_sub_int(big_int_t* self, big_num_strg_t value, size_t index);
static big_num_carry_t		_big_int_check_min_carry(big_int_t* self, bool ss1_is_sign, bool ss2_is_sign);

static big_num_carry_t		_big_int_pow2(big_int_t* self, const big_int_t pow);

/**
 * this method sets the max value which this class can hold
 * (all bits will be one besides the last one)
 * @param[in, out] self the big num object
 */
void big_int_set_max(big_int_t* self)
{
	big_uint_set_max(self);
	self->table[TABLE_SIZE-1] = ~BIG_NUM_HIGHEST_BIT;
}

/**
 * this method sets the min value which this class can hold
 * (all bits will be zero besides the last one which is one)
 * @param[in, out] self the big num object
 */
void big_int_set_min(big_int_t* self)
{
	big_uint_set_zero(self);
	self->table[TABLE_SIZE-1] = BIG_NUM_HIGHEST_BIT;
}

/**
 * 
 * @param[in, out] self the big num object
 */
void big_int_set_zero(big_int_t* self)
{
	big_uint_set_zero(self);
}

/**
 * this method swappes self and ss2
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the value to swap with self, ss2 contains self after swap
 */
void big_int_swap(big_int_t* self, big_int_t* ss2)
{
	big_uint_swap(self, ss2);
}

/**
 * this method sets -1 as the value
 * (-1 is equal the max value in an unsigned type)
 * @param[in, out] self the big num object
 */
void big_int_set_sign_one(big_int_t* self)
{
	big_uint_set_max(self);
}

/**
 * we change the sign of the value
 * 
 * if it isn't possible to change the sign this method returns 1
 * else return 0 and changing the sign
 * @param[in, out] self the big num object
 * @return big_int_sign_ret_t 
 */
big_int_sign_ret_t big_int_change_sign(big_int_t* self)
{
	/*
		if the value is equal that one which has been returned from SetMin
		(only the highest bit is set) that means we can't change sign
		because the value is too big (bigger about one)

		e.g. when value_size = 1 and value is -2147483648 we can't change it to the
		2147483648 because the max value which can be held is 2147483647

		we don't change the value and we're using this fact somewhere in some methods
		(if we look on our value without the sign we get the correct value 
		eg. -2147483648 in Int<1> will be 2147483648 on the UInt<1> type)
	*/
	if( big_uint_is_only_the_highest_bit_set(*self) )
		return 1;

	big_uint_t temp = *self;
	big_uint_set_zero(self);
	big_uint_sub(self, temp, 0);

	return 0;
}

/**
 * this method sets the sign
 * samples
 * -	 1  -> -1
 * - 	-2 -> -2
 * from a positive value we make a negative value,
 * if the value is negative we do nothing
 * @param[in, out] self the big num object
 */
void big_int_set_sign(big_int_t* self)
{
	if (big_int_is_sign(*self))
		return;
	big_int_change_sign(self);
}

/**
 * this method returns true if there's the sign
 * @param[in] self the big num object
 * @return true is negative
 * @return false is positive
 */
bool big_int_is_sign(big_int_t self)
{
	return big_uint_is_the_highest_bit_set(self);
}

/**
 * it sets an absolute value
 * it can return carry (1) (look on ChangeSign() for details)
 * @param[in, out] self the big num object
 * @return big_num_strg_t 
 */
big_num_strg_t big_int_abs(big_int_t* self)
{
	if (!big_int_is_sign(*self))
		return 0;
	return big_int_change_sign(self);
}

/**
 * this method adds two value with a sign and returns a carry
 * 
 * we're using methods from the base class because values are stored with U2
 * we must only make the carry correction
 * 
 * self = p1(=this) + p2
 * 
 * when p1>=0 i p2>=0 carry is set when the highest bit of value is set
 * when p1<0  i p2<0  carry is set when the highest bit of value is clear
 * when p1>=0 i p2<0  carry will never be set
 * when p1<0  i p2>=0 carry will never be set
 * @param[in, out] self the big num object
 * @param[in] ss2 
 * @return big_num_carry_t 
 */
big_num_carry_t big_int_add(big_int_t* self, big_int_t ss2)
{
	bool p1_is_sign = big_int_is_sign(*self);
	bool p2_is_sign = big_int_is_sign(ss2);

	big_uint_add(self, ss2, 0);

	return _big_int_correct_carry_after_adding(self, p1_is_sign, p2_is_sign);
}

/**
 * this method adds one *unsigned* word (at a specific position)
 * and returns a carry (if it was)
 * 
 * look at a description in UInt<>::AddInt(...)
 * @param[in, out] self the big num object
 * @param[in] value the value to add
 * @return big_num_strg_t 
 */
big_num_strg_t big_int_add_int(big_int_t* self, big_num_strg_t value)
{
	return _big_int_add_int(self, value, 0);
}

/**
 * this method subtracts two values with a sign
 * we don't use the previous Add because the method ChangeSign can
 * 
 * sometimes return carry 
 * 
 * this = p1(=this) - p2
 * 
 * -  when p1>=0 i p2>=0 carry will never be set
 * -  when p1<0  i p2<0  carry will never be set
 * -  when p1>=0 i p2<0  carry is set when the highest bit of value is set
 * -  when p1<0  i p2>=0 carry is set when the highest bit of value is clear
 * @param[in, out] self the big num object
 * @param[in] ss2 value to subtract from big num object
 * @return big_num_carry_t 
 */
big_num_carry_t big_int_sub(big_int_t* self, big_int_t ss2)
{
	bool p1_is_sign = big_int_is_sign(*self);
	bool p2_is_sign = big_int_is_sign(ss2);

	big_uint_sub(self, ss2, 0);		

	return _big_int_correct_carry_after_subtracting(self, p1_is_sign, p2_is_sign);
}

/**
 * Subtracts value from self
 * @param[in, out] self the big num object
 * @param[in] value the value to subtract from self
 * @return big_num_carry_t 
 */
big_num_strg_t big_int_sub_int(big_int_t* self, big_num_strg_t value)
{
	return _big_int_sub_int(self, value, 0);
}

/**
 * this method moves all bits into the left side
 * @param[in, out] self the big num object
 * @return size_t value how many bits have been moved
 */
size_t big_int_compensation_to_left(big_int_t* self)
{
	return big_uint_compensation_to_left(self);
}

/**
 * multiplication: this = this * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the value to multiply self by
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_mul_int(big_int_t* self, big_num_sstrg_t ss2)
{
	bool ss1_is_sign, ss2_is_sign;
	big_num_carry_t c;

	ss1_is_sign = big_int_is_sign(*self);

	/*
		we don't have to check the carry from Abs (values will be correct
		because next we're using the method MulInt from the base class UInt
		which is without a sign)
	*/
	big_int_abs(self);

	if( ss2 < 0 ) {
		ss2 = -ss2;
		ss2_is_sign = true;
	} else {
		ss2_is_sign = false;
	}

	c  = big_uint_mul_int(self, (big_num_strg_t)ss2);
	c += _big_int_check_min_carry(self, ss1_is_sign, ss2_is_sign);

	if( ss1_is_sign != ss2_is_sign )
		big_int_set_sign(self);

	return c;
}

/**
 * multiplication this = this * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the value to multiply self by
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_mul(big_int_t* self, big_int_t ss2)
{
	bool ss1_is_sign, ss2_is_sign;
	big_num_carry_t c;

	ss1_is_sign = big_int_is_sign(*self);
	ss2_is_sign = big_int_is_sign(ss2);

	/*
		we don't have to check the carry from Abs (values will be correct
		because next we're using the method Mul from the base class UInt
		which is without a sign)
	*/
	big_int_abs(self);
	big_int_abs(&ss2);

	c  = big_uint_mul(self, ss2, BIG_NUM_MUL_DEF);
	c += _big_int_check_min_carry(self, ss1_is_sign, ss2_is_sign);

	if( ss1_is_sign != ss2_is_sign )
		big_int_set_sign(self);

	return c;
}

/**
 * division this = this / divisor  (divisor is int)
 * returned values:
 * -  	0 - ok
 * -  	1 - division by zero
 * 
 * for example: (result means 'this')
 * -  	 20 /  3 --> result:  6   remainder:  2
 * -  	-20 /  3 --> result: -6   remainder: -2
 * -  	 20 / -3 --> result: -6   remainder:  2
 * -  	-20 / -3 --> result:  6   remainder: -2
 * 
 * in other words: this(old) = ss2 * this(new)(result) + remainder
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[in] remainder the remainder
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_int_div_int(big_int_t* self, big_num_sstrg_t divisor, big_num_sstrg_t * remainder)
{
	bool self_is_sign, divisor_is_sign;

	self_is_sign = big_int_is_sign(*self);

	/*
		we don't have to test the carry from Abs as well as in Mul
	*/
	big_int_abs(self);

	if( divisor < 0 ) {
		divisor = -divisor;
		divisor_is_sign = true;
	} else {
		divisor_is_sign = false;
	}

	big_num_strg_t rem;
	big_num_div_ret_t c = big_uint_div_int(self, (big_num_strg_t)divisor, &rem);

	if( self_is_sign != divisor_is_sign )
		big_int_set_sign(self);

	if( remainder ) {
		if( self_is_sign )
			*remainder = -(big_num_sstrg_t)rem;
		else
			*remainder = (big_num_sstrg_t)rem;
	}

	return c;
}

/**
 * division this = this / divisor
 * returned values:
 * -  0 - ok
 * -  1 - division by zero
 * 
 * for example: (result means 'this')
 * -  	 20 /  3 --> result:  6   remainder:  2
 * -  	-20 /  3 --> result: -6   remainder: -2
 * -  	 20 / -3 --> result: -6   remainder:  2
 * -  	-20 / -3 --> result:  6   remainder: -2
 * 
 * in other words: this(old) = divisor * this(new)(result) + remainder
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remainder
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_int_div(big_int_t* self, big_int_t divisor, big_int_t * remainder)
{
	bool self_is_sign, divisor_is_sign;

	self_is_sign = big_int_is_sign(*self);
	divisor_is_sign = big_int_is_sign(divisor);

	/*
		we don't have to test the carry from Abs as well as in Mul
	*/
	big_int_abs(self);
	big_int_abs(&divisor);

	big_num_div_ret_t c = big_uint_div(self, divisor, remainder, BIG_NUM_DIV_DEF);

	if( self_is_sign != divisor_is_sign )
		big_int_set_sign(self);

	if( self_is_sign && remainder )
		big_int_set_sign(remainder);

	return c;
}

/**
 * power this = this ^ pow
 * @param[in, out] self the big num object
 * @param[in] pow the power to raise self to
 * @return big_num_ret_t 
 */
big_num_ret_t big_int_pow(big_int_t* self, big_int_t pow)
{
	bool was_sign = big_int_is_sign(*self);
	big_num_strg_t c = 0;

	if( was_sign )
		c += big_int_abs(self);

	big_num_ret_t c_temp = big_uint_pow(self, pow);
	if( c_temp > 0 )
		return c_temp; // c_temp can be: 0, 1 or 2
	
	if( was_sign && (pow.table[0] & 1) == 1 )
		// negative value to the power of odd number is negative
		c += big_int_change_sign(self);

	return (c==0)? 0 : 1;
}

/**
 * UInt constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t
 */
big_num_carry_t big_int_init_uint(big_int_t* self, big_num_strg_t value)
{
	for(size_t i = 1; i < TABLE_SIZE ; ++i)
		self->table[i] = 0;
	self->table[0] = value;

	// there can be a carry here when the size of this value is equal to one word
	// and the 'value' has the highest bit set
	if( TABLE_SIZE==1 && (value & BIG_NUM_HIGHEST_BIT)!=0 )
		return 1;

	return 0;
}

/**
 * ULint constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_init_ulint(big_int_t* self, big_num_lstrg_t value)
{
	big_num_carry_t c = big_uint_init_ulint(self, value);

	if( c )
		return 1;

	if( TABLE_SIZE == 1 )
		return ((self->table[0] & BIG_NUM_HIGHEST_BIT) == 0) ? 0 : 1;
	
	if( TABLE_SIZE == 2 )
		return ((self->table[1] & BIG_NUM_HIGHEST_BIT) == 0) ? 0 : 1;

	return 0;
}

/**
 * big_uint_t constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t 
 */
big_num_carry_t big_int_init_big_uint(big_int_t* self, big_uint_t value)
{
	*self = value;
	return (value.table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0; // check if highest bit set, if so there is a carry
}

/**
 * int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_int_init_int(big_int_t* self, big_num_sstrg_t value)
{
	big_num_strg_t fill = ( value<0 ) ? BIG_NUM_MAX_VALUE : 0;

	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		self->table[i] = fill;

	self->table[0] = (big_num_strg_t)(value);
}

/**
 * long int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_init_lint(big_int_t* self, big_num_lsstrg_t value)
{
	big_num_strg_t mask = (value < 0) ? BIG_NUM_MAX_VALUE : 0;

	self->table[0] = (big_num_strg_t)(big_num_lstrg_t)value;

	if( TABLE_SIZE == 1 )
	{
		if( (big_num_strg_t)((big_num_lstrg_t)(value) >> 32) != mask )
			return 1;

		return ((self->table[0] & BIG_NUM_HIGHEST_BIT) == (mask & BIG_NUM_HIGHEST_BIT)) ? 0 : 1;
	}

	self->table[1] = (big_num_strg_t)((big_num_lstrg_t)(value) >> 32);

	for(size_t i=2 ; i < TABLE_SIZE ; ++i)
		self->table[i] = mask;

	return 0;
}

/**
 * big_int_t constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_int_init_big_int(big_int_t* self, big_int_t value)
{
	*self = value;
}

/**
 * uint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_uint(big_int_t self, big_num_strg_t * result)
{
	big_num_carry_t c = big_uint_to_uint(self, result);
	if (TABLE_SIZE == 1)
		return (*result & BIG_NUM_HIGHEST_BIT) == 0 ? 0 : 1;
	return c;
}

/**
 * int converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_int(big_int_t self, big_num_sstrg_t * result)
{
	return big_int_to_uint(self, result);
}

/**
 * luint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_luint(big_int_t self, big_num_lstrg_t * result)
{
	big_num_carry_t c = big_uint_to_luint(self, result);

	if (TABLE_SIZE == 1)
		return (self.table[0] & BIG_NUM_HIGHEST_BIT) == 0 ? 0 : 1;
	
	if (TABLE_SIZE == 2)
		return (self.table[1] & BIG_NUM_HIGHEST_BIT) == 0 ? 0 : 1;
	
	return c;
}

/**
 * lint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_lint(big_int_t self, big_num_lsstrg_t * result)
{
	if( TABLE_SIZE == 1 ) {
		*result = (big_num_lsstrg_t)((big_num_sstrg_t)(self.table[0]));
	} else {
		big_num_strg_t low  = self.table[0];
		big_num_strg_t high = self.table[1];

		*result = low;
		*result |= ((big_num_lsstrg_t)(high) << BIG_NUM_BITS_PER_UNIT);

		big_num_strg_t mask = big_int_is_sign(self) ? BIG_NUM_MAX_VALUE : 0;

		if( (high & BIG_NUM_HIGHEST_BIT) != (mask & BIG_NUM_HIGHEST_BIT) )
			return 1;

		for(size_t i=2 ; i<TABLE_SIZE ; ++i)
			if( self.table[i] != mask )
				return 1;
	}

	return 0;
}

/**
 * this method returns true if 'self' is smaller than 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * 
 * introduced for some optimization in the second division algorithm (Div2)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_int_cmp_smaller(big_int_t self, big_int_t l)
{
	big_num_sstrg_t i = TABLE_SIZE-1;

	big_num_sstrg_t a1 = (big_num_sstrg_t)(self.table[i]);
	big_num_sstrg_t a2 = (big_num_sstrg_t)(l.table[i]);

	if (a1 != a2)
		return a1 < a2;

	for (--i ; i >= 0 ; --i) {
		if (self.table[i] != l.table[i])
			return self.table[i] < l.table[i];
	}

	return false;
}

/**
 * this method returns true if 'self' is bigger than 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * 
 * introduced it for some optimization in the second division algorithm (Div2)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_int_cmp_bigger(big_int_t self, big_int_t l)
{
	big_num_sstrg_t i = TABLE_SIZE-1;

	big_num_sstrg_t a1 = (big_num_sstrg_t)(self.table[i]);
	big_num_sstrg_t a2 = (big_num_sstrg_t)(l.table[i]);

	if (a1 != a2)
		return a1 > a2;

	for (--i ; i >= 0 ; --i) {
		if (self.table[i] != l.table[i])
			return self.table[i] > l.table[i];
	}

	return false;
}

/**
 * this method returns true if 'self' is equal 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_int_cmp_equal(big_int_t self, big_int_t l)
{
	return big_uint_cmp_equal(self, l, -1);
}

/**
 * this method returns true if 'self' is smaller than or equal 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_int_cmp_smaller_equal(big_int_t self, big_int_t l)
{
	big_num_sstrg_t i = TABLE_SIZE-1;

	big_num_sstrg_t a1 = (big_num_sstrg_t)(self.table[i]);
	big_num_sstrg_t a2 = (big_num_sstrg_t)(l.table[i]);

	if (a1 != a2)
		return a1 < a2;

	for (--i ; i >= 0 ; --i) {
		if (self.table[i] != l.table[i])
			return self.table[i] < l.table[i];
	}

	return true;
}

/**
 * this method returns true if 'self' is bigger than or equal 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @return true 
 * @return false 
 */
bool big_int_cmp_bigger_equal(big_int_t self, big_int_t l)
{
	big_num_sstrg_t i = TABLE_SIZE-1;

	big_num_sstrg_t a1 = (big_num_sstrg_t)(self.table[i]);
	big_num_sstrg_t a2 = (big_num_sstrg_t)(l.table[i]);

	if (a1 != a2)
		return a1 > a2;

	for (--i ; i >= 0 ; --i) {
		if (self.table[i] != l.table[i])
			return self.table[i] > l.table[i];
	}

	return true;
}

void big_int_print(big_int_t self) {
	big_num_lsstrg_t val;
	big_int_to_lint(self, &val);
	printf("%lld", val);
}



/**
 * 
 * @param[in, out] self the big num object
 * @param[in] p1_is_sign 
 * @param[in] p2_is_sign 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_int_correct_carry_after_adding(big_int_t* self, bool p1_is_sign, bool p2_is_sign)
{
	if( !p1_is_sign && !p2_is_sign ) {
		if( big_uint_is_the_highest_bit_set(*self) )
			return 1;
	}

	if( p1_is_sign && p2_is_sign ) {	
		if( ! big_uint_is_the_highest_bit_set(*self) )
			return 1;
	}

	return 0;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_int_add_int(big_int_t* self, big_num_strg_t value, size_t index)
{
	bool p1_is_sign = big_int_is_sign(*self);

	_big_uint_add_uint(self, value, index);

	return _big_int_correct_carry_after_adding(self, p1_is_sign, false);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] x2 
 * @param[in] x1 
 * @param[in] index 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_int_add_two_ints(big_int_t* self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	bool p1_is_sign = big_int_is_sign(*self);

	_big_uint_add_two_uints(self, x2, x1, index);

	return _big_int_correct_carry_after_adding(self, p1_is_sign, false);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] p1_is_sign 
 * @param[in] p2_is_sign 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_int_correct_carry_after_subtracting(big_int_t* self, bool p1_is_sign, bool p2_is_sign)
{
	if( !p1_is_sign && p2_is_sign ) {
		if( big_uint_is_the_highest_bit_set(*self) )
			return 1;
	}

	if( p1_is_sign && !p2_is_sign ) {	
		if( ! big_uint_is_the_highest_bit_set(*self) )
			return 1;
	}

	return 0;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_int_sub_int(big_int_t* self, big_num_strg_t value, size_t index)
{
	bool p1_is_sign = big_int_is_sign(*self);

	_big_uint_sub_uint(self, value, index);

	return _big_int_correct_carry_after_subtracting(self, p1_is_sign, false);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] ss1_is_sign 
 * @param[in] ss2_is_sign 
 * @return big_num_carry_t 0, 1
 */
static big_num_carry_t _big_int_check_min_carry(big_int_t* self, bool ss1_is_sign, bool ss2_is_sign)
{
	/*
	we have to examine the sign of the result now
	but if the result is with the sign then:
		1. if the signs were the same that means the result is too big
		(the result must be without a sign)
		2. if the signs were different that means if the result
		is different from that one which has been returned from SetMin()
		that is carry (result too big) but if the result is equal SetMin()
		there'll be ok (and the next SetSign will has no effect because
		the value is actually negative -- look at description of that case
		in ChangeSign())
	*/
	if( big_int_is_sign(*self) )
	{
		if( ss1_is_sign != ss2_is_sign )
		{
			/*
				there can be one case where signs are different and
				the result will be equal the value from SetMin() (only the highest bit is set)
				(this situation is ok)
			*/
			if( !big_uint_is_only_the_highest_bit_set(*self) )
				return 1;
		}
		else
		{
			// signs were the same
			return 1;
		}
	}

	return 0;
}

/**
 * power this = this ^ pow
 * this can be negative
 * pow is >= 0
 * @param[in, out] self the big num object
 * @param[in] pow 
 * @return big_num_ret_t 
 */
static big_num_ret_t _big_int_pow2(big_int_t* self, const big_int_t pow)
{
	bool was_sign = big_int_is_sign(*self);
	big_num_carry_t c = 0;

	if( was_sign )
		c += big_int_abs(self);

	big_num_ret_t c_temp = big_uint_pow(self, pow);
	if( c_temp > 0 )
		return c_temp; // c_temp can be: 0, 1 or 2
	
	if( was_sign && (pow.table[0] & 1) == 1 )
		// negative value to the power of odd number is negative
		c += big_int_change_sign(self);

	return (c==0)? 0 : 1;
}

#ifdef __cplusplus
}
#endif