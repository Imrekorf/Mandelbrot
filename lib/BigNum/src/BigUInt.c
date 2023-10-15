/**
 * @file BigUInt.c
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#ifndef GL_core_profile
#include "BigNum/BigUInt.h"
#include <assert.h>
#endif

#if defined(__cplusplus) && !defined(GL_core_profile)
extern "C" {
#endif

#ifndef GL_core_profile
/**
 * return results for _big_uint_div_standard_test
 */
typedef enum {
	/**
	 *  none has to be done
	 */
	BIG_NUM_STD_DIV_NONE = 0,
	/**
	 * division by zero
	 */
	BIG_NUM_STD_DIV_ZERO = 1,
	/**
	 * division should be made
	 */
	BIG_NUM_STD_DIV		 = 2,
} div_std_test_t;

/**
 * return results for _big_uint_div_calculating_size
 */
typedef enum {
	/** ok
	 * -  'm' - is the index (from 0) of last non-zero word in table ('self')
	 * -  'n' - is the index (from 0) of last non-zero word in v.table
	 */
	BIG_NUM_CALC_DIV_OK    = 0, 
	/**
	 * v is zero
	 */
	BIG_NUM_CALC_DIV_ZERO  = 1,
	/**
	 * 'self' is zero
	 */
	BIG_NUM_CALC_SELF_ZERO = 2,
	/**
	 * 'self' is smaller than v
	 */
	BIG_NUM_CALC_SELF_LST  = 3,
	/**
	 * 'self' is equal v
	 */
	BIG_NUM_CALC_SELF_EQ   = 4,
} div_calc_test_t;

#else

#define BIG_NUM_STD_DIV_NONE	0
#define BIG_NUM_STD_DIV_ZERO	1
#define BIG_NUM_STD_DIV			2
#define div_std_test_t			uint

#define BIG_NUM_CALC_DIV_OK    	0
#define BIG_NUM_CALC_DIV_ZERO	1
#define BIG_NUM_CALC_SELF_ZERO	2
#define BIG_NUM_CALC_SELF_LST	3
#define BIG_NUM_CALC_SELF_EQ 	4
#define div_calc_test_t			uint

#endif

// protected functions
big_num_carry_t			_big_uint_sub_uint(_big_num_inout(big_uint_t, self), big_num_strg_t value, size_t index);
big_num_carry_t			_big_uint_add_uint(_big_num_inout(big_uint_t, self), big_num_strg_t value, size_t index);
big_num_carry_t 		_big_uint_add_two_uints(_big_num_inout(big_uint_t, self), big_num_strg_t x2, big_num_strg_t x1, size_t index);
big_num_sstrg_t  		_big_uint_find_leading_bit_in_word(big_num_strg_t x);

// private functions
_big_num_static big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, _big_num_out(big_num_strg_t, result));
_big_num_static big_num_carry_t	_big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, _big_num_out(big_num_strg_t, result));
_big_num_static big_num_carry_t	_big_uint_rcl2_one(_big_num_inout(big_uint_t, self), big_num_strg_t c);
_big_num_static big_num_carry_t _big_uint_rcr2_one(_big_num_inout(big_uint_t, self), big_num_strg_t c);
_big_num_static big_num_carry_t	_big_uint_rcl2(_big_num_inout(big_uint_t, self), size_t bits, big_num_strg_t c);
_big_num_static big_num_carry_t	_big_uint_rcr2(_big_num_inout(big_uint_t, self), size_t bits, big_num_strg_t c);
_big_num_static big_num_sstrg_t _big_uint_find_lowest_bit_in_word(big_num_strg_t x);
_big_num_static big_num_strg_t 	_big_uint_set_bit_in_word(_big_num_out(big_num_strg_t , value), size_t bit);
_big_num_static void 			_big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, _big_num_out(big_num_strg_t, result_high), _big_num_out(big_num_strg_t, result_low));
_big_num_static void			_big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, _big_num_out(big_num_strg_t, r), _big_num_out(big_num_strg_t, rest));


_big_num_static void 			_big_uint_rcl_move_all_words(_big_num_inout(big_uint_t, self), _big_num_out(size_t, rest_bits), _big_num_out(big_num_carry_t, last_c), size_t bits, big_num_strg_t c);
_big_num_static void 			_big_uint_rcr_move_all_words(_big_num_inout(big_uint_t, self), _big_num_out(size_t, rest_bits), _big_num_out(big_num_carry_t, last_c), size_t bits, big_num_strg_t c);

_big_num_static big_num_carry_t _big_uint_mul1(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2));
_big_num_static void 			_big_uint_mul1_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2_), _big_num_out(big_uint_t, result));
_big_num_static big_num_carry_t _big_uint_mul2(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2));
_big_num_static void 			_big_uint_mul2_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), _big_num_out(big_uint_t, result));
_big_num_static void 			_big_uint_mul2_no_carry2(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t ss1[2*UINT_PREC], _big_num_const_param big_num_strg_t ss2[2*UINT_PREC], _big_num_out(big_uint_t, result));
_big_num_static void			_big_uint_mul2_no_carry3(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t ss1[2*UINT_PREC], _big_num_const_param big_num_strg_t ss2[2*UINT_PREC], _big_num_out(big_uint_t, result), size_t x1start, size_t x1size, size_t x2start, size_t x2size);

_big_num_static div_std_test_t	_big_uint_div_standard_test(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(size_t, m), _big_num_out(size_t, n), _big_num_out(big_uint_t, remainder));
_big_num_static div_calc_test_t	_big_uint_div_calculating_size(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(size_t, m), _big_num_out(size_t, n));
_big_num_static big_num_div_ret_t 	_big_uint_div1(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder));
_big_num_static big_num_div_ret_t	_big_uint_div1_calculate(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, rest));
_big_num_static big_num_div_ret_t 	_big_uint_div2(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder));
_big_num_static big_num_div_ret_t	_big_uint_div2_calculate(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), _big_num_out(size_t, bits_diff));
_big_num_static big_num_div_ret_t	_big_uint_div2_find_leading_bits_and_check(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder),_big_num_out(size_t, table_id), _big_num_out(size_t, index), _big_num_out(size_t, divisor_table_id), _big_num_out(size_t, divisor_index));
_big_num_static bool 			_big_uint_div2_divisor_greater_or_equal(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), size_t table_id, size_t index, size_t divisor_index);


/**
 * this method returns the size of the table
 * @param[in, out] self the big num object
 * @return size_t the size of the table
 */
size_t big_uint_size(_big_num_inout(big_uint_t, self))
{
	return self->size;
}

