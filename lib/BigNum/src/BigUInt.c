/**
 * @file BigUInt.c
 * @author Imre Korf (I.korf@outlook.com)
 * @date 2023-10-01
 * 
 * @copyright 2023 Imre Korf 
 */

#include "BigNum/BigUInt.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TABLE_SIZE 	BIG_NUM_PREC_UINT
#define BIG_TABLE_SIZE (2*TABLE_SIZE)

#define BIG_NUM_KARATSUBA_MULTIPLICATION_FROM_SIZE 3

// used for MulXBigN functions
typedef struct {
	big_num_strg_t table[2*BIG_NUM_PREC_UINT];
} _big_big_uint_t;

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

// protected functions
big_num_carry_t			_big_uint_sub_uint(big_uint_t* self, big_num_strg_t value, size_t index);
big_num_carry_t			_big_uint_add_uint(big_uint_t* self, big_num_strg_t value, size_t index);
big_num_carry_t 		_big_uint_add_two_uints(big_uint_t* self, big_num_strg_t x2, big_num_strg_t x1, size_t index);

// private functions
static big_num_carry_t 	_big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, big_num_strg_t * result);
static big_num_carry_t 	_big_uint_add_vector(const big_num_strg_t* ss1, const big_num_strg_t* ss2, size_t ss1_size, size_t ss2_size, big_num_strg_t * result);
static big_num_carry_t	_big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, big_num_strg_t * result);
static big_num_carry_t 	_big_uint_sub_vector(const big_num_strg_t* ss1, const big_num_strg_t* ss2, size_t ss1_size, size_t ss2_size, big_num_strg_t * result);
static big_num_carry_t	_big_uint_rcl2_one(big_uint_t* self, big_num_strg_t c);
static big_num_carry_t 	_big_uint_rcr2_one(big_uint_t* self, big_num_strg_t c);
static big_num_carry_t	_big_uint_rcl2(big_uint_t* self, size_t bits, big_num_strg_t c);
static big_num_carry_t	_big_uint_rcr2(big_uint_t* self, size_t bits, big_num_strg_t c);
static big_num_sstrg_t  _big_uint_find_leading_bit_in_word(big_num_strg_t x);
static big_num_sstrg_t 	_big_uint_find_lowest_bit_in_word(big_num_strg_t x);
static big_num_strg_t 	_big_uint_set_bit_in_word(big_num_strg_t * value, size_t bit);
static void 			_big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t* result_high, big_num_strg_t* result_low);
static void				_big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, big_num_strg_t* r, big_num_strg_t* rest);


static void 			_big_uint_rcl_move_all_words(big_uint_t* self, size_t * rest_bits, big_num_carry_t * last_c, size_t bits, big_num_strg_t c);
static void 			_big_uint_rcr_move_all_words(big_uint_t* self, size_t * rest_bits, big_num_carry_t * last_c, size_t bits, big_num_strg_t c);

static big_num_carry_t	_big_uint_mul1_ref(big_uint_t* self, const big_uint_t ss2);
static big_num_carry_t 	_big_uint_mul1(big_uint_t* self, const big_uint_t ss2);
static big_num_carry_t 	_big_uint_mul2(big_uint_t* self, const big_uint_t ss2);
static void 			_big_uint_mul2_big(big_uint_t* self, const big_uint_t ss2, _big_big_uint_t * result);
static void 			_big_uint_mul2_big2(big_uint_t* self, const big_num_strg_t ss1[], const big_num_strg_t ss2[], _big_big_uint_t * result);
static void				_big_uint_mul2_big3(big_uint_t* self, const big_num_strg_t ss1[], const big_num_strg_t ss2[], _big_big_uint_t * result, size_t x1start, size_t x1size, size_t x2start, size_t x2size);

static div_std_test_t	_big_uint_div_standard_test(big_uint_t* self, const big_uint_t divisor, size_t * m, size_t * n, big_uint_t * remainder);
static div_calc_test_t	_big_uint_div_calculating_size(big_uint_t* self, const big_uint_t divisor, size_t * m, size_t * n);
static big_num_div_ret_t 	_big_uint_div1(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder);
static big_num_div_ret_t	_big_uint_div1_calculate(big_uint_t* self, const big_uint_t divisor, big_uint_t * rest);
static big_num_div_ret_t	_big_uint_div1_calculate_ref(big_uint_t* self, const big_uint_t divisor, big_uint_t * rest);
static big_num_div_ret_t 	_big_uint_div2(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder);
static big_num_div_ret_t	_big_uint_div2_ref(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder);
static big_num_div_ret_t	_big_uint_div2_calculate(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, size_t * bits_diff);
static big_num_div_ret_t	_big_uint_div2_find_leading_bits_and_check(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder,size_t * table_id, size_t * index, size_t * divisor_table_id, size_t * divisor_index);
static bool 			_big_uint_div2_divisor_greater_or_equal(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, size_t table_id, size_t index, size_t divisor_index);


/**
 * this method returns the size of the table
 * @param[in, out] self the big num object
 * @return size_t the size of the table
 */
size_t big_uint_size(big_uint_t* self)
{
	return TABLE_SIZE;
}

/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_uint_set_zero(big_uint_t* self)
{
	for(size_t i=0 ; i < TABLE_SIZE ; ++i)
		self->table[i] = 0;
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_uint_set_one(big_uint_t* self)
{
	big_uint_set_zero(self);
	self->table[0] = 1;
}

/**
 * this method sets the max value which self class can hold
 * @param[in, out] self the big num object
 */
void big_uint_set_max(big_uint_t* self)
{
	for(size_t i=0 ; i < TABLE_SIZE; ++i)
		self->table[i] = BIG_NUM_MAX_STRG;
}

/**
 * this method sets the min value which self class can hold
 * (for an unsigned integer value the zero is the smallest value)
 * @param[in, out] self the big num object
 */
void big_uint_set_min(big_uint_t* self)
{
	big_uint_set_zero(self);
}

/**
 * this method swappes self and ss2
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the value to swap with self, ss2 contains self after swap
 */
void big_uint_swap(big_uint_t* self, big_uint_t* ss2)
{
	for(size_t i=0 ; i < TABLE_SIZE ; ++i) {
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
void big_uint_set_from_table(big_uint_t* self, const big_num_strg_t temp_table[], size_t temp_table_len)
{
	size_t temp_table_index = 0;
	ssize_t i; // 'i' with a sign

	for(i=TABLE_SIZE-1 ; i>=0 && temp_table_index<temp_table_len; --i, ++temp_table_index)
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
			if( self->table[0] != BIG_NUM_MAX_STRG )
				++(self->table)[0];
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
big_num_carry_t big_uint_add_uint(big_uint_t* self, big_num_strg_t val)
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
big_num_carry_t big_uint_add(big_uint_t* self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;	

	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_add_two_words(self->table[i], ss2.table[i], c, &self->table[i]);

	return c;
}

/**
 * this method subtracts val from the existing value
 * @param[in, out] self the big num object
 * @param[in] val the value to subtract from self
 * @return big_num_carry_t the carry, if there was one
 */
big_num_carry_t	big_uint_sub_uint(big_uint_t* self, big_num_strg_t val)
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
big_num_carry_t big_uint_sub(big_uint_t* self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;
	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_sub_two_words(self->table[i], ss2.table[i], c, &self->table[i]);

	return c;
}

/**
 * moving all bits into the left side 'bits' times
 * return value <- self <- C
 * 
 * bits is from a range of <0, man * BIG_NUM_BITS_PER_STRG>
 * or it can be even bigger then all bits will be set to 'c'
 * 
 * the value c will be set into the lowest bits
 * and the method returns state of the last moved bit
 * @param[in, out] self the big num object
 * @param[in] bits 
 * @param[in] c 
 * @return big_num_strg_t 
 */
big_num_strg_t big_uint_rcl(big_uint_t* self, size_t bits, big_num_carry_t c)
{
	big_num_strg_t last_c   = 0;
	size_t rest_bits 		= bits;

	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_STRG )
		_big_uint_rcl_move_all_words(self, &rest_bits, &last_c, bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to BIG_NUM_BITS_PER_STRG-1 now
	if( rest_bits == 1 ) {
		last_c = _big_uint_rcl2_one(self, c);
	} else if( rest_bits == 2 ) {
		// performance tests showed that for rest_bits==2 it's better to use _big_uint_rcl2_one twice instead of _big_uint_rcl2(2,c)
		_big_uint_rcl2_one(self, c);
		last_c = _big_uint_rcl2_one(self, c);
	}
	else {
		last_c = _big_uint_rcl2_one(self, c);
	}
	
	return last_c;
}

/**
 * moving all bits into the right side 'bits' times
 * c -> self -> return value
 * 
 * @param[in, out] self the big num object
 * @param[in] bits number of bits to shift, range of <0, man * BIG_NUM_BITS_PER_STRG>, or it can be even bigger then all bits will be set to 'c'
 * @param[in] c bit to insert in MSb
 * @return big_num_strg_t state of the last moved bit
 */
big_num_strg_t big_uint_rcr(big_uint_t* self, size_t bits, big_num_carry_t c)
{
	big_num_carry_t last_c    = 0;
	size_t rest_bits = bits;
	
	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_STRG )
		_big_uint_rcr_move_all_words(self, &rest_bits, &last_c, bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to TTMATH_BITS_PER_UINT-1 now
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
size_t big_uint_compensation_to_left(big_uint_t* self)
{
	size_t moving = 0;

	// a - index a last word which is different from zero
	ssize_t a;
	for(a=TABLE_SIZE-1 ; a>=0 && self->table[a]==0 ; --a);

	if( a < 0 )
		return moving; // all words in table have zero

	if( a != TABLE_SIZE-1 )
	{
		moving += ( TABLE_SIZE-1 - a ) * BIG_NUM_BITS_PER_STRG;

		// moving all words
		ssize_t i;
		for(i=TABLE_SIZE-1 ; a>=0 ; --i, --a)
			self->table[i] = self->table[a];

		// setting the rest word to zero
		for(; i>=0 ; --i)
			self->table[i] = 0;
	}

	size_t moving2 = _big_uint_find_leading_bit_in_word( self->table[TABLE_SIZE-1] );
	// moving2 is different from -1 because the value table[TABLE_SIZE-1]
	// is not zero

	moving2 = BIG_NUM_BITS_PER_STRG - moving2 - 1;
	big_uint_rcl(self, moving2, 0);

	return moving + moving2;
}

/**
 * this method looks for the highest set bit
 * @param[in, out] self the big num object
 * @param[out] table_id 'self' != 0: the index of a word <0..value_size-1>, else 0
 * @param[out] index    'self' != 0: the index of self set bit in the word <0..BIG_NUM_BITS_PER_STRG), else 0
 * @return true: 	'self' is not zero
 * @return false: 	'self' is zero, both 'table_id' and 'index' are zero
 */
bool big_uint_find_leading_bit(big_uint_t self, size_t * table_id, size_t * index)
{
	for(*table_id=TABLE_SIZE-1 ; *table_id!=0 && self.table[*table_id]==0 ; --(*table_id));

	if( *table_id==0 && self.table[*table_id]==0 )
	{
		// is zero
		index = 0;

		return false;
	}
	
	// table[table_id] is different from 0
	*index = _big_uint_find_leading_bit_in_word( self.table[*table_id] );

	return true;
}

/**
 * this method looks for the smallest set bit
 * @param[in, out] self the big num object
 * @param[out] table_id 'self' != 0: the index of a word <0..value_size-1>, else 0
 * @param[out] index 'self' != 0: the index of self set bit in the word <0..BIG_NUM_BITS_PER_STRG), else 0
 * @return true: self' is not zero
 * @return false: both 'table_id' and 'index' are zero
 */
bool big_uint_find_lowest_bit(big_uint_t self, size_t * table_id, size_t * index)
{
	for(*table_id=0 ; *table_id<TABLE_SIZE && self.table[*table_id]==0 ; ++(*table_id));

		if( *table_id >= TABLE_SIZE )
		{
			// is zero
			*index    = 0;
			*table_id = 0;

			return false;
		}
		
		// table[table_id] is different from 0
		*index = _big_uint_find_lowest_bit_in_word( self.table[*table_id] );

	return true;
}

/**
 * getting the 'bit_index' bit
 * 
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the state of the bit
 */
bool big_uint_get_bit(big_uint_t self, size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_STRG;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_STRG;

	big_num_strg_t temp = self.table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(&temp, bit);

	return res;
}

/**
 * setting the 'bit_index' bit
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the last state of the bit 
 */
bool big_uint_set_bit(big_uint_t* self, size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_STRG;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_STRG;

	big_num_strg_t temp = self->table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(&temp, bit);

	return res;
}

/**
 * this method performs a bitwise operation AND 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_and(big_uint_t* self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self->table[x] &= ss2.table[x];
}

/**
 * this method performs a bitwise operation OR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_or(big_uint_t* self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self->table[x] |= ss2.table[x];
}

/**
 * this method performs a bitwise operation XOR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_xor(big_uint_t* self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self->table[x] ^= ss2.table[x];
}

/**
 * this method performs a bitwise operation NOT
 * @param[in, out] self the big num object
 */
void big_uint_bit_not(big_uint_t* self)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
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
void big_uint_bit_not2(big_uint_t* self)
{
	size_t table_id, index;

	if( big_uint_find_leading_bit(*self, &table_id, &index) ) {
		for(size_t x=0 ; x<table_id ; ++x)
			self->table[x] = ~(self->table[x]);

		big_num_strg_t mask  = BIG_NUM_MAX_STRG;
		size_t shift = BIG_NUM_BITS_PER_STRG - index - 1;

		if(shift)
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
big_num_carry_t big_uint_mul_int(big_uint_t* self, big_num_strg_t ss2)
{
	big_num_strg_t r1, r2;
	size_t x1;
	big_num_carry_t c = 0;

	big_uint_t u = *self;
	big_uint_set_zero(self);

	if( ss2 == 0 ) {
		return 0;
	}

	for(x1=0 ; x1<TABLE_SIZE-1 ; ++x1) {
		_big_uint_mul_two_words(u.table[x1], ss2, &r2, &r1);
		c += _big_uint_add_two_uints(self, r2,r1,x1);
	}

	// x1 = value_size-1  (last word)
	_big_uint_mul_two_words(u.table[x1], ss2, &r2, &r1);
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
big_num_carry_t big_uint_mul(big_uint_t* self, const big_uint_t ss2, big_num_algo_t algorithm)
{
	switch (algorithm) {
		case BIG_NUM_MUL1: return _big_uint_mul1(self, ss2); break;
		case BIG_NUM_MUL2: return _big_uint_mul2(self, ss2); break;
		default: _big_uint_mul1(self, ss2); break;
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
big_num_div_ret_t big_uint_div_int(big_uint_t* self, big_num_strg_t divisor, big_num_strg_t * remainder)
{
	if( divisor == 0 ) {
		if( remainder )
			*remainder = 0; // this is for convenience, without it the compiler can report that 'remainder' is uninitialized

		return 1;
	}

	if( divisor == 1 ) {
		if( remainder )
			*remainder = 0;

		return 0;
	}

	big_uint_t dividend = *self;
	big_uint_set_zero(self);
	
	big_num_sstrg_t i;  // i must be with a sign
	big_num_strg_t r = 0;

	// we're looking for the last word in ss1
	for(i=TABLE_SIZE-1 ; i>0 && dividend.table[i]==0 ; --i);

	for( ; i>=0 ; --i)
		_big_uint_div_two_words(r, dividend.table[i], divisor, &self->table[i], &r);

	if( remainder )
		*remainder = r;

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
big_num_div_ret_t big_uint_div(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, big_num_algo_t algorithm)
{
	switch( algorithm ){
		case 1:
			return _big_uint_div1(self, divisor, remainder);

		case 2:
		default:
			return _big_uint_div2(self, divisor, remainder);
	}
}

/**
 * power self = self ^ pow
 * binary algorithm (r-to-l)
 * 
 * @param[in, out] self the big num object
 * @param[in] pow The power to raise self to
 * @return big_num_pow_ret_t 
 */
big_num_pow_ret_t big_uint_pow(big_uint_t* self, big_uint_t pow)
{
	if(big_uint_is_zero(pow) && big_uint_is_zero(*self))
		// we don't define zero^zero
		return BIG_NUM_POW_INVALID;

	big_uint_t start = *self;
	big_uint_t result;
	big_uint_set_one(&result);
	big_num_carry_t c = 0;

	while( !c )
	{
		if( pow.table[0] & 1 )
			c += big_uint_mul(&result, start, BIG_NUM_MUL_DEF);

		_big_uint_rcr2_one(&pow, 0);
		if( big_uint_is_zero(pow) )
			break;

		c += big_uint_mul(&start, start, BIG_NUM_MUL_DEF);
	}

	*self = result;

	return (c==0)? 0 : 1;
}

/**
 * square root
 * e.g. Sqrt(9) = 3
 * ('digit-by-digit' algorithm)
 * @param[in, out] self the big num object
 */
void big_uint_sqrt(big_uint_t* self)
{
	big_uint_t bit, temp;

	if( big_uint_is_zero(*self) )
		return;

	big_uint_t value = *self;

	big_uint_set_zero(self);
	big_uint_set_zero(&bit);
	bit.table[TABLE_SIZE-1] = (BIG_NUM_HIGHEST_BIT >> 1);
	
	while( big_uint_cmp_bigger(bit, value, -1))
		big_uint_rcr(&bit, 2, 0);

	while( !big_uint_is_zero(bit) ) {
		temp = *self;
		big_uint_add(&temp, bit, 0);

		if( big_uint_cmp_bigger_equal(value, temp, -1) ) {
			big_uint_sub(&value, temp, 0);
			big_uint_rcr(self, 1, 0);
			big_uint_add(self, bit, 0);
		}
		else {
			big_uint_rcr(self, 1, 0);
		}

		big_uint_rcr(&bit, 2, 0);
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
void big_uint_clear_first_bits(big_uint_t* self, size_t n)
{
	if( n >= TABLE_SIZE*BIG_NUM_BITS_PER_STRG ) {
		big_uint_set_zero(self);
		return;
	}

	big_num_strg_t * p = self->table;

	// first we're clearing the whole words
	while( n >= BIG_NUM_BITS_PER_STRG ) {
		*p++ = 0;
		n   -= BIG_NUM_BITS_PER_STRG;
	}

	if( n == 0 ) {
		return;
	}

	// and then we're clearing one word which has left
	// mask -- all bits are set to one
	big_num_strg_t mask = BIG_NUM_MAX_STRG;

	mask = mask << n;

	(*p) &= mask;
}

/**
 * this method returns true if the highest bit of the value is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_the_highest_bit_set(big_uint_t self)
{
	return (self.table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0;
}

/**
 * this method returns true if the lowest bit of the value is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_the_lowest_bit_set(big_uint_t self)
{
	return (self.table[0] & 1) != 0;
}

/**
 * returning true if only the highest bit is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_only_the_highest_bit_set(big_uint_t self)
{
	for(size_t i=0 ; i<TABLE_SIZE-1 ; ++i)
		if( self.table[i] != 0 )
			return false;
	
	if( self.table[TABLE_SIZE-1] != BIG_NUM_HIGHEST_BIT )
		return false;
	
	return true;
}

/**
 * returning true if only the lowest bit is set
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_only_the_lowest_bit_set(big_uint_t self)
{
	if( self.table[0] != 1 )
		return false;

	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		if( self.table[i] != 0 )
			return false;

	return true;
}

/**
 * this method returns true if the value is equal zero
 * @param[in, out] self the big num object
 * @return true 
 * @return false 
 */
bool big_uint_is_zero(big_uint_t self)
{
	for(size_t i=0 ; i<TABLE_SIZE ; ++i)
		if(self.table[i] != 0)
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
bool big_uint_are_first_bits_zero(big_uint_t self, size_t bits)
{
	size_t index = bits / BIG_NUM_BITS_PER_STRG;
	size_t rest  = bits % BIG_NUM_BITS_PER_STRG;
	size_t i;

	for(i=0 ; i<index ; ++i)
		if( self.table[i] != 0 )
			return false;

	if( rest == 0 )
		return true;

	big_num_strg_t mask = BIG_NUM_MAX_STRG >> (BIG_NUM_BITS_PER_STRG - rest);

	return (self.table[i] & mask) == 0;
}

/*!
 *
 *	initialization methods
 *
 */

/**
 * a constructor for converting the big_num_strg_t big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 */
void big_uint_init_uint(big_uint_t* self, big_num_strg_t value)
{
	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		self->table[i] = 0;

	self->table[0] = value;
}

/**
 * a constructor for converting big_num_lstrg_t int to big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_ulint(big_uint_t* self, big_num_lstrg_t value)
{
	self->table[0] = (big_num_strg_t)value;

	if( TABLE_SIZE == 1 )
	{
		big_num_carry_t c = ((value >> BIG_NUM_BITS_PER_STRG) == 0) ? 0 : 1;

		return c;
	}

	self->table[1] = (big_num_strg_t)(value >> BIG_NUM_BITS_PER_STRG);

	for(size_t i=2 ; i<TABLE_SIZE ; ++i)
		self->table[i] = 0;

	return 0;
}

/**
 * a copy constructor
 * @param[in, out] self the big num object
 * @param[in] u the other big num object
 */
void big_uint_init_big_uint(big_uint_t* self, const big_uint_t value)
{
	for(size_t i=0 ; i<TABLE_SIZE ; ++i)
		self->table[i] = value.table[i];
}

/**
 * a constructor for converting the big_num_sstrg_t to big_uint_t
 * 
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_int(big_uint_t* self, big_num_sstrg_t value)
{
	big_uint_init_uint(self, (big_num_strg_t)value);
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
big_num_carry_t big_uint_init_lint(big_uint_t* self, big_num_lsstrg_t value)
{
	big_num_carry_t c = big_uint_init_ulint(self, (big_num_lstrg_t)value);
	if (c || value < 0)
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
big_num_carry_t big_uint_to_uint(big_uint_t self, big_num_strg_t * result)
{
	*result = self.table[0];

	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		if( self.table[i] != 0 )
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
big_num_carry_t big_uint_to_int(big_uint_t self, big_num_sstrg_t * result)
{
	return big_uint_to_uint(self, result);
}

/**
 * this method converts the value to big_num_lstrg_t type (long integer)
 * can return a carry if the value is too long to store it in big_num_lstrg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t 
 */
big_num_carry_t	big_uint_to_luint(big_uint_t self, big_num_lstrg_t * result)
{
	*result = self.table[0];
	*result += ((big_num_lstrg_t)self.table[1]) << BIG_NUM_BITS_PER_STRG;

	for(size_t i=2 ; i<TABLE_SIZE ; ++i)
		if( self.table[i] != 0 )
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
big_num_carry_t	big_uint_to_lint(big_uint_t self, big_num_lsstrg_t * result)
{
	return big_uint_to_luint(self, result);
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
bool big_uint_cmp_smaller(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index >= (big_num_sstrg_t)TABLE_SIZE )
		i = TABLE_SIZE - 1;
	else
		i = index;


	for( ; i>=0 ; --i)
	{
		if( self.table[i] != l.table[i] )
			return self.table[i] < l.table[i];
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
bool big_uint_cmp_bigger(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>=(big_num_sstrg_t)TABLE_SIZE )
		i = TABLE_SIZE - 1;
	else
		i = index;


	for( ; i>=0 ; --i)
	{
		if( self.table[i] != l.table[i] )
			return self.table[i] > l.table[i];
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
bool big_uint_cmp_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>=(big_num_sstrg_t)TABLE_SIZE )
		i = TABLE_SIZE - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self.table[i] != l.table[i] )
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
bool big_uint_cmp_smaller_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>=(big_num_sstrg_t)TABLE_SIZE )
		i = TABLE_SIZE - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self.table[i] != l.table[i] )
			return self.table[i] < l.table[i];
	
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
bool big_uint_cmp_bigger_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>=(big_num_sstrg_t)TABLE_SIZE )
		i = TABLE_SIZE - 1;
	else
		i = index;

	for( ; i>=0 ; --i)
		if( self.table[i] != l.table[i] )
			return self.table[i] > l.table[i];
	
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
static big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, big_num_strg_t * result)
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

	*result = temp;

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
big_num_carry_t _big_uint_add_uint(big_uint_t* self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self->table[index], value, 0, &self->table[index]);

	for(i=index+1 ; i < TABLE_SIZE && c ; ++i)
		c = _big_uint_add_two_words(self->table[i], 0, c, &self->table[i]);
	
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
big_num_carry_t _big_uint_add_two_uints(big_uint_t* self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self->table[index],   x1, 0, &self->table[index]);
	c = _big_uint_add_two_words(self->table[index+1], x2, c, &self->table[index+1]);

	for(i=index+2 ; i<TABLE_SIZE && c ; ++i)
		c = _big_uint_add_two_words(self->table[i], 0, c, &self->table[i]);
	
	return c;
}

/**
 * this static method addes one vector to the other
 * 'ss1' is larger in size or equal to 'ss2'
 * -  ss1 points to the first (larger) vector
 * -  ss2 points to the second vector
 * -  ss1_size - size of the ss1 (and size of the result too)
 * -  ss2_size - size of the ss2
 * -  result - is the result vector (which has size the same as ss1: ss1_size)
 * 		Example:  ss1_size is 5, ss2_size is 3
 * 		ss1:      ss2:   result (output):
 * 		  5        1         5+1
 * 		  4        3         4+3
 * 		  2        7         2+7
 * 		  6                  6
 * 		  9                  9
 *  of course the carry is propagated and will be returned from the last item
 *  (this method is used by the Karatsuba multiplication algorithm)
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[in] ss1_size 
 * @param[in] ss2_size 
 * @param[out] result 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_uint_add_vector(const big_num_strg_t* ss1, const big_num_strg_t* ss2, size_t ss1_size, size_t ss2_size, big_num_strg_t * result)
{
	size_t i;
	big_num_carry_t c = 0;

	for(i=0 ; i<ss2_size ; ++i)
		c = _big_uint_add_two_words(ss1[i], ss2[i], c, &result[i]);

	for( ; i<ss1_size ; ++i)
		c = _big_uint_add_two_words(ss1[i], 0, c, &result[i]);

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
static big_num_carry_t _big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, big_num_strg_t * result)
{
	if( carry == 0 ) {
		*result = a - b;

		if( a < b )
			carry = 1;
	} else {
		carry   = 1;
		*result = a - b - carry;

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
big_num_carry_t _big_uint_sub_uint(big_uint_t* self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_sub_two_words(self->table[index], value, 0, &self->table[index]);

	for(i=index+1 ; i<TABLE_SIZE && c ; ++i)
		c = _big_uint_sub_two_words(self->table[i], 0, c, &self->table[i]);

	return c;
}

/**
 * this static method subtractes one vector from the other
 * 'ss1' is larger in size or equal to 'ss2'
 * -  ss1 points to the first (larger) vector
 * -  ss2 points to the second vector
 * -  ss1_size - size of the ss1 (and size of the result too)
 * -  ss2_size - size of the ss2
 * -  result - is the result vector (which has size the same as ss1: ss1_size)
 * 		Example:  ss1_size is 5, ss2_size is 3
 * 		ss1:      ss2:   result (output):
 * 		  5        1         5-1
 * 		  4        3         4-3
 * 		  2        7         2-7
 * 		  6                  6-1  (the borrow from previous item)
 * 		  9                  9
 * 	               	 return (carry): 0
 *  of course the carry (borrow) is propagated and will be returned from the last item
 *  (this method is used by the Karatsuba multiplication algorithm)
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[in] ss1_size 
 * @param[in] ss2_size 
 * @param[out] result 
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_uint_sub_vector(const big_num_strg_t* ss1, const big_num_strg_t* ss2, size_t ss1_size, size_t ss2_size, big_num_strg_t * result)
{
	size_t i;
	big_num_carry_t c = 0;

	for(i=0 ; i<ss2_size ; ++i)
			c = _big_uint_sub_two_words(ss1[i], ss2[i], c, &result[i]);

	for( ; i<ss1_size ; ++i)
		c = _big_uint_sub_two_words(ss1[i], 0, c, &result[i]);

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
static big_num_carry_t _big_uint_rcl2_one(big_uint_t* self, big_num_carry_t c)
{
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = 1;

	for(i=0 ; i<TABLE_SIZE ; ++i) {
		new_c    		= (self->table[i] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
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
static big_num_carry_t _big_uint_rcr2_one(big_uint_t* self, big_num_carry_t c)
{
	big_num_sstrg_t i; // signed i
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_HIGHEST_BIT;

	for(i=(big_num_sstrg_t)TABLE_SIZE-1 ; i>=0 ; --i) {
		new_c    		= (self->table[i] & 1) ? BIG_NUM_HIGHEST_BIT : 0;
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
static big_num_carry_t _big_uint_rcl2(big_uint_t* self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_STRG - bits;
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_STRG >> move;

	for(i=0 ; i<TABLE_SIZE ; ++i) {
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
static big_num_carry_t _big_uint_rcr2(big_uint_t* self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_STRG - bits;
	size_t i; // signed
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_STRG << move;

	for(i=TABLE_SIZE-1 ; i>=0 ; --i) {
		new_c    		= self->table[i] << move;
		self->table[i] 	= (self->table[i] >> bits) | c;
		c        		= new_c;
	}

	c = (c & BIG_NUM_HIGHEST_BIT) ? 1 : 0;

	return c;
}

/**
 * this method returns the number of the highest set bit in x
 * @param[in] x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
static big_num_sstrg_t _big_uint_find_leading_bit_in_word(big_num_strg_t x)
{
	if( x == 0 )
		return -1;

	big_num_sstrg_t bit = BIG_NUM_BITS_PER_STRG - 1;
	
	while( (x & BIG_NUM_HIGHEST_BIT) == 0 ) {
		x = x << 1;
		--bit;
	}

	return bit;
}

/**
 * this method returns the number of the highest set bit in x
 * @param[in] x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
static big_num_sstrg_t _big_uint_find_lowest_bit_in_word(big_num_strg_t x)
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
 * @param[in] bit the bit to set, between <0,BIG_NUM_BITS_PER_STRG-1>
 * @return big_num_strg_t 
 */
static big_num_strg_t _big_uint_set_bit_in_word(big_num_strg_t * value, size_t bit)
{
	big_num_strg_t mask = 1;

	if( bit > 0 )
		mask = mask << bit;

	big_num_strg_t last = *value & mask;
	*value     	 	    = *value | mask;

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
static void _big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t* result_high, big_num_strg_t* result_low)
{
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

	*result_high = res.u_.high;
	*result_low  = res.u_.low;
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
static void _big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, big_num_strg_t* r, big_num_strg_t* rest)
{
	union {
		struct {
			big_num_strg_t low;  // 1 word
			big_num_strg_t high; // 1 word
		} u_;

		big_num_lstrg_t u;       // 2 words
	} ab;

	ab.u_.high = a;
	ab.u_.low  = b;

	*r    = (big_num_strg_t)(ab.u / c);
	*rest = (big_num_strg_t)(ab.u % c);
}




/**
 * an auxiliary method for moving bits into the left hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param[out] rest_bits 
 * @param[out] last_c 
 * @param[in] bits 
 * @param[in] c carry
 */
static void _big_uint_rcl_move_all_words(big_uint_t* self, size_t * rest_bits, big_num_carry_t * last_c, size_t bits, big_num_carry_t c)
{
	*rest_bits      	= bits % BIG_NUM_BITS_PER_STRG;
	size_t all_words 	= bits / BIG_NUM_BITS_PER_STRG;
	big_num_strg_t mask = ( c ) ? BIG_NUM_MAX_STRG : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && *rest_bits == 0 )
			*last_c = self->table[0] & 1;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<TABLE_SIZE ; ++i)
			self->table[i] = mask;

		*rest_bits = 0;
	}
	else
	if( all_words > 0 ) {
		// 0 < all_words < TABLE_SIZE
		ssize_t first, second;
		*last_c = self->table[TABLE_SIZE - all_words] & 1; // all_words is greater than 0

		// copying the first part of the value
		for(first = TABLE_SIZE-1, second=first-all_words ; second>=0 ; --first, --second)
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
static void _big_uint_rcr_move_all_words(big_uint_t* self, size_t * rest_bits, big_num_carry_t * last_c, size_t bits, big_num_carry_t c)
{
	*rest_bits      		= bits % BIG_NUM_BITS_PER_STRG;
	size_t all_words 	= bits / BIG_NUM_BITS_PER_STRG;
	big_num_strg_t mask = ( c ) ? BIG_NUM_MAX_STRG : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && *rest_bits == 0 )
			*last_c = (self->table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) ? 1 : 0;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<TABLE_SIZE ; ++i)
			self->table[i] = mask;

		*rest_bits = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < TABLE_SIZE

		ssize_t first, second;
		*last_c = (self->table[all_words - 1] & BIG_NUM_HIGHEST_BIT) ? 1 : 0; // all_words is > 0

		// copying the first part of the value
		for(first=0, second=all_words ; second<TABLE_SIZE ; ++first, ++second)
			self->table[first] = self->table[second];

		// setting the rest to 'c'
		for( ; first<TABLE_SIZE ; ++first )
			self->table[first] = mask;
	}
}


/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t carry
 */
static big_num_carry_t _big_uint_mul1_ref(big_uint_t* self, const big_uint_t ss2)
{
	big_uint_t ss1 = *self;
	big_uint_set_zero(self);

	for(size_t i=0; i < TABLE_SIZE*BIG_NUM_BITS_PER_STRG ; ++i) {
		if( big_uint_add(self, *self, 0) ) {
			return 1;
		}

		if( big_uint_rcl(&ss1, 1, 0) ) {
			if( big_uint_add(self, ss2, 0) ) {
				return 1;
			}
		}
	}

	return 0;
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t carry
 */
static big_num_carry_t _big_uint_mul1(big_uint_t* self, const big_uint_t ss2)
{
	return _big_uint_mul1_ref(self, ss2);
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t 
 */
static big_num_carry_t _big_uint_mul2(big_uint_t* self, const big_uint_t ss2)
{
	_big_big_uint_t result;
	size_t i;
	big_num_carry_t c = 0;

	_big_uint_mul2_big(self, ss2, &result);

	// copying result
	for(i=0 ; i<TABLE_SIZE ; ++i)
		self->table[i] = result.table[i];

	// testing carry
	for( ; i<BIG_TABLE_SIZE ; ++i) {
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
static void _big_uint_mul2_big(big_uint_t* self, const big_uint_t ss2, _big_big_uint_t * result)
{
	_big_uint_mul2_big2(self, self->table, ss2.table, result);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[out] result 
 */
static void _big_uint_mul2_big2(big_uint_t* self, const big_num_strg_t ss1[], const big_num_strg_t ss2[], _big_big_uint_t * result)
{
	size_t x1size  = BIG_TABLE_SIZE, 	x2size  = BIG_TABLE_SIZE;
	size_t x1start = 0,       		x2start = 0;

	if( BIG_TABLE_SIZE > 2 )
	{	
		// if the ss_size is smaller than or equal to 2
		// there is no sense to set x1size (and others) to another values

		for(x1size=BIG_TABLE_SIZE ; x1size>0 && ss1[x1size-1]==0 ; --x1size);
		for(x2size=BIG_TABLE_SIZE ; x2size>0 && ss2[x2size-1]==0 ; --x2size);

		for(x1start=0 ; x1start<x1size && ss1[x1start]==0 ; ++x1start);
		for(x2start=0 ; x2start<x2size && ss2[x2start]==0 ; ++x2start);
	}

	_big_uint_mul2_big3(self, ss1, ss2, result, x1start, x1size, x2start, x2size);
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
static void	_big_uint_mul2_big3(big_uint_t* self, const big_num_strg_t ss1[], const big_num_strg_t ss2[], _big_big_uint_t * result, size_t x1start, size_t x1size, size_t x2start, size_t x2size)
{
	big_num_strg_t r2, r1;

	for(size_t i=0 ; i < BIG_TABLE_SIZE; ++i)
		result->table[i] = 0;

	if( x1size==0 || x2size==0 )
		return;

	for(size_t x1=x1start ; x1<x1size ; ++x1)
	{
		for(size_t x2=x2start ; x2<x2size ; ++x2)
		{
			_big_uint_mul_two_words(ss1[x1], ss2[x2], &r2, &r1);
			size_t i;
			big_num_carry_t c;

			c = _big_uint_add_two_words(result->table[x2+x1],   r1, 0, &result->table[x2+x1]);
			c = _big_uint_add_two_words(result->table[x2+x1+1], r2, c, &result->table[x2+x1+1]);

			for(i=x2+x1+2 ; i<BIG_TABLE_SIZE && c ; ++i)
				c = _big_uint_add_two_words(result->table[i], 0, c, &result->table[i]);
			// here will never be a carry
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
static div_std_test_t _big_uint_div_standard_test(big_uint_t* self, const big_uint_t divisor, size_t * m, size_t * n, big_uint_t * remainder){
	switch( _big_uint_div_calculating_size(self, divisor, m, n) ) {
	case 4: // 'this' is equal divisor
		if( remainder )
			big_uint_set_zero(remainder);

		big_uint_set_one(self);
		return BIG_NUM_STD_DIV_NONE;
	case 3: // 'this' is smaller than divisor
		if( remainder )
			*remainder = *self; // copy

		big_uint_set_zero(self);
		return BIG_NUM_STD_DIV_NONE;
	case 2: // 'this' is zero
		if( remainder )
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
static div_calc_test_t _big_uint_div_calculating_size(big_uint_t* self, const big_uint_t divisor, size_t * m, size_t * n){
	*m = *n = TABLE_SIZE-1;

	for( ; *n!=0 && divisor.table[*n]==0 ; --n);

	if( *n==0 && divisor.table[*n]==0 )
		return BIG_NUM_CALC_DIV_ZERO;

	for( ; *m!=0 && self->table[*m]==0 ; --m);

	if( *m==0 && self->table[*m]==0 )
		return BIG_NUM_CALC_SELF_ZERO;

	if( *m < *n )
		return BIG_NUM_CALC_SELF_LST;
	else if( *m == *n ) {
		size_t i;
		for(i = *n ; i!=0 && self->table[i]==divisor.table[i] ; --i);
		
		if( self->table[i] < divisor.table[i] )
			return BIG_NUM_CALC_SELF_LST;
		else if (self->table[i] == divisor.table[i] )
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
static big_num_div_ret_t _big_uint_div1(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder){
	size_t m,n;
	div_std_test_t test;

	test = _big_uint_div_standard_test(self, divisor, &m, &n, remainder);
	switch (test) {
		case BIG_NUM_STD_DIV_NONE: return BIG_NUM_DIV_OK;
		case BIG_NUM_STD_DIV_ZERO: return BIG_NUM_DIV_ZERO;
		default: break;
	}

	if( !remainder ) {
		big_uint_t rem;
		return _big_uint_div1_calculate(self, divisor, &rem);
	}

	return _big_uint_div1_calculate(self, divisor, remainder);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
static big_num_div_ret_t _big_uint_div1_calculate(big_uint_t* self, const big_uint_t divisor, big_uint_t * rest){
	return _big_uint_div1_calculate_ref(self, divisor, rest);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
static big_num_div_ret_t _big_uint_div1_calculate_ref(big_uint_t* self, const big_uint_t divisor, big_uint_t * rest){
	big_num_sstrg_t loop;
	big_num_carry_t c;

	big_uint_set_zero(rest);
	loop = TABLE_SIZE * BIG_NUM_BITS_PER_STRG;
	c = 0;

	for ( ;; ) {
		for ( ;; ) {
			// div_a
			c = big_uint_rcl(self, 1, c);
			c = big_uint_add(rest, *rest, c);
			c = big_uint_sub(rest, divisor,c);

			c = !c;
			if (!c)
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
			if (loop) {
				// div_c
				c = big_uint_rcl(self, 1, c);
				c = big_uint_add(rest, *rest, c);
				c = big_uint_add(rest, divisor, 0);

				if (c) {
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
static big_num_div_ret_t  _big_uint_div2(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder){
	return _big_uint_div2_ref(self, divisor, remainder);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
static big_num_div_ret_t _big_uint_div2_ref(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder){
	size_t bits_diff;
	big_num_div_ret_t status = _big_uint_div2_calculate(self, divisor, remainder, &bits_diff);
	if( status < BIG_NUM_DIV_BUSY )
		return status;

	if( big_uint_cmp_bigger_equal(*self, divisor, -1) ) {
		_big_uint_div2(self, divisor, remainder);
		big_uint_set_bit(self, bits_diff);
	} else {
		if( remainder )
			*remainder = *self;

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
static big_num_div_ret_t _big_uint_div2_calculate(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, size_t * bits_diff)
{
	size_t table_id, index;
	size_t divisor_table_id, divisor_index;

	big_num_div_ret_t status = _big_uint_div2_find_leading_bits_and_check(self,
									divisor, remainder,
									&table_id, &index,
									&divisor_table_id, &divisor_index);

	if( status < BIG_NUM_DIV_BUSY )
		return status;
	
	// here we know that 'this' is greater than divisor
	// then 'index' is greater or equal 'divisor_index'
	*bits_diff = index - divisor_index;

	big_uint_t divisor_copy = divisor;
	big_uint_rcl(&divisor_copy, *bits_diff, 0);

	if( big_uint_cmp_smaller(*self, divisor_copy, table_id) )
	{
		big_uint_rcr(&divisor_copy, 1, 0);
		--(*bits_diff);
	}

	big_uint_sub(self, divisor_copy, 0);

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
static big_num_div_ret_t _big_uint_div2_find_leading_bits_and_check(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, size_t * table_id, size_t * index, size_t * divisor_table_id, size_t * divisor_index)
{
	if(0 == big_uint_find_leading_bit(divisor, divisor_table_id, divisor_index) ) {
		// division by zero
		return BIG_NUM_DIV_ZERO;
	}

	if(	!big_uint_find_leading_bit(*self, table_id, index) ) {
		// zero is divided by something
		big_uint_set_zero(self);

		if( remainder )
			big_uint_set_zero(remainder);

		return BIG_NUM_DIV_OK;
	}

	*divisor_index += *divisor_table_id * BIG_NUM_BITS_PER_STRG;
	*index         += *table_id         * BIG_NUM_BITS_PER_STRG;

	if( divisor_table_id == 0 ) {
		// dividor has only one 32-bit word

		big_num_strg_t r;
		big_uint_div_int(self, divisor.table[0], &r);

		if( remainder ) {
			big_uint_set_zero(remainder);
			remainder->table[0] = r;
		}

		return BIG_NUM_DIV_OK;
	}


	if( _big_uint_div2_divisor_greater_or_equal(self,
			divisor, remainder,
			*table_id, *index,
			*divisor_index) ) {
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
static bool _big_uint_div2_divisor_greater_or_equal(big_uint_t* self, const big_uint_t divisor, big_uint_t * remainder, size_t table_id, size_t index, size_t divisor_index)
{
	if( divisor_index > index ) {
		// divisor is greater than self

		if( remainder )
			*remainder = *self;

		big_uint_set_zero(self);

		return true;
	}

	if( divisor_index == index ) {
		// table_id == divisor_table_id as well

		size_t i;
		for(i = table_id ; i!=0 && self->table[i]==divisor.table[i] ; --i);
		
		if( self->table[i] < divisor.table[i] )
		{
			// divisor is greater than 'this'

			if( remainder )
				*remainder = *self;

			big_uint_set_zero(self);

			return true;
		} else if( self->table[i] == divisor.table[i] ) {
			// divisor is equal 'this'

			if( remainder )
				big_uint_set_zero(remainder);

			big_uint_set_one(self);

			return true;
		}
	}

	return false;
}

#ifdef __cplusplus
}
#endif