/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_uint_set_zero(_big_num_inout(big_uint_t, self))
{
	for(size_t i=0 ; i < self->size ; ++i)
		self->table[i] = 0;
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_uint_set_one(_big_num_inout(big_uint_t, self))
{
	big_uint_set_zero(self);
	self->table[0] = 1;
}

/**
 * this method sets the max value which self class can hold
 * @param[in, out] self the big num object
 */
void big_uint_set_max(_big_num_inout(big_uint_t, self))
{
	for(size_t i=0 ; i < self->size; ++i)
		self->table[i] = BIG_NUM_MAX_VALUE;
}

/**
 * this method sets the min value which self class can hold
 * (for an unsigned integer value the zero is the smallest value)
 * @param[in, out] self the big num object
 */
void big_uint_set_min(_big_num_inout(big_uint_t, self))
{
	big_uint_set_zero(self);
}

/**
 * this method swappes self and ss2
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the value to swap with self, ss2 contains self after swap
 */
void big_uint_swap(_big_num_inout(big_uint_t, self), _big_num_inout(big_uint_t, ss2))
{
	for(size_t i=0 ; i < self->size ; ++i) {
		big_num_strg_t temp = self->table[i];
		self->table[i] = ss2->table[i];
		ss2->table[i] = temp;
	}
}

/**
 * this method copies the value stored in an another table
 * (warning: first values in temp_table are the highest words -- it's different
 * from our table)
 * we copy as many words as it is possible
 * 
 * if temp_table_len is bigger than value_size we'll try to round 
 * the lowest word from table depending on the last not used bit in temp_table
 * (self rounding isn't a perfect rounding -- look at the description below)
 * 
 * and if temp_table_len is smaller than value_size we'll clear the rest words
 * in the table
 * @param[in, out] self the big num object
 * @param[in] temp_table 
 * @param[in] temp_table_len 
 */
void big_uint_set_from_table(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t temp_table[2*UINT_PREC], size_t temp_table_len)
{
	size_t temp_table_index = 0;
	ssize_t i; // 'i' with a sign

	for(i=_big_num_ssize_t(self->size)-1 ; i>=0 && temp_table_index<temp_table_len; --i, ++temp_table_index)
		self->table[i] = temp_table[ temp_table_index ];


	// rounding mantissa
	if( temp_table_index < temp_table_len ) {
		if( (temp_table[temp_table_index] & BIG_NUM_HIGHEST_BIT) != 0 ) {
			/*
				very simply rounding
				if the bit from not used last word from temp_table is set to one
				we're rouding the lowest word in the table

				in fact there should be a normal addition but
				we don't use Add() or AddTwoInts() because these methods 
				can set a carry and then there'll be a small problem
				for optimization
			*/
			if( self->table[0] != BIG_NUM_MAX_VALUE )
				++(self->table[0]);
		}
	}

	// cleaning the rest of the mantissa
	for( ; i>=0 ; --i)
		self->table[i] = 0;
}

/*!
 *
 *	basic mathematic functions
 *
 */

/**
 * this method adds val to the existing value
 * @param[in, out] self the big num object
 * @param[in] val the value to add to the existing vlaue
 * @return big_num_carry_t the carry, if there was one
 */
big_num_carry_t big_uint_add_uint(_big_num_inout(big_uint_t, self), big_num_strg_t val)
{
	return _big_uint_add_uint(self, val, 0);
}

/**
 * this method adding ss2 to the this and adding carry if it's defined. (this = this + ss2 + c)
 * @param[in, out] self the big num object
 * @param[in] ss2  the big num object to add to self
 * @param[in] c    must be zero or one (might be a bigger value than 1), set to 1 if previous operation resulted in a carry
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_add(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_carry_t c)
{
	size_t i;	

	for(i=0 ; i<self->size ; ++i)
		c = _big_uint_add_two_words(self->table[i], ss2->table[i], c, _big_num_ref(self->table[i]));

	return c;
}

/**
 * this method subtracts val from the existing value
 * @param[in, out] self the big num object
 * @param[in] val the value to subtract from self
 * @return big_num_carry_t the carry, if there was one
 */
big_num_carry_t	big_uint_sub_uint(_big_num_inout(big_uint_t, self), big_num_strg_t val)
{
	return _big_uint_sub_uint(self, val, 0);
}

/**
 * this method's subtracting ss2 from the 'this' and subtracting
 * carry if it has been defined
 * (this = this - ss2 - c)
 * 
 * c must be zero or one (might be a bigger value than 1)
 * function returns carry (1) (if it was)
 * @param[in, out] self the big num object
 * @param[in] ss2  the big num object to subtracte from self
 * @param[in] c    must be zero or one (might be a bigger value than 1), set to 1 if previous operation resulted in a carry
 * @return big_num_carry_t 
 */
big_num_carry_t big_uint_sub(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_carry_t c)
{
	size_t i;
	for(i=0 ; i<self->size ; ++i)
		c = _big_uint_sub_two_words(self->table[i], ss2->table[i], c, _big_num_ref(self->table[i]));

	return c;
}

/**
 * moving all bits into the left side 'bits' times
 * return value <- self <- C
 * 
 * bits is from a range of <0, man * BIG_NUM_BITS_PER_UNIT>
 * or it can be even bigger then all bits will be set to 'c'
 * 
 * the value c will be set into the lowest bits
 * and the method returns state of the last moved bit
 * @param[in, out] self the big num object
 * @param[in] bits 
 * @param[in] c 
 * @return big_num_strg_t 
 */
big_num_strg_t big_uint_rcl(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c)
{
	big_num_strg_t last_c   = 0;
	size_t rest_bits 		= bits;

	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcl_move_all_words(self, _big_num_ref(rest_bits), _big_num_ref(last_c), bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to BIG_NUM_BITS_PER_UNIT-1 now
	if( rest_bits == 1 ) {
		last_c = _big_uint_rcl2_one(self, c);
	} else if( rest_bits == 2 ) {
		// performance tests showed that for rest_bits==2 it's better to use _big_uint_rcl2_one twice instead of _big_uint_rcl2(2,c)
		_big_uint_rcl2_one(self, c);
		last_c = _big_uint_rcl2_one(self, c);
	}
	else {
		last_c = _big_uint_rcl2(self, rest_bits, c);
	}
	
	return last_c;
}

/**
 * moving all bits into the right side 'bits' times
 * c -> self -> return value
 * 
 * @param[in, out] self the big num object
 * @param[in] bits number of bits to shift, range of <0, man * BIG_NUM_BITS_PER_UNIT>, or it can be even bigger then all bits will be set to 'c'
 * @param[in] c bit to insert in MSb
 * @return big_num_strg_t state of the last moved bit
 */
big_num_strg_t big_uint_rcr(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c)
{
	big_num_carry_t last_c  	= 0;
	size_t rest_bits 			= bits;
	
	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcr_move_all_words(self, _big_num_ref(rest_bits), _big_num_ref(last_c), bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to BIG_NUM_BITS_PER_UNIT-1 now
	if( rest_bits == 1 ) {
		last_c = _big_uint_rcr2_one(self, c);
	} else if( rest_bits == 2 ) {
		// performance tests showed that for rest_bits==2 it's better to use Rcr2_one twice instead of Rcr2(2,c)
		_big_uint_rcr2_one(self, c);
		last_c = _big_uint_rcr2_one(self, c);
	} else {
		last_c = _big_uint_rcr2(self, rest_bits, c);
	}

	return last_c;
}

/**
 * this method moves all bits into the left side
 * @param[in, out] self the big num object
 * @return size_t value how many bits have been moved
 */
size_t big_uint_compensation_to_left(_big_num_inout(big_uint_t, self))
{
	size_t moving = 0;

	// a - index a last word which is different from zero
	ssize_t a;
	for(a=_big_num_ssize_t(self->size)-1 ; a>=0 && self->table[a]==0 ; --a);

	if( a < 0 )
		return moving; // all words in table have zero

	if( a != self->size-1 ) {
		moving += ( self->size-1 - a ) * BIG_NUM_BITS_PER_UNIT;

		// moving all words
		ssize_t i;
		for(i=_big_num_ssize_t(self->size)-1 ; a>=0 ; --i, --a)
			self->table[i] = self->table[a];

		// setting the rest word to zero
		for(; i>=0 ; --i)
			self->table[i] = 0;
	}

	size_t moving2 = _big_uint_find_leading_bit_in_word( self->table[self->size-1] );
	// moving2 is different from -1 because the value table[self->size-1]
	// is not zero

	moving2 = BIG_NUM_BITS_PER_UNIT - moving2 - 1;
	big_uint_rcl(self, moving2, 0);

	return moving + moving2;
}

/**
 * this method looks for the highest set bit
 * @param[in, out] self the big num object
 * @param[out] table_id 'self' != 0: the index of a word <0..value_size-1>, else 0
 * @param[out] index    'self' != 0: the index of self set bit in the word <0..BIG_NUM_BITS_PER_UNIT), else 0
 * @return true: 	'self' is not zero
 * @return false: 	'self' is zero, both 'table_id' and 'index' are zero
 */
bool big_uint_find_leading_bit(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(size_t, table_id), _big_num_out(size_t, index))
{
	for(_big_num_deref(table_id)=self->size-1 ; (_big_num_deref(table_id))!=0 && self->table[_big_num_deref(table_id)]==0 ; --(_big_num_deref(table_id)));

	if( _big_num_deref(table_id)==0 && self->table[_big_num_deref(table_id)]==0 ) {
		// is zero
		_big_num_deref(index) = 0;

		return false;
	}
	
	// table[table_id] is different from 0
	_big_num_deref(index) = _big_uint_find_leading_bit_in_word( self->table[_big_num_deref(table_id)] );

	return true;
}

/**
 * this method looks for the smallest set bit
 * @param[in, out] self the big num object
 * @param[out] table_id 'self' != 0: the index of a word <0..value_size-1>, else 0
 * @param[out] index 'self' != 0: the index of self set bit in the word <0..BIG_NUM_BITS_PER_UNIT), else 0
 * @return true: self' is not zero
 * @return false: both 'table_id' and 'index' are zero
 */
bool big_uint_find_lowest_bit(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(size_t, table_id), _big_num_out(size_t, index))
{
	for(_big_num_deref(table_id)=0 ; _big_num_deref(table_id)<self->size && self->table[_big_num_deref(table_id)]==0 ; ++(_big_num_deref(table_id)));

		if( _big_num_deref(table_id) >= self->size )
		{
			// is zero
			_big_num_deref(index)    = 0;
			_big_num_deref(table_id) = 0;

			return false;
		}
		
		// table[table_id] is different from 0
		_big_num_deref(index) = _big_uint_find_lowest_bit_in_word( self->table[_big_num_deref(table_id)] );

	return true;
}

/**
 * getting the 'bit_index' bit
 * 
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the state of the bit
 */
bool big_uint_get_bit(_big_num_const_param _big_num_inout(big_uint_t, self), size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_UNIT;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_UNIT;

	big_num_strg_t temp = self->table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(_big_num_ref(temp), bit);

	return res != 0;
}

/**
 * setting the 'bit_index' bit
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the last state of the bit 
 */
bool big_uint_set_bit(_big_num_inout(big_uint_t, self), size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_UNIT;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_UNIT;

	big_num_strg_t temp = self->table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(_big_num_ref(temp), bit);

	return res != 0;
}

/**
 * this method performs a bitwise operation AND 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_and(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2))
{
	for(size_t x=0 ; x<self->size ; ++x)
		self->table[x] &= ss2->table[x];
}

/**
 * this method performs a bitwise operation OR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_or(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2))
{
	for(size_t x=0 ; x<self->size ; ++x)
		self->table[x] |= ss2->table[x];
}

/**
 * this method performs a bitwise operation XOR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_xor(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2))
{
	for(size_t x=0 ; x<self->size ; ++x)
		self->table[x] ^= ss2->table[x];
}

/**
 * this method performs a bitwise operation NOT
 * @param[in, out] self the big num object
 */
void big_uint_bit_not(_big_num_inout(big_uint_t, self))
{
	for(size_t x=0 ; x<self->size ; ++x)
		self->table[x] = ~self->table[x];
}

/**
 * this method performs a bitwise operation NOT but only
 * on the range of <0, leading_bit>
 * 
 * for example:
 * 		BitNot2(8) = BitNot2( 1000(bin) ) = 111(bin) = 7
 * @param[in, out] self the big num object
 */
void big_uint_bit_not2(_big_num_inout(big_uint_t, self))
{
	size_t table_id, index;

	if( big_uint_find_leading_bit(self, _big_num_ref(table_id), _big_num_ref(index)) ) {
		for(size_t x=0 ; x<table_id ; ++x)
			self->table[x] = ~(self->table[x]);

		big_num_strg_t mask  = BIG_NUM_MAX_VALUE;
		size_t shift = BIG_NUM_BITS_PER_UNIT - index - 1;

		if(shift != 0)
			mask >>= shift;

		self->table[table_id] ^= mask;
	}
	else
		self->table[0] = 1;
}

/*!
 *
 * Multiplication
 *
 *
 */

/**
 * multiplication: self = self * ss2
 * 
 * it can return a carry
 * @param[in, out] self the big num object
 * @param[in] ss2 number to multiply by
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_mul_int(_big_num_inout(big_uint_t, self), big_num_strg_t ss2)
{
	big_num_strg_t r1, r2;
	size_t x1;
	big_num_carry_t c = 0;

	big_uint_t u = _big_num_deref(self);
	big_uint_set_zero(self);

	if( ss2 == 0 ) {
		return 0;
	}

	for(x1=0 ; x1<self->size-1 ; ++x1) {
		_big_uint_mul_two_words(u.table[x1], ss2, _big_num_ref(r2), _big_num_ref(r1));
		c += _big_uint_add_two_uints(self, r2,r1,x1);
	}

	// x1 = value_size-1  (last word)
	_big_uint_mul_two_words(u.table[x1], ss2, _big_num_ref(r2), _big_num_ref(r1));
	c += (r2!=0) ? 1 : 0;
	c += _big_uint_add_uint(self, r1, x1);

	return (c==0)? 0 : 1;
}

/**
 * the multiplication 'self' = 'self' * ss2
 * 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @param[in] algorithm the algorithm to use for multiplication
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_mul(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), big_num_algo_t algorithm)
{
	switch (algorithm) {
		case BIG_NUM_MUL1: return _big_uint_mul1(self, ss2); break;
		case BIG_NUM_MUL2: return _big_uint_mul2(self, ss2); break;
		default: _big_uint_mul1(self, ss2); break;
	}
}

/**
 * the multiplication 'self' = 'self' * ss2
 * 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @param[in] algorithm the algorithm to use for multiplication
 */
void big_uint_mul_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), _big_num_out(big_uint_t, result), big_num_algo_t algorithm)
{
	switch (algorithm) {
		case BIG_NUM_MUL1: _big_uint_mul1_no_carry(self, ss2, result); break;
		case BIG_NUM_MUL2: _big_uint_mul2_no_carry(self, ss2, result); break;
		default: _big_uint_mul1_no_carry(self, ss2, result); break;
	}
}

/*!
 *
 * Division
 *
 *
 */

/**
 * division by one unsigned word
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor 
 * @param[out] remainder 
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_uint_div_int(_big_num_inout(big_uint_t, self), big_num_strg_t divisor, _big_num_out(big_num_strg_t, remainder))
{
	if( divisor == 0 ) {
		#ifndef GL_core_profile 
		if( remainder )
		#endif
			_big_num_deref(remainder) = 0; // this is for convenience, without it the compiler can report that 'remainder' is uninitialized

		return 1;
	}

	if( divisor == 1 ) {
		#ifndef GL_core_profile 
		if( remainder )
		#endif
			_big_num_deref(remainder) = 0;

		return 0;
	}

	big_uint_t dividend = _big_num_deref(self);
	big_uint_set_zero(self);
	
	big_num_sstrg_t i;  // i must be with a sign
	big_num_strg_t r = 0;

	// we're looking for the last word in ss1
	for(i=_big_num_ssize_t(self->size)-1 ; i>0 && dividend.table[i]==0 ; --i);

	for( ; i>=0 ; --i)
		_big_uint_div_two_words(r, dividend.table[i], divisor, _big_num_ref(self->table[i]), _big_num_ref(r));

	#ifndef GL_core_profile 
	if( remainder )
	#endif
		_big_num_deref(remainder) = r;

	return 0;
}

/**
 * division self = self / ss2
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor 
 * @param[out] remainder 
 * @param[in] algorithm 
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_uint_div(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), big_num_algo_t algorithm)
{
	switch( algorithm ){
		default:
		case 1:
			return _big_uint_div1(self, divisor, remainder);

		case 2:
			return _big_uint_div2(self, divisor, remainder);
	}
}

/**
 * power self = self ^ pow
 * binary algorithm (r-to-l)
 * 
 * @param[in, out] self the big num object
 * @param[in] _pow The power to raise self to
 * @return big_num_ret_t 
 */
big_num_ret_t big_uint_pow(_big_num_inout(big_uint_t, self), big_uint_t _pow)
{
	if(big_uint_is_zero(_big_num_ref(_pow)) && big_uint_is_zero(self))
		// we don't define zero^zero
		return BIG_NUM_INVALID_ARG;

	big_uint_t start = _big_num_deref(self);
	big_uint_t result;
	big_uint_init(_big_num_ref(result), self->size);
	big_uint_set_one(_big_num_ref(result));
	big_num_carry_t c = 0;

	while( c == 0) {
		if( (_pow.table[0] & 1) != 0)
			c += big_uint_mul(_big_num_ref(result), _big_num_ref(start), BIG_NUM_MUL_DEF);

		_big_uint_rcr2_one(_big_num_ref(_pow), 0);
		if( big_uint_is_zero(_big_num_ref(_pow)) )
			break;

		c += big_uint_mul(_big_num_ref(start), _big_num_ref(start), BIG_NUM_MUL_DEF);
	}

	_big_num_deref(self) = result;

	return (c==0)? 0 : 1;
}

/**
 * square root
 * e.g. Sqrt(9) = 3
 * ('digit-by-digit' algorithm)
 * @param[in, out] self the big num object
 */
void big_uint_sqrt(_big_num_inout(big_uint_t, self))
{
	big_uint_t bit, temp;
	big_uint_init(_big_num_ref(bit), self->size);

	if( big_uint_is_zero(self))
		return;

	big_uint_t value = _big_num_deref(self);

	big_uint_set_zero(self);
	big_uint_set_zero(_big_num_ref(bit));
	bit.table[self->size-1] = (BIG_NUM_HIGHEST_BIT >> 1);
	
	while( big_uint_cmp_bigger(_big_num_ref(bit), _big_num_ref(value), -1))
		big_uint_rcr(_big_num_ref(bit), 2, 0);

	while( !big_uint_is_zero(_big_num_ref(bit)) ) {
		temp = _big_num_deref(self);
		big_uint_add(_big_num_ref(temp), _big_num_ref(bit), 0);

		if( big_uint_cmp_bigger_equal(_big_num_ref(value), _big_num_ref(temp), -1) ) {
			big_uint_sub(_big_num_ref(value), _big_num_ref(temp), 0);
			big_uint_rcr(self, 1, 0);
			big_uint_add(self, _big_num_ref(bit), 0);
		}
		else {
			big_uint_rcr(self, 1, 0);
		}

		big_uint_rcr(_big_num_ref(bit), 2, 0);
	}
}

/**
 * this method sets n first bits to value zero
 * 
 * For example:
 * let n=2 then if there's a value 111 (bin) there'll be '100' (bin)
 * @param[in, out] self the big num object
 * @param[in] n 
 */
void big_uint_clear_first_bits(_big_num_inout(big_uint_t, self), size_t n)
{
	if( n >= self->size*BIG_NUM_BITS_PER_UNIT ) {
		big_uint_set_zero(self);
		return;
	}

	size_t p = 0;

	// first we're clearing the whole words
	while( n >= BIG_NUM_BITS_PER_UNIT ) {
		self->table[p++] = 0;
		n   -= BIG_NUM_BITS_PER_UNIT;
	}

	if( n == 0 ) {
		return;
	}

	// and then we're clearing one word which has left
	// mask -- all bits are set to one
	big_num_strg_t mask = BIG_NUM_MAX_VALUE;

	mask = mask << n;

	(self->table[p]) &= mask;
}

/**
 * this method returns true if the highest bit of the value is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_the_highest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self))
{
	return (self->table[self->size-1] & BIG_NUM_HIGHEST_BIT) != 0;
}

/**
 * this method returns true if the lowest bit of the value is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_the_lowest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self))
{
	return (self->table[0] & 1) != 0;
}

/**
 * returning true if only the highest bit is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_only_the_highest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self))
{
	for(size_t i=0 ; i<self->size-1 ; ++i)
		if( self->table[i] != 0 )
			return false;
	
	if( self->table[self->size-1] != BIG_NUM_HIGHEST_BIT )
		return false;
	
	return true;
}

/**
 * returning true if only the lowest bit is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_only_the_lowest_bit_set(_big_num_const_param _big_num_inout(big_uint_t, self))
{
	if( self->table[0] != 1 )
		return false;

	for(size_t i=1 ; i<self->size ; ++i)
		if( self->table[i] != 0 )
			return false;

	return true;
}

/**
 * this method returns true if the value is equal zero
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_zero(_big_num_const_param _big_num_inout(big_uint_t, self))
{
	for(size_t i=0 ; i<self->size ; ++i)
		if(self->table[i] != 0)
			return false;

	return true;
}

/**
 * returning true if first 'bits' bits are equal zero
 * @param[in, out] self the big num object
 * @param[in] bits 
 * @return true 
 * @return false 
 */
bool big_uint_are_first_bits_zero(_big_num_const_param _big_num_inout(big_uint_t, self), size_t bits)
{
	size_t index = bits / BIG_NUM_BITS_PER_UNIT;
	size_t rest  = bits % BIG_NUM_BITS_PER_UNIT;
	size_t i;

	for(i=0 ; i<index ; ++i)
		if( self->table[i] != 0 )
			return false;

	if( rest == 0 )
		return true;

	big_num_strg_t mask = BIG_NUM_MAX_VALUE >> (BIG_NUM_BITS_PER_UNIT - rest);

	return (self->table[i] & mask) == 0;
}

/*!
 *
 *	initialization methods
 *
 */

/**
 * default constructor big_uint_t
 * @param[in, out] self the big num object
 * @param[in] size the size for this big uint
 */
void big_uint_init(_big_num_inout(big_uint_t, self), size_t size)
{
	#ifndef GL_core_profile
	assert(size <= 2*UINT_PREC);
	#endif
	self->size = size > 2*UINT_PREC ? 2*UINT_PREC : size;
}

/**
 * a constructor for converting the big_num_strg_t big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 */
void big_uint_init_uint(_big_num_inout(big_uint_t, self), size_t size, big_num_strg_t value)
{
	#ifndef GL_core_profile
	assert(size <= 2*UINT_PREC);
	#endif
	self->size = size;
	for(size_t i=1 ; i<self->size ; ++i)
		self->table[i] = 0;

	self->table[0] = value;
}

/**
 * a constructor for converting big_num_lstrg_t int to big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_ulint(_big_num_inout(big_uint_t, self), size_t size, big_num_lstrg_t value)
{
	#ifndef GL_core_profile
	assert(size <= 2*UINT_PREC);
	#endif
	self->size = size;
	self->table[0] = _big_num_strg_t(value);

	if( self->size == 1 )
	{
		big_num_carry_t c = ((value >> BIG_NUM_BITS_PER_UNIT) == 0) ? 0 : 1;

		return c;
	}

	self->table[1] = _big_num_strg_t(value >> BIG_NUM_BITS_PER_UNIT);

	for(size_t i=2 ; i<self->size ; ++i)
		self->table[i] = 0;

	return 0;
}

/**
 * a copy constructor
 * @param[in, out] self the big num object
 * @param[in] u the other big num object
 */
big_num_carry_t big_uint_init_big_uint(_big_num_inout(big_uint_t, self), size_t size, _big_num_const_param _big_num_inout(big_uint_t, value))
{
	#ifndef GL_core_profile
	assert(size <= 2*UINT_PREC);
	#endif
	self->size = size;
	size_t min_size = (self->size < value->size) ? self->size : value->size;
	size_t i;
	for( i = 0; i<min_size ; ++i)
		self->table[i] = value->table[i];
	if (self->size > value->size) {
		for ( ; i < self->size ; ++i)
			self->table[i] = 0;
	} else {
		for( ; i<value->size ; ++i)
			if (value->table[i] != 0)
				return 1;
	}

	return 0;
}

/**
 * a constructor for converting the big_num_sstrg_t to big_uint_t
 * 
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_int(_big_num_inout(big_uint_t, self), size_t size, big_num_sstrg_t value)
{
	big_uint_init_uint(self, size, _big_num_strg_t(value));
	if (value < 0)
		return 1;
	return 0;
}

/**
 * a constructor for converting big_num_lsstrg_t to big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_lint(_big_num_inout(big_uint_t, self), size_t size, big_num_lsstrg_t value)
{
	big_num_carry_t c = big_uint_init_ulint(self, size, _big_num_lstrg_t(value));
	if (c != 0 || value < 0)
		return 1;
	return 0;
}

/**
 * this method converts the value to big_num_strg_t type
 * can return a carry if the value is too long to store it in big_num_strg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_to_uint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_strg_t, result))
{
	_big_num_deref(result) = self->table[0];

	for(size_t i=1 ; i<self->size ; ++i)
		if( self->table[i] != 0 )
			return 1;

	return 0;
}

/**
 * this method converts the value to big_num_sstrg_t type (signed integer)
 * can return a carry if the value is too long to store it in big_num_sstrg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_to_int(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_sstrg_t, result))
{
	big_num_strg_t _result;
	big_num_carry_t c = big_uint_to_uint(self, _big_num_ref(_result));
	_big_num_deref(result) = _big_num_sstrg_t(_result);
	return c;
}

/**
 * this method converts the value to big_num_lstrg_t type (long integer)
 * can return a carry if the value is too long to store it in big_num_lstrg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t 
 */
big_num_carry_t	big_uint_to_luint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_lstrg_t, result))
{
	_big_num_deref(result) = self->table[0];
	_big_num_deref(result) += (_big_num_lstrg_t(self->table[1])) << BIG_NUM_BITS_PER_UNIT;

	for(size_t i=2 ; i<self->size ; ++i)
		if( self->table[i] != 0 )
			return 1;

	return 0;
}

/**
 * this method converts the value to big_num_lsstrg_t type (long signed integer)
 * can return a carry if the value is too long to store it in big_num_lsstrg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t 
 */
big_num_carry_t	big_uint_to_lint(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_out(big_num_lsstrg_t, result))
{
	big_num_lstrg_t _result;
	big_num_carry_t c = big_uint_to_luint(self, _big_num_ref(_result));
	_big_num_deref(result) = _big_num_lsstrg_t(_result);
	return c;
}

/*!
 *
 *	methods for comparing
 *
 */

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
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_smaller(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index >= _big_num_sstrg_t(self->size) )
		i = _big_num_sstrg_t(self->size) - 1;
	else
		i = index;


	for( ; i>=0 ; --i)
	{
		if( self->table[i] != l->table[i] )
			return self->table[i] < l->table[i];
	}

	// they're equal
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
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_bigger(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= _big_num_sstrg_t(self->size) )
		i = _big_num_sstrg_t(self->size) - 1;
	else
		i = index;


	for( ; i>=0 ; --i)
	{
		if( self->table[i] != l->table[i] )
			return self->table[i] > l->table[i];
	}

	// they're equal
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
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= _big_num_sstrg_t(self->size) )
		i = _big_num_sstrg_t(self->size) - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self->table[i] != l->table[i] )
			return false;
	
	return true;
}

/**
 * this method returns true if 'self' is smaller than or equal 'l'
 * 
 * 'index' is an index of the first word from will be the comparison performed
 * (note: we start the comparison from back - from the last word, when index is -1 /default/
 * it is automatically set into the last word)
 * @param[in, out] self the big num object
 * @param[in] l the other big num object
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_smaller_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= _big_num_sstrg_t(self->size) )
		i = _big_num_sstrg_t(self->size) - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self->table[i] != l->table[i] )
			return self->table[i] < l->table[i];
	
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
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_bigger_equal(_big_num_const_param _big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, l), ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= _big_num_sstrg_t(self->size) )
		i = _big_num_sstrg_t(self->size) - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self->table[i] != l->table[i] )
			return self->table[i] > l->table[i];
	
	return true;
}


/**
 * this method adds two words together
 * @param[in] a 
 * @param[in] b 
 * @param[in] carry carry
 * @param[out] result 
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, _big_num_out(big_num_strg_t, result))
{
	big_num_strg_t temp;

	if( carry == 0 ) {
		temp = a + b;

		if( temp < a )
			carry = 1;
	} else {
		carry = 1;
		temp  = a + b + carry;

		if( temp > a ) // !(temp<=a)
			carry = 0;
	}

	_big_num_deref(result) = temp;

	return carry;
}

/**
 * this method adds one word (at a specific position)
 * and returns a carry (if it was)
 * if we've got (value_size=3):
 * 		table[0] = 10;
 * 		table[1] = 30;
 * 		table[2] = 5;
 * and we call:
 * 		AddInt(2,1)
 * then it'll be:
 * 		table[0] = 10;
 * 		table[1] = 30 + 2;
 * 		table[2] = 5;
 * of course if there was a carry from table[2] it would be returned
 * @param[in, out] self the big num object
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_uint(_big_num_inout(big_uint_t, self), big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self->table[index], value, 0, _big_num_ref(self->table[index]));

	for(i=index+1 ; i < self->size && c != 0 ; ++i)
		c = _big_uint_add_two_words(self->table[i], 0, c, _big_num_ref(self->table[i]));
	
	return c;
}

/**
 * this method adds only two unsigned words to the existing value
 * and these words begin on the 'index' position
 * (it's used in the multiplication algorithm 2)
 * index should be equal or smaller than value_size-2 (index <= value_size-2)
 * x1 - lower word, x2 - higher word
 * for example if we've got value_size equal 4 and:
 * 		table[0] = 3
 * 		table[1] = 4
 * 		table[2] = 5
 * 		table[3] = 6
 * then let
 * 		x1 = 10
 * 		x2 = 20
 * and
 * 		index = 1
 * the result of this method will be:
 * 		table[0] = 3
 * 		table[1] = 4 + x1 = 14
 * 		table[2] = 5 + x2 = 25
 * 		table[3] = 6
 * and no carry at the end of table[3]
 * (of course if there was a carry in table[2](5+20) then 
 * this carry would be passed to the table[3] etc.)
 * @param[in, out] self the big num object
 * @param[in] x1 lower word
 * @param[in] x2 higher word
 * @param[in] index index <= value_size-2
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_two_uints(_big_num_inout(big_uint_t, self), big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self->table[index],   x1, 0, _big_num_ref(self->table[index]));
	c = _big_uint_add_two_words(self->table[index+1], x2, c, _big_num_ref(self->table[index+1]));

	for(i=index+2 ; i<self->size && c != 0; ++i)
		c = _big_uint_add_two_words(self->table[i], 0, c, _big_num_ref(self->table[i]));
	
	return c;
}

/**
 * this method subtractes one word from the other
 * @param[in] a 
 * @param[in] b 
 * @param[in] carry carry
 * @param[out] result 
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, _big_num_out(big_num_strg_t, result))
{
	if( carry == 0 ) {
		_big_num_deref(result) = a - b;

		if( a < b )
			carry = 1;
	} else {
		carry   = 1;
		_big_num_deref(result) = a - b - carry;

		if( a > b ) // !(a <= b )
			carry = 0;
	}

	return carry;
}

/**
 * this method subtracts one word (at a specific position)
 * and returns a carry (if it was)
 * if we've got (value_size=3):
 * 		table[0] = 10;
 * 		table[1] = 30;
 * 		table[2] = 5;	
 * and we call:
 * 		SubInt(2,1)
 * then it'll be:
 * 		table[0] = 10;
 * 		table[1] = 30 - 2;
 * 		table[2] = 5;
 * of course if there was a carry from table[2] it would be returned
 * @param[in, out] self the big num object
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_sub_uint(_big_num_inout(big_uint_t, self), big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_sub_two_words(self->table[index], value, 0, _big_num_ref(self->table[index]));

	for(i=index+1 ; i<self->size && c != 0; ++i)
		c = _big_uint_sub_two_words(self->table[i], 0, c, _big_num_ref(self->table[i]));

	return c;
}

/**
 * this method moves all bits into the left hand side
 * return value <- self <- c
 * 
 * the lowest *bit* will be held the 'c' and
 * the state of one additional bit (on the left hand side)
 * will be returned
 * 
 * for example:
 * let self is 001010000
 * after _big_uint_rcl2_one(1) there'll be 010100001 and _big_uint_rcl2_one returns 0
 * @param[in, out] self 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_rcl2_one(_big_num_inout(big_uint_t, self), big_num_carry_t c)
{
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = 1;

	for(i=0 ; i<self->size ; ++i) {
		new_c    		= (self->table[i] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
		self->table[i] 	= (self->table[i] << 1) | c;
		c        		= new_c;
	}

	return c;
}

/**
 * this method moves all bits into the right hand side
 * c -> self -> return value
 * 
 * the highest *bit* will be held the 'c' and
 * the state of one additional bit (on the right hand side)
 * will be returned
 * 
 * for example:
 * let self is 000000010
 * after _big_uint_rcr2_one(1) there'll be 100000001 and _big_uint_rcr2_one returns 0
 * @param[in, out] self 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_rcr2_one(_big_num_inout(big_uint_t, self), big_num_carry_t c)
{
	big_num_sstrg_t i; // signed i
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_HIGHEST_BIT;

	for(i= _big_num_sstrg_t(self->size-1) ; i>=0 ; --i) {
		new_c    		= (self->table[i] & 1) != 0 ? BIG_NUM_HIGHEST_BIT : 0;
		self->table[i] 	= (self->table[i] >> 1) | c;
		c        		= new_c;
	}

	c = (c != 0)? 1 : 0;

	return c;
}

/**
 * this method moves all bits into the left hand side
 * return value <- self <- c
 * 
 * the lowest *bits* will be held the 'c' and
 * the state of one additional bit (on the left hand side)
 * will be returned
 * 
 * for example:
 * let self is 001010000
 * after _big_uint_rcl2(3, 1) there'll be 010000111 and _big_uint_rcl2 returns 1
 * @param[in, out] self 
 * @param[in] bits 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_rcl2(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE >> move;

	for(i=0 ; i<self->size ; ++i) {
		new_c    		= self->table[i] >> move;
		self->table[i] 	= (self->table[i] << bits) | c;
		c        		= new_c;
	}

	return (c & 1);
}

/**
 * this method moves all bits into the right hand side
 * C -> self -> return value
 * 
 * the highest *bits* will be held the 'c' and
 * the state of one additional bit (on the right hand side)
 * will be returned
 * 
 * for example:
 * let self is 000000010
 * after _big_uint_rcr2(2, 1) there'll be 110000000 and _big_uint_rcr2 returns 1
 * @param[in, out] self 
 * @param[in] bits 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_rcr2(_big_num_inout(big_uint_t, self), size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	ssize_t i; // signed
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE << move;

	for(i=_big_num_ssize_t(self->size)-1 ; i>=0 ; --i) {
		new_c    		= self->table[i] << move;
		self->table[i] 	= (self->table[i] >> bits) | c;
		c        		= new_c;
	}

	c = (c & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;

	return c;
}

/**
 * this method returns the number of the highest set bit in x
 * @param[in] x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
big_num_sstrg_t _big_uint_find_leading_bit_in_word(big_num_strg_t x)
{
	if( x == 0 )
		return -1;

	big_num_strg_t bit = BIG_NUM_BITS_PER_UNIT - 1;
	
	while( (x & BIG_NUM_HIGHEST_BIT) == 0 ) {
		x = x << 1;
		--bit;
	}

	return _big_num_sstrg_t(bit);
}

/**
 * this method returns the number of the highest set bit in x
 * @param[in] x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
_big_num_static big_num_sstrg_t _big_uint_find_lowest_bit_in_word(big_num_strg_t x)
{
	if( x == 0 )
		return -1;

	big_num_sstrg_t bit = 0;
	
	while( (x & 1) == 0 ) {
		x = x >> 1;
		++bit;
	}

	return bit;
}

/**
 * this method sets a special bit in the 'value'
 * and returns the last state of the bit (zero or one)
 * 
 * e.g.
 * 		uint x = 100;
 * 		uint bit = _big_uint_set_bit_in_word(x, 3);
 * now: x = 108 and bit = 0
 * @param[in, out] value 
 * @param[in] bit the bit to set, between <0,BIG_NUM_BITS_PER_UNIT-1>
 * @return big_num_strg_t 
 */
_big_num_static big_num_strg_t _big_uint_set_bit_in_word(_big_num_out(big_num_strg_t , value), size_t bit)
{
	big_num_strg_t mask = 1;

	if( bit > 0 )
		mask = mask << bit;

	big_num_strg_t last = _big_num_deref(value) & mask;
	_big_num_deref(value)= _big_num_deref(value) | mask;

	return (last != 0) ? 1 : 0;
}

/**
 * multiplication: result_high:result_low = a * b
 * -  result_high - higher word of the result
 * -  result_low  - lower word of the result
 * 
 * this methos never returns a carry
 * 
 * this method is used in the second version of the multiplication algorithms
 * @param[in] a 
 * @param[in] b 
 * @param[out] result_high 
 * @param[out] result_low 
 */
_big_num_static void _big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, _big_num_out(big_num_strg_t, result_high), _big_num_out(big_num_strg_t, result_low))
{
#ifndef GL_core_profile
	union uint_
	{
		struct
		{
			big_num_strg_t low;  // 1 word
			big_num_strg_t high; // 1 word
		} u_;

		big_num_lstrg_t u;       // 2 words
	} res;

	res.u = (big_num_lstrg_t)a * (big_num_lstrg_t)b;     // multiply two 32bit words, the result has 64 bits

	_big_num_deref(result_high) = res.u_.high;
	_big_num_deref(result_low)  = res.u_.low;
#else
	uvec2 res = unpackUint2x32(big_num_lstrg_t(a) * big_num_lstrg_t(b));     // multiply two 32bit words, the result has 64 bits

	result_high = res.y;
	result_low  = res.x;
#endif
}


/**
 * this method calculates big_num_lstrg_t a / big_num_strg_t c (a higher, b lower word)
 * r = a / c and rest - remainder
 * @warning the c has to be suitably large for the result being keeped in one word,
 * if c is equal zero there'll be a hardware interruption (0)
 * and probably the end of your program
 * @param[in] a 
 * @param[in] b 
 * @param[in] c
 * @param[out] r 
 * @param[out] rest 
 */
_big_num_static void _big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, _big_num_out(big_num_strg_t, r), _big_num_out(big_num_strg_t, rest))
{
	big_num_lstrg_t ab_u = (_big_num_lstrg_t(a) << BIG_NUM_BITS_PER_UNIT) | _big_num_lstrg_t(b);

	_big_num_deref(r)    = _big_num_strg_t(ab_u / c);
	_big_num_deref(rest) = _big_num_strg_t(ab_u % c);
}




/**
 * an auxiliary method for moving bits into the left hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param[out] rest_bits 
 * @param[out] last_c 
 * @param[in] bits 
 * @param[in] c carry
 */
_big_num_static void _big_uint_rcl_move_all_words(_big_num_inout(big_uint_t, self), _big_num_out(size_t, rest_bits), _big_num_out(big_num_carry_t, last_c), size_t bits, big_num_carry_t c)
{
	_big_num_deref(rest_bits) 	= bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 			= _big_num_ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask 		= ( c != 0 ) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= self->size ) {
		if( all_words == self->size && _big_num_deref(rest_bits) == 0 )
			_big_num_deref(last_c) = self->table[0] & 1;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<self->size ; ++i)
			self->table[i] = mask;

		_big_num_deref(rest_bits) = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < self->size
		ssize_t first, second;
		_big_num_deref(last_c) = self->table[self->size - all_words] & 1; // all_words is greater than 0

		// copying the first part of the value
		for(first = _big_num_ssize_t(self->size)-1, second=first-all_words ; second>=0 ; --first, --second)
			self->table[first] = self->table[second];

		// setting the rest to 'c'
		for( ; first>=0 ; --first )
			self->table[first] = mask;
	}
}

/**
 * an auxiliary method for moving bits into the right hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param[in] rest_bits 
 * @param[in] last_c 
 * @param[in] bits 
 * @param[in] c 
 */
_big_num_static void _big_uint_rcr_move_all_words(_big_num_inout(big_uint_t, self), _big_num_out(size_t, rest_bits), _big_num_out(big_num_carry_t, last_c), size_t bits, big_num_carry_t c)
{
	_big_num_deref(rest_bits)   = bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 			= _big_num_ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask 		= ( c != 0) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= self->size ) {
		if( all_words == self->size && _big_num_deref(rest_bits) == 0 )
			_big_num_deref(last_c) = (self->table[self->size-1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<self->size ; ++i)
			self->table[i] = mask;

		_big_num_deref(rest_bits) = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < self->size

		ssize_t first, second;
		_big_num_deref(last_c) = (self->table[all_words - 1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0; // all_words is > 0

		// copying the first part of the value
		for(first=0, second=all_words ; second<self->size ; ++first, ++second)
			self->table[first] = self->table[second];

		// setting the rest to 'c'
		for( ; first<self->size ; ++first )
			self->table[first] = mask;
	}
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t carry
 */
_big_num_static big_num_carry_t _big_uint_mul1(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2))
{
	big_uint_t ss1 = _big_num_deref(self);
	big_uint_set_zero(self);

	for(size_t i=0; i < self->size*BIG_NUM_BITS_PER_UNIT ; ++i) {
		if( big_uint_add(self, self, 0) != 0 ) {
			return 1;
		}

		if( big_uint_rcl(_big_num_ref(ss1), 1, 0) != 0 ) {
			if( big_uint_add(self, ss2, 0) != 0 ) {
				return 1;
			}
		}
	}

	return 0;
}

/**
 * multiplication: result = this * ss2
 * result is twice bigger than 'this' and 'ss2'
 * this method never returns carry		
 * @param[in] self 
 * @param[in] ss2_ 
 * @param[in] result 
 */
_big_num_static void _big_uint_mul1_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2_), _big_num_out(big_uint_t, result))
{
	big_uint_t ss2;
	big_uint_init(_big_num_ref(ss2), 2*self->size);
	size_t i;

	// copying _big_num_deref(self) into result and ss2_ into ss2
	for(i=0 ; i<self->size ; ++i) {
		result->table[i] = self->table[i];
		ss2.table[i]    = ss2_->table[i];
	}

	// cleaning the highest bytes in result and ss2
	for( ; i < ss2.size ; ++i) {
		result->table[i] = 0;
		ss2.table[i]    = 0;
	}

	// multiply
	_big_uint_mul1(result, _big_num_ref(ss2));

	return;
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t 
 */
_big_num_static big_num_carry_t _big_uint_mul2(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2))
{
	big_uint_t result;
	big_uint_init(_big_num_ref(result), 2*self->size);
	size_t i;
	big_num_carry_t c = 0;

	_big_uint_mul2_no_carry(self, ss2, _big_num_ref(result));

	// copying result
	for(i=0 ; i<UINT_PREC ; ++i)
		self->table[i] = result.table[i];

	// testing carry
	for( ; i<result.size ; ++i) {
		if( result.table[i] != 0 ) {
			c = 1;
			break;
		}
	}

	return c;
}

/**
 * multiplication: result = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @param[out] result twice bigger than self and ss2
 */
_big_num_static void _big_uint_mul2_no_carry(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, ss2), _big_num_out(big_uint_t, result))
{
	_big_uint_mul2_no_carry2(self, self->table, ss2->table, result);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[out] result 
 */
_big_num_static void _big_uint_mul2_no_carry2(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t ss1[2*UINT_PREC], _big_num_const_param big_num_strg_t ss2[2*UINT_PREC], _big_num_out(big_uint_t, result))
{
	size_t x1size  = self->size, 	x2size  = self->size;
	size_t x1start = 0,       		x2start = 0;

	if( self->size > 2 ) {	
		// if the ss_size is smaller than or equal to 2
		// there is no sense to set x1size (and others) to another values

		for(x1size=self->size ; x1size>0 && ss1[x1size-1]==0 ; --x1size);
		for(x2size=self->size ; x2size>0 && ss2[x2size-1]==0 ; --x2size);

		for(x1start=0 ; x1start<x1size && ss1[x1start]==0 ; ++x1start);
		for(x2start=0 ; x2start<x2size && ss2[x2start]==0 ; ++x2start);
	}

	_big_uint_mul2_no_carry3(self, ss1, ss2, result, x1start, x1size, x2start, x2size);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[in] result 
 * @param[in] x1start 
 * @param[in] x1size 
 * @param[in] x2start 
 * @param[in] x2size 
 */
_big_num_static void	_big_uint_mul2_no_carry3(_big_num_inout(big_uint_t, self), _big_num_const_param big_num_strg_t ss1[2*UINT_PREC], _big_num_const_param big_num_strg_t ss2[2*UINT_PREC], _big_num_out(big_uint_t, result), size_t x1start, size_t x1size, size_t x2start, size_t x2size)
{
	big_num_strg_t r2, r1;

	big_uint_set_zero(self);

	if( x1size==0 || x2size==0 )
		return;

	for(size_t x1=x1start ; x1<x1size ; ++x1)
	{
		for(size_t x2=x2start ; x2<x2size ; ++x2)
		{
			_big_uint_mul_two_words(ss1[x1], ss2[x2], _big_num_ref(r2), _big_num_ref(r1));
			_big_uint_add_two_uints(result, r2, r1, x2+x1);
		}
	}
}


/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor 
 * @param[out] m 
 * @param[out] n 
 * @param[out] remainder 
 * @return div_std_test_t 
 */
_big_num_static div_std_test_t _big_uint_div_standard_test(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(size_t, m), _big_num_out(size_t, n), _big_num_out(big_uint_t, remainder))
{
	switch( _big_uint_div_calculating_size(self, divisor, m, n) ) {
	case 4: // 'this' is equal divisor
		#ifndef GL_core_profile 
		if( remainder )
		#endif
			big_uint_set_zero(remainder);

		big_uint_set_one(self);
		return BIG_NUM_STD_DIV_NONE;
	case 3: // 'this' is smaller than divisor
		#ifndef GL_core_profile 
		if( remainder )
		#endif
			_big_num_deref(remainder) = _big_num_deref(self); // copy

		big_uint_set_zero(self);
		return BIG_NUM_STD_DIV_NONE;
	case 2: // 'this' is zero
		#ifndef GL_core_profile 
		if( remainder )
		#endif
			big_uint_set_zero(remainder);

		big_uint_set_zero(self);
		return BIG_NUM_STD_DIV_NONE;
	case 1: // divisor is zero
		return BIG_NUM_STD_DIV_ZERO;
	}
	return BIG_NUM_STD_DIV;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor
 * @param[out] m 
 * @param[out] n 
 * @return div_calc_test_t 
 */
_big_num_static div_calc_test_t _big_uint_div_calculating_size(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(size_t, m), _big_num_out(size_t, n))
{
	_big_num_deref(m) = _big_num_deref(n) = self->size-1;

	for( ; _big_num_deref(n)!=0 && divisor->table[_big_num_deref(n)]==0 ; --_big_num_deref(n));

	if( _big_num_deref(n)==0 && divisor->table[_big_num_deref(n)]==0 )
		return BIG_NUM_CALC_DIV_ZERO;

	for( ; _big_num_deref(m)!=0 && self->table[_big_num_deref(m)]==0 ; --_big_num_deref(m));

	if( _big_num_deref(m)==0 && self->table[_big_num_deref(m)]==0 )
		return BIG_NUM_CALC_SELF_ZERO;

	if( _big_num_deref(m) < _big_num_deref(n) )
		return BIG_NUM_CALC_SELF_LST;
	else if( _big_num_deref(m) == _big_num_deref(n) ) {
		ssize_t i;
		for(i = _big_num_ssize_t(_big_num_deref(n)) ; i!=0 && self->table[i]==divisor->table[i] ; --i);
		
		if( self->table[i] < divisor->table[i] )
			return BIG_NUM_CALC_SELF_LST;
		else if (self->table[i] == divisor->table[i] )
			return BIG_NUM_CALC_SELF_EQ;
	}

	return BIG_NUM_CALC_DIV_OK;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
_big_num_static big_num_div_ret_t _big_uint_div1(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder))
{
	size_t m,n;
	div_std_test_t test;

	test = _big_uint_div_standard_test(self, divisor, _big_num_ref(m), _big_num_ref(n), remainder);
	switch (test) {
		case BIG_NUM_STD_DIV_NONE: return BIG_NUM_DIV_OK;
		case BIG_NUM_STD_DIV_ZERO: return BIG_NUM_DIV_ZERO;
		default: break;
	}

	#ifndef GL_core_profile
	if( !remainder ) {
		big_uint_t rem;
		big_uint_init(_big_num_ref(rem), self->size);
		return _big_uint_div1_calculate(self, divisor, _big_num_ref(rem));
	}
	#endif

	return _big_uint_div1_calculate(self, divisor, remainder);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
_big_num_static big_num_div_ret_t _big_uint_div1_calculate(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, rest))
{
	big_num_sstrg_t loop;
	big_num_carry_t c;

	big_uint_set_zero(rest);
	loop = _big_num_sstrg_t(self->size * BIG_NUM_BITS_PER_UNIT);
	c = 0;

	for ( ;; ) {
		for ( ;; ) {
			// div_a
			c = big_uint_rcl(self, 1, c);
			c = big_uint_add(rest, rest, c);
			c = big_uint_sub(rest, divisor,c);

			c = c == 0 ? 1 : 0;
			if (c == 0)
				break; // goto div_d
			
			// div_b
			--loop;
			if (loop > 0)
				continue; // goto div_a

			c = big_uint_rcl(self, 1, c);
			return BIG_NUM_DIV_OK;
		}

		for ( ;; ) {
			// div_d
			--loop;
			if (loop != 0) {
				// div_c
				c = big_uint_rcl(self, 1, c);
				c = big_uint_add(rest, rest, c);
				c = big_uint_add(rest, divisor, 0);

				if (c != 0) {
					// "goto" div_b
					--loop;
					if (loop > 0)
						break; // goto div_a
						
					c = big_uint_rcl(self, 1, c);
					return 0;
				}
				else
					continue; // goto div_d
			}

			c = big_uint_rcl(self, 1, c);
			c = big_uint_add(rest, divisor, 0);

			return BIG_NUM_DIV_OK;
		}
	}
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
_big_num_static big_num_div_ret_t  _big_uint_div2(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder))
{
	size_t bits_diff;
	big_num_div_ret_t status = _big_uint_div2_calculate(self, divisor, remainder, _big_num_ref(bits_diff));
	if( status < BIG_NUM_DIV_BUSY )
		return status;

	if( big_uint_cmp_bigger_equal(self, divisor, -1) ) {
		_big_uint_div2(self, divisor, remainder);
		big_uint_set_bit(self, bits_diff);
	} else {
		#ifndef GL_core_profile
		if( remainder )
		#endif
			_big_num_deref(remainder) = _big_num_deref(self);

		big_uint_set_zero(self);
		big_uint_set_bit(self, bits_diff);
	}

	return BIG_NUM_DIV_OK;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @param[out] bits_diff 
 * @return big_num_div_ret_t 
 */
_big_num_static big_num_div_ret_t _big_uint_div2_calculate(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), _big_num_out(size_t, bits_diff))
{
	size_t table_id, index;
	size_t divisor_table_id, divisor_index;

	big_num_div_ret_t status = _big_uint_div2_find_leading_bits_and_check(self,
									divisor, remainder,
									_big_num_ref(table_id), _big_num_ref(index),
									_big_num_ref(divisor_table_id), _big_num_ref(divisor_index));

	if( status < BIG_NUM_DIV_BUSY )
		return status;
	
	// here we know that 'this' is greater than divisor
	// then 'index' is greater or equal 'divisor_index'
	_big_num_deref(bits_diff) = index - divisor_index;

	big_uint_t divisor_copy = _big_num_deref(divisor);
	big_uint_rcl(_big_num_ref(divisor_copy), _big_num_deref(bits_diff), 0);

	if( big_uint_cmp_smaller(self, _big_num_ref(divisor_copy), _big_num_ssize_t(table_id)) )
	{
		big_uint_rcr(_big_num_ref(divisor_copy), 1, 0);
		--(_big_num_deref(bits_diff));
	}

	big_uint_sub(self, _big_num_ref(divisor_copy), 0);

	return BIG_NUM_DIV_BUSY;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @param[out] table_id 
 * @param[out] index 
 * @param[out] divisor_table_id 
 * @param[out] divisor_index 
 * @return big_num_div_ret_t 
 */
_big_num_static big_num_div_ret_t _big_uint_div2_find_leading_bits_and_check(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), _big_num_out(size_t, table_id), _big_num_out(size_t, index), _big_num_out(size_t, divisor_table_id), _big_num_out(size_t, divisor_index))
{
	if(false == big_uint_find_leading_bit(divisor, divisor_table_id, divisor_index) ) {
		// division by zero
		return BIG_NUM_DIV_ZERO;
	}

	if(	!big_uint_find_leading_bit(self, table_id, index) ) {
		// zero is divided by something
		big_uint_set_zero(self);

		#ifndef GL_core_profile
		if( remainder )
		#endif
			big_uint_set_zero(remainder);

		return BIG_NUM_DIV_OK;
	}

	_big_num_deref(divisor_index) += _big_num_deref(divisor_table_id) * BIG_NUM_BITS_PER_UNIT;
	_big_num_deref(index)         += _big_num_deref(table_id)         * BIG_NUM_BITS_PER_UNIT;

	if( divisor_table_id == 0 ) {
		// dividor has only one 32-bit word

		big_num_strg_t r;
		big_uint_div_int(self, divisor->table[0], _big_num_ref(r));

		#ifndef GL_core_profile
		if( remainder ) 
		#endif
		{
			big_uint_set_zero(remainder);
			remainder->table[0] = r;
		}

		return BIG_NUM_DIV_OK;
	}


	if( _big_uint_div2_divisor_greater_or_equal(self,
			divisor, remainder,
			_big_num_deref(table_id), _big_num_deref(index),
			_big_num_deref(divisor_index)) ) {
		return BIG_NUM_DIV_OK;
	}

	return BIG_NUM_DIV_BUSY;
}

/**
 * Checks if divisor is greater than self
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number), set to self if divisor > self
 * @param[in] table_id 
 * @param[in] index 
 * @param[in] divisor_index 
 * @return true divisor is equal or greater than self
 * @return false divisor is less than self
 */
_big_num_static bool _big_uint_div2_divisor_greater_or_equal(_big_num_inout(big_uint_t, self), _big_num_const_param _big_num_inout(big_uint_t, divisor), _big_num_out(big_uint_t, remainder), size_t table_id, size_t index, size_t divisor_index)
{
	if( divisor_index > index ) {
		// divisor is greater than self

		#ifndef GL_core_profile
		if( remainder )
		#endif
			_big_num_deref(remainder) = _big_num_deref(self);

		big_uint_set_zero(self);

		return true;
	}

	if( divisor_index == index ) {
		// table_id == divisor_table_id as well

		ssize_t i;
		for(i = _big_num_ssize_t(table_id) ; i!=0 && self->table[i]==divisor->table[i] ; --i);
		
		if( self->table[i] < divisor->table[i] )
		{
			// divisor is greater than 'this'

			#ifndef GL_core_profile
			if( remainder )
			#endif
				_big_num_deref(remainder) = _big_num_deref(self);

			big_uint_set_zero(self);

			return true;
		} else if( self->table[i] == divisor->table[i] ) {
			// divisor is equal 'this'

			#ifndef GL_core_profile
			if( remainder )
			#endif
				big_uint_set_zero(remainder);

			big_uint_set_one(self);

			return true;
		}
	}

	return false;
}

#if defined(__cplusplus) && !defined(GL_core_profile)
}
#endif