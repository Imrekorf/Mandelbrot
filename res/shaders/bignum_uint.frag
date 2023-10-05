#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

// Config

#define BIG_NUM_PREC_EXP 	3 // integer part precision
#define BIG_NUM_PREC_MAN 	3 // floating part precision

#define BIG_NUM_DEF_MUL_ALGO 	BIG_NUM_MUL1
#define BIG_NUM_DEF_DIV_ALGO 	BIG_NUM_DIV1

//===============
// Don't touch
//===============

//===============
// defines from BigNum.h
//===============

#define BIG_NUM_MAX_STRG 			((1<<( 32   ))-1)
#define BIG_NUM_MAX_LSTRG 			((1<<( 64   ))-1)
#define BIG_NUM_MAX_SSTRG 			((1<<((32)-1))-1)
#define BIG_NUM_MAX_LSSTRG 			((1<<((64)-1))-1)
#define BIG_NUM_MIN_SSTRG 			((1<<((32)-1)))
#define BIG_NUM_MIN_LSSTRG 			((1<<((64)-1)))
#define BIG_NUM_HIGHEST_BIT			((1<<( 32   )))
#define BIG_NUM_BITS_PER_STRG		(32)
#define BIG_NUM_BITS_PER_LSTRG		(64)
#define BIG_NUM_BITS_PER_SSTRG		(32)
#define BIG_NUM_BITS_PER_LSSTRG		(64)

#define BIG_NUM_PREC_INT 	BIG_NUM_PREC_EXP
#define BIG_NUM_PREC_UINT 	BIG_NUM_PREC_MAN

#define big_num_strg_t		uint
#define big_num_sstrg_t		int
#define big_num_lstrg_t		uint64_t
#define big_num_lsstrg_t	int64_t
#define big_num_carry_t		big_num_strg_t
// let glsl know about some C types to make C code a bit easier to port
#define size_t				uint
#define ssize_t				int

#define BIG_NUM_MUL1 		0
// #define BIG_NUM_MUL2 		1
#define BIG_NUM_DIV1		0
// #define BIG_NUM_DIV2		1
#define big_num_algo_t		uint

#define BIG_NUM_DIV_OK		0
#define BIG_NUM_DIV_ZERO	1
#define BIG_NUM_DIV_BUSY	2
#define big_num_div_ret_t	uint

#define BIG_NUM_POW_OK		0
#define BIG_NUM_POW_CARRY	1
#define BIG_NUM_POW_INVALID	2
#define big_num_pow_ret_t	uint

//===============
// BigUint.h
//===============

struct big_uint_t {
	big_num_strg_t			table[BIG_NUM_PREC_UINT];
};

size_t	 			big_uint_size(inout big_uint_t self);
void 				big_uint_set_zero(inout big_uint_t self);
void 				big_uint_set_one(inout big_uint_t self);
void 				big_uint_set_max(inout big_uint_t self);
void 				big_uint_set_min(inout big_uint_t self);
void 				big_uint_swap(inout big_uint_t self, inout big_uint_t ss2);
void 				big_uint_set_from_table(inout big_uint_t self, in const big_num_strg_t temp_table[BIG_NUM_PREC_UINT], in size_t temp_table_len);


big_num_strg_t		big_uint_add_one(inout big_uint_t self);
big_num_carry_t 	big_uint_add(inout big_uint_t self, in big_uint_t ss2, in big_num_carry_t c);
big_num_strg_t		big_uint_sub_one(inout big_uint_t self);
big_num_carry_t 	big_uint_sub(inout big_uint_t self, in big_uint_t ss2, in big_num_carry_t c);
big_num_strg_t 		big_uint_rcl(inout big_uint_t self, in size_t bits, in big_num_carry_t c);
big_num_strg_t 		big_uint_rcr(inout big_uint_t self, in size_t bits, in big_num_carry_t c);
size_t	 			big_uint_compensation_to_left(inout big_uint_t self);
bool 				big_uint_find_leading_bit(in big_uint_t self, inout size_t table_id, inout size_t index);
bool 				big_uint_find_lowest_bit(in big_uint_t self, inout size_t table_id, inout size_t index);
bool	 			big_uint_get_bit(in big_uint_t self, in size_t bit_index);
bool	 			big_uint_set_bit(inout big_uint_t self, in size_t bit_index);
void 				big_uint_bit_and(inout big_uint_t self, in const big_uint_t ss2);
void 				big_uint_bit_or(inout big_uint_t self, in const big_uint_t ss2);
void 				big_uint_bit_xor(inout big_uint_t self, in const big_uint_t ss2);
void 				big_uint_bit_not(inout big_uint_t self);
void				big_uint_bit_not2(inout big_uint_t self);


big_num_carry_t		big_uint_mul_int(inout big_uint_t self, in big_num_strg_t ss2);
big_num_carry_t		big_uint_mul(inout big_uint_t self, in const big_uint_t ss2, in big_num_algo_t algorithm);


big_num_div_ret_t 	big_uint_div_int(inout big_uint_t self, big_num_strg_t divisor, inout big_num_strg_t remainder);
big_num_div_ret_t 	big_uint_div(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t remainder, big_num_algo_t algorithm);

big_num_pow_ret_t	big_uint_pow(inout big_uint_t self, big_uint_t _pow);
void 				big_uint_sqrt(inout big_uint_t self);


void 				big_uint_clear_first_bits(inout big_uint_t self, size_t n);
bool 				big_uint_is_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_zero(big_uint_t self);
bool 				big_uint_are_first_bits_zero(big_uint_t self, size_t bits);


void 				big_uint_init(inout big_uint_t self);
void 				big_uint_init_uint(inout big_uint_t self, big_num_strg_t value);
void 				big_uint_init_big_uint(inout big_uint_t self, const big_uint_t value);
big_num_carry_t		big_uint_init_int(inout big_uint_t self, big_num_sstrg_t value);
big_num_carry_t		big_uint_to_uint(big_uint_t self, inout big_num_strg_t result);
big_num_carry_t		big_uint_to_int(big_uint_t self, inout big_num_sstrg_t result);

bool 				big_uint_cmp_smaller(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_equal(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_smaller_equal(big_uint_t self, const big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger_equal(big_uint_t self, const big_uint_t l, ssize_t index);

//===============
// BigUint.c
//===============

#define TABLE_SIZE 	BIG_NUM_PREC_UINT
#define BIG_TABLE_SIZE (2*TABLE_SIZE)
#define BIG_NUM_KARATSUBA_MULTIPLICATION_FROM_SIZE 3

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

// used for MulXBigN functions
struct _big_big_uint_t {
	big_num_strg_t table[2*BIG_NUM_PREC_UINT];
};

big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, inout big_num_strg_t result);
big_num_carry_t	_big_uint_add_uint(inout big_uint_t self, big_num_strg_t value, size_t index);
big_num_carry_t _big_uint_add_two_uints(inout big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index);
big_num_carry_t _big_uint_add_vector(inout big_num_strg_t ss1[TABLE_SIZE], inout big_num_strg_t ss2[TABLE_SIZE], size_t ss1_size, size_t ss2_size, inout big_num_strg_t result[TABLE_SIZE]);
big_num_carry_t	_big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, inout big_num_strg_t result);
big_num_carry_t	_big_uint_sub_uint(inout big_uint_t self, big_num_strg_t value, size_t index);
big_num_carry_t _big_uint_sub_vector(inout big_num_strg_t ss1[TABLE_SIZE], inout big_num_strg_t ss2[TABLE_SIZE], size_t ss1_size, size_t ss2_size, inout big_num_strg_t result[TABLE_SIZE]);
big_num_carry_t	_big_uint_rcl2_one(inout big_uint_t self, big_num_strg_t c);
big_num_carry_t _big_uint_rcr2_one(inout big_uint_t self, big_num_strg_t c);
big_num_carry_t	_big_uint_rcl2(inout big_uint_t self, size_t bits, big_num_strg_t c);
big_num_carry_t	_big_uint_rcr2(inout big_uint_t self, size_t bits, big_num_strg_t c);
big_num_sstrg_t _big_uint_find_leading_bit_in_word(big_num_strg_t x);
big_num_sstrg_t _big_uint_find_lowest_bit_in_word(big_num_strg_t x);
big_num_strg_t 	_big_uint_set_bit_in_word(inout big_num_strg_t value, size_t bit);
void 			_big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, inout big_num_strg_t result_high, inout big_num_strg_t result_low);
void			_big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, inout big_num_strg_t r, inout big_num_strg_t rest);


void 			_big_uint_rcl_move_all_words(inout big_uint_t self, inout size_t rest_bits, inout big_num_carry_t last_c, size_t bits, big_num_strg_t c);
void 			_big_uint_rcr_move_all_words(inout big_uint_t self, inout size_t rest_bits, inout big_num_carry_t last_c, size_t bits, big_num_strg_t c);

big_num_carry_t	_big_uint_mul1_ref(inout big_uint_t self, const big_uint_t ss2);
big_num_carry_t _big_uint_mul1(inout big_uint_t self, const big_uint_t ss2);

div_std_test_t	_big_uint_div_standard_test(inout big_uint_t self, const big_uint_t divisor, inout size_t m, inout size_t n, inout big_uint_t remainder);
div_calc_test_t	_big_uint_div_calculating_size(inout big_uint_t self, const big_uint_t divisor, inout size_t m, inout size_t n);
big_num_div_ret_t 	_big_uint_div1(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t remainder);
big_num_div_ret_t	_big_uint_div1_calculate(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t rest);
big_num_div_ret_t	_big_uint_div1_calculate_ref(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t rest);
bool 			_big_uint_div2_divisor_greater_or_equal(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t remainder, size_t table_id, size_t index, size_t divisor_index);

/**
 * this method returns the size of the table
 * @param[in, out] self the big num object
 * @return size_t the size of the table
 */
size_t big_uint_size(inout big_uint_t self)
{
	return TABLE_SIZE;
}

/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_uint_set_zero(inout big_uint_t self)
{
	for(size_t i=0 ; i < TABLE_SIZE ; ++i)
		self.table[i] = 0;
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_uint_set_one(inout big_uint_t self)
{
	big_uint_set_zero(self);
	self.table[0] = 1;
}

/**
 * this method sets the max value which self class can hold
 * @param[in, out] self the big num object
 */
void big_uint_set_max(inout big_uint_t self)
{
	for(size_t i=0 ; i < TABLE_SIZE; ++i)
		self.table[i] = BIG_NUM_MAX_STRG;
}

/**
 * this method sets the min value which self class can hold
 * (for an unsigned integer value the zero is the smallest value)
 * @param[in, out] self the big num object
 */
void big_uint_set_min(inout big_uint_t self)
{
	big_uint_set_zero(self);
}

/**
 * this method swappes self and ss2
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the value to swap with self, ss2 contains self after swap
 */
void big_uint_swap(inout big_uint_t self, inout big_uint_t ss2)
{
	for(size_t i=0 ; i < TABLE_SIZE ; ++i) {
		big_num_strg_t temp = self.table[i];
		self.table[i] = ss2.table[i];
		ss2.table[i] = temp;
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
void big_uint_set_from_table(inout big_uint_t self, const big_num_strg_t temp_table[TABLE_SIZE], size_t temp_table_len)
{
	size_t temp_table_index = 0;
	ssize_t i; // 'i' with a sign

	for(i=TABLE_SIZE-1 ; i>=0 && temp_table_index<temp_table_len; --i, ++temp_table_index)
		self.table[i] = temp_table[ temp_table_index ];


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
			if( self.table[0] != BIG_NUM_MAX_STRG )
				++(self.table)[0];
		}
	}

	// cleaning the rest of the mantissa
	for( ; i>=0 ; --i)
		self.table[i] = 0;
}

/*!
 *
 *	basic mathematic functions
 *
 */

/**
 * this method adds one to the existing value
 * @param[in, out] self the big num object
 * @return big_num_strg_t the carry, if there was one
 */
big_num_strg_t big_uint_add_one(inout big_uint_t self)
{
	return _big_uint_add_uint(self, 1, 0);
}

/**
 * this method adding ss2 to the this and adding carry if it's defined. (this = this + ss2 + c)
 * @param[in, out] self the big num object
 * @param[in] ss2  the big num object to add to self
 * @param[in] c    must be zero or one (might be a bigger value than 1), set to 1 if previous operation resulted in a carry
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_add(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;	

	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_add_two_words(self.table[i], ss2.table[i], c, self.table[i]);

	return c;
}


/**
 * this method subtracts one from the existing value
 * @param[in, out] self the big num object
 * @return big_num_strg_t the carry, if there was one
 */
big_num_strg_t big_uint_sub_one(inout big_uint_t self)
{
	return _big_uint_sub_uint(self, 1, 0);
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
big_num_carry_t big_uint_sub(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;
	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_sub_two_words(self.table[i], ss2.table[i], c, self.table[i]);

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
big_num_strg_t big_uint_rcl(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_strg_t last_c   = 0;
	size_t rest_bits 		= bits;

	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_STRG )
		_big_uint_rcl_move_all_words(self, rest_bits, last_c, bits, c);

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
 * c . self . return value
 * 
 * @param[in, out] self the big num object
 * @param[in] bits number of bits to shift, range of <0, man * BIG_NUM_BITS_PER_STRG>, or it can be even bigger then all bits will be set to 'c'
 * @param[in] c bit to insert in MSb
 * @return big_num_strg_t state of the last moved bit
 */
big_num_strg_t big_uint_rcr(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_carry_t last_c    = 0;
	size_t rest_bits = bits;
	
	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_STRG )
		_big_uint_rcr_move_all_words(self, rest_bits, last_c, bits, c);

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
size_t big_uint_compensation_to_left(inout big_uint_t self)
{
	size_t moving = 0;

	// a - index a last word which is different from zero
	ssize_t a;
	for(a=TABLE_SIZE-1 ; a>=0 && self.table[a]==0 ; --a);

	if( a < 0 )
		return moving; // all words in table have zero

	if( a != TABLE_SIZE-1 )
	{
		moving += ( TABLE_SIZE-1 - a ) * BIG_NUM_BITS_PER_STRG;

		// moving all words
		ssize_t i;
		for(i=TABLE_SIZE-1 ; a>=0 ; --i, --a)
			self.table[i] = self.table[a];

		// setting the rest word to zero
		for(; i>=0 ; --i)
			self.table[i] = 0;
	}

	size_t moving2 = _big_uint_find_leading_bit_in_word( self.table[TABLE_SIZE-1] );
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
bool big_uint_find_leading_bit(big_uint_t self, inout size_t table_id, inout size_t index)
{
	for(table_id=TABLE_SIZE-1 ; table_id!=0 && self.table[table_id]==0 ; --(table_id));

	if( table_id==0 && self.table[table_id]==0 )
	{
		// is zero
		index = 0;

		return false;
	}
	
	// table[table_id] is different from 0
	index = _big_uint_find_leading_bit_in_word( self.table[table_id] );

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
bool big_uint_find_lowest_bit(big_uint_t self, inout size_t table_id, inout size_t index)
{
	for(table_id=0 ; table_id<TABLE_SIZE && self.table[table_id]==0 ; ++(table_id));

		if( table_id >= TABLE_SIZE )
		{
			// is zero
			index    = 0;
			table_id = 0;

			return false;
		}
		
		// table[table_id] is different from 0
		index = _big_uint_find_lowest_bit_in_word( self.table[table_id] );

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
	big_num_strg_t res  = _big_uint_set_bit_in_word(temp, bit);

	return res != 0;
}

/**
 * setting the 'bit_index' bit
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the last state of the bit 
 */
bool big_uint_set_bit(inout big_uint_t self, size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_STRG;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_STRG;

	big_num_strg_t temp = self.table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(temp, bit);

	return res != 0;
}

/**
 * this method performs a bitwise operation AND 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_and(inout big_uint_t self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] &= ss2.table[x];
}

/**
 * this method performs a bitwise operation OR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_or(inout big_uint_t self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] |= ss2.table[x];
}

/**
 * this method performs a bitwise operation XOR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_xor(inout big_uint_t self, const big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] ^= ss2.table[x];
}

/**
 * this method performs a bitwise operation NOT
 * @param[in, out] self the big num object
 */
void big_uint_bit_not(inout big_uint_t self)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] = ~self.table[x];
}

/**
 * this method performs a bitwise operation NOT but only
 * on the range of <0, leading_bit>
 * 
 * for example:
 * 		BitNot2(8) = BitNot2( 1000(bin) ) = 111(bin) = 7
 * @param[in, out] self the big num object
 */
void big_uint_bit_not2(inout big_uint_t self)
{
	size_t table_id, index;

	if( big_uint_find_leading_bit(self, table_id, index) ) {
		for(size_t x=0 ; x<table_id ; ++x)
			self.table[x] = ~(self.table[x]);

		big_num_strg_t mask  = BIG_NUM_MAX_STRG;
		size_t shift = BIG_NUM_BITS_PER_STRG - index - 1;

		if(shift != 0)
			mask >>= shift;

		self.table[table_id] ^= mask;
	}
	else
		self.table[0] = 1;
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
big_num_carry_t big_uint_mul_int(inout big_uint_t self, big_num_strg_t ss2)
{
	big_num_strg_t r1, r2;
	size_t x1;
	big_num_carry_t c = 0;

	big_uint_t u = self;
	big_uint_set_zero(self);

	if( ss2 == 0 ) {
		return 0;
	}

	for(x1=0 ; x1<TABLE_SIZE-1 ; ++x1) {
		_big_uint_mul_two_words(u.table[x1], ss2, r2, r1);
		c += _big_uint_add_two_uints(self, r2,r1,x1);
	}

	// x1 = value_size-1  (last word)
	_big_uint_mul_two_words(u.table[x1], ss2, r2, r1);
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
big_num_carry_t big_uint_mul(inout big_uint_t self, const big_uint_t ss2, big_num_algo_t algorithm)
{
	switch (algorithm) {
		case BIG_NUM_MUL1: return _big_uint_mul1(self, ss2); break;
		// case BIG_NUM_MUL2: return _big_uint_mul2(self, ss2); break;
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
 * @param divisor 
 * @param remainder 
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_uint_div_int(inout big_uint_t self, big_num_strg_t divisor, inout big_num_strg_t remainder)
{
	if( divisor == 0 ) {
		remainder = 0; // this is for convenience, without it the compiler can report that 'remainder' is uninitialized

		return 1;
	}

	if( divisor == 1 ) {
		remainder = 0;

		return 0;
	}

	big_uint_t dividend = self;
	big_uint_set_zero(self);
	
	big_num_sstrg_t i;  // i must be with a sign
	big_num_strg_t r = 0;

	// we're looking for the last word in ss1
	for(i=TABLE_SIZE-1 ; i>0 && dividend.table[i]==0 ; --i);

	for( ; i>=0 ; --i)
		_big_uint_div_two_words(r, dividend.table[i], divisor, self.table[i], r);

	remainder = r;

	return 0;
}

/**
 * division self = self / ss2
 * 
 * @param[in, out] self the big num object
 * @param divisor 
 * @param remainder 
 * @param algorithm 
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_uint_div(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t remainder, big_num_algo_t algorithm)
{
	switch( algorithm ){
		case 1:
		default:
			return _big_uint_div1(self, divisor, remainder);

		// case 2:
			// return _big_uint_div2(self, divisor, remainder);
	}
}

/**
 * power self = self ^ pow
 * binary algorithm (r-to-l)
 * 
 * @param[in, out] self the big num object
 * @param pow The power to raise self to
 * @return big_num_pow_ret_t 
 */
big_num_pow_ret_t big_uint_pow(inout big_uint_t self, big_uint_t _pow)
{
	if(big_uint_is_zero(_pow) && big_uint_is_zero(self))
		// we don't define zero^zero
		return BIG_NUM_POW_INVALID;

	big_uint_t start = self;
	big_uint_t result;
	big_uint_set_one(result);
	big_num_carry_t c = 0;

	while( c == 0 )
	{
		if( (_pow.table[0] & 1) != 0 )
			c += big_uint_mul(result, start, BIG_NUM_DEF_MUL_ALGO);

		_big_uint_rcr2_one(_pow, 0);
		if( big_uint_is_zero(_pow) )
			break;

		c += big_uint_mul(start, start, BIG_NUM_DEF_MUL_ALGO);
	}

	self = result;

	return (c==0)? 0 : 1;
}

/**
 * square root
 * e.g. Sqrt(9) = 3
 * ('digit-by-digit' algorithm)
 * @param[in, out] self the big num object
 */
void big_uint_sqrt(inout big_uint_t self)
{
	big_uint_t bit, temp;

	if( big_uint_is_zero(self) )
		return;

	big_uint_t value = self;

	big_uint_set_zero(self);
	big_uint_set_zero(bit);
	bit.table[TABLE_SIZE-1] = (BIG_NUM_HIGHEST_BIT >> 1);
	
	while( big_uint_cmp_bigger(bit, value, -1))
		big_uint_rcr(bit, 2, 0);

	while( !big_uint_is_zero(bit) ) {
		temp = self;
		big_uint_add(temp, bit, 0);

		if( big_uint_cmp_bigger_equal(value, temp, -1) ) {
			big_uint_sub(value, temp, 0);
			big_uint_rcr(self, 1, 0);
			big_uint_add(self, bit, 0);
		}
		else {
			big_uint_rcr(self, 1, 0);
		}

		big_uint_rcr(bit, 2, 0);
	}
}

/**
 * this method sets n first bits to value zero
 * 
 * For example:
 * let n=2 then if there's a value 111 (bin) there'll be '100' (bin)
 * @param[in, out] self the big num object
 * @param n 
 */
void big_uint_clear_first_bits(inout big_uint_t self, size_t n)
{
	if( n >= TABLE_SIZE*BIG_NUM_BITS_PER_STRG ) {
		big_uint_set_zero(self);
		return;
	}

	size_t index = 0;
	// first we're clearing the whole words
	while( n >= BIG_NUM_BITS_PER_STRG ) {
		self.table[index++] = 0;
		n   -= BIG_NUM_BITS_PER_STRG;
	}

	if( n == 0 ) {
		return;
	}

	// and then we're clearing one word which has left
	// mask -- all bits are set to one
	big_num_strg_t mask = BIG_NUM_MAX_STRG;

	mask = mask << n;

	self.table[index] &= mask;
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
 * @param bits 
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
 * a default constructor
 * we don't clear the table
 * @param[in, out] self the big num object
 */
void big_uint_init(inout big_uint_t self)
{
	return;
}

/**
 * a constructor for converting the big_num_strg_t big_uint_t
 * @param[in, out] self the big num object
 * @param value
 */
void big_uint_init_uint(inout big_uint_t self, big_num_strg_t value)
{
	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		self.table[i] = 0;

	self.table[0] = value;
}

/**
 * a copy constructor
 * @param[in, out] self the big num object
 * @param u the other big num object
 */
void big_uint_init_big_uint(inout big_uint_t self, const big_uint_t value)
{
	for(size_t i=0 ; i<TABLE_SIZE ; ++i)
		self.table[i] = value.table[i];
}

/**
 * a constructor for converting the big_num_sstrg_t to big_uint_t
 * 
 * @param[in, out] self the big num object
 * @param value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_int(inout big_uint_t self, big_num_sstrg_t value)
{
	big_uint_init_uint(self, big_num_strg_t(value));
	if (value < 0)
		return 1;
	return 0;
}

/**
 * this method converts the value to big_num_strg_t type
 * can return a carry if the value is too long to store it in big_num_strg_t type
 * @param[in, out] self the big num object
 * @param result 
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_to_uint(big_uint_t self, inout big_num_strg_t result)
{
	result = self.table[0];

	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		if( self.table[i] != 0 )
			return 1;

	return 0;
}

/**
 * this method converts the value to big_num_sstrg_t type (signed integer)
 * can return a carry if the value is too long to store it in big_num_sstrg_t type
 * @param[in, out] self the big num object
 * @param result 
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_to_int(big_uint_t self, inout big_num_sstrg_t result)
{
	big_num_strg_t temp;
	big_num_carry_t carry = big_uint_to_uint(self, temp);
	result = big_num_sstrg_t(temp);
	return carry;
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
 * @param l the other big num object
 * @param index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_smaller(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index >= big_num_sstrg_t(TABLE_SIZE) )
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
 * @param l the other big num object
 * @param index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_bigger(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index >= big_num_sstrg_t(TABLE_SIZE) )
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
 * @param l the other big num object
 * @param index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= big_num_sstrg_t(TABLE_SIZE) )
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
 * @param l the other big num object
 * @param index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_smaller_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= big_num_sstrg_t(TABLE_SIZE) )
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
 * @param l the other big num object
 * @param index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_bigger_equal(big_uint_t self, const big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index >= big_num_sstrg_t(TABLE_SIZE) )
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
 * @param a 
 * @param b 
 * @param carry carry
 * @param result 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, inout big_num_strg_t result)
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

	result = temp;

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
 * @param value 
 * @param index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_uint(inout big_uint_t self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index], value, 0, self.table[index]);

	for(i=index+1 ; i < TABLE_SIZE && (c != 0) ; ++i)
		c = _big_uint_add_two_words(self.table[i], 0, c, self.table[i]);
	
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
 * @param x1
 * @param x2
 * @param index
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_two_uints(inout big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index],   x1, 0, self.table[index]);
	c = _big_uint_add_two_words(self.table[index+1], x2, c, self.table[index+1]);

	for(i=index+2 ; i<TABLE_SIZE && (c != 0) ; ++i)
		c = _big_uint_add_two_words(self.table[i], 0, c, self.table[i]);
	
	return c;
}

/**
 * this method addes one vector to the other
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
 * @param ss1 
 * @param ss2 
 * @param ss1_size 
 * @param ss2_size 
 * @param result 
 * @return big_num_carry_t 
 */
big_num_carry_t _big_uint_add_vector(inout big_num_strg_t ss1[TABLE_SIZE], inout big_num_strg_t ss2[TABLE_SIZE], size_t ss1_size, size_t ss2_size, inout big_num_strg_t result[TABLE_SIZE])
{
	size_t i;
	big_num_carry_t c = 0;

	for(i=0 ; i<ss2_size ; ++i)
		c = _big_uint_add_two_words(ss1[i], ss2[i], c, result[i]);

	for( ; i<ss1_size ; ++i)
		c = _big_uint_add_two_words(ss1[i], 0, c, result[i]);

	return c;
}

/**
 * this method subtractes one word from the other
 * @param a 
 * @param b 
 * @param carry carry
 * @param result 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, inout big_num_strg_t result)
{
	if( carry == 0 ) {
		result = a - b;

		if( a < b )
			carry = 1;
	} else {
		carry   = 1;
		result = a - b - carry;

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
 * @param value 
 * @param index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_sub_uint(inout big_uint_t self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_sub_two_words(self.table[index], value, 0, self.table[index]);

	for(i=index+1 ; i<TABLE_SIZE && (c != 0) ; ++i)
		c = _big_uint_sub_two_words(self.table[i], 0, c, self.table[i]);

	return c;
}

/**
 * this method subtractes one vector from the other
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
 * @param ss1 
 * @param ss2 
 * @param ss1_size 
 * @param ss2_size 
 * @param result 
 * @return big_num_carry_t 
 */
big_num_carry_t _big_uint_sub_vector(inout big_num_strg_t ss1[TABLE_SIZE], inout big_num_strg_t ss2[TABLE_SIZE], size_t ss1_size, size_t ss2_size, inout big_num_strg_t result[TABLE_SIZE])
{
	size_t i;
	big_num_carry_t c = 0;

	for(i=0 ; i<ss2_size ; ++i)
			c = _big_uint_sub_two_words(ss1[i], ss2[i], c, result[i]);

	for( ; i<ss1_size ; ++i)
		c = _big_uint_sub_two_words(ss1[i], 0, c, result[i]);

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
 * @param c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcl2_one(inout big_uint_t self, big_num_carry_t c)
{
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = 1;

	for(i=0 ; i<TABLE_SIZE ; ++i) {
		new_c    		= (self.table[i] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
		self.table[i] 	= (self.table[i] << 1) | c;
		c        		= new_c;
	}

	return c;
}

/**
 * this method moves all bits into the right hand side
 * c . self . return value
 * 
 * the highest *bit* will be held the 'c' and
 * the state of one additional bit (on the right hand side)
 * will be returned
 * 
 * for example:
 * let self is 000000010
 * after _big_uint_rcr2_one(1) there'll be 100000001 and _big_uint_rcr2_one returns 0
 * @param[in, out] self 
 * @param c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2_one(inout big_uint_t self, big_num_carry_t c)
{
	big_num_sstrg_t i; // signed i
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_HIGHEST_BIT;

	for(i=big_num_sstrg_t(TABLE_SIZE-1) ; i>=0 ; --i) {
		new_c    		= (self.table[i] & 1) != 0 ? BIG_NUM_HIGHEST_BIT : 0;
		self.table[i] 	= (self.table[i] >> 1) | c;
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
 * @param bits 
 * @param c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcl2(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_STRG - bits;
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_STRG >> move;

	for(i=0 ; i<TABLE_SIZE ; ++i) {
		new_c    		= self.table[i] >> move;
		self.table[i] 	= (self.table[i] << bits) | c;
		c        		= new_c;
	}

	return (c & 1);
}

/**
 * this method moves all bits into the right hand side
 * C . self . return value
 * 
 * the highest *bits* will be held the 'c' and
 * the state of one additional bit (on the right hand side)
 * will be returned
 * 
 * for example:
 * let self is 000000010
 * after _big_uint_rcr2(2, 1) there'll be 110000000 and _big_uint_rcr2 returns 1
 * @param[in, out] self 
 * @param bits 
 * @param c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_STRG - bits;
	size_t i; // signed
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_STRG << move;

	for(i=TABLE_SIZE-1 ; i>=0 ; --i) {
		new_c    		= self.table[i] << move;
		self.table[i] 	= (self.table[i] >> bits) | c;
		c        		= new_c;
	}

	c = (c & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;

	return c;
}

/**
 * this method returns the number of the highest set bit in x
 * @param x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
big_num_sstrg_t _big_uint_find_leading_bit_in_word(big_num_strg_t x)
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
 * @param x 
 * @return big_num_sstrg_t if the 'x' is zero this method returns '-1'
 */
big_num_sstrg_t _big_uint_find_lowest_bit_in_word(big_num_strg_t x)
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
 * @param value 
 * @param bit the bit to set, between <0,BIG_NUM_BITS_PER_STRG-1>
 * @return big_num_strg_t 
 */
big_num_strg_t _big_uint_set_bit_in_word(inout big_num_strg_t value, size_t bit)
{
	big_num_strg_t mask = 1;

	if( bit > 0 )
		mask = mask << bit;

	big_num_strg_t last = value & mask;
	value     	 	    = value | mask;

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
 * @param a 
 * @param b 
 * @param result_high 
 * @param result_low 
 */
void _big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, inout big_num_strg_t result_high, inout big_num_strg_t result_low)
{
	uvec2 res = unpackUint2x32(big_num_lstrg_t(a) * big_num_lstrg_t(b));     // multiply two 32bit words, the result has 64 bits

	result_high = res.y;
	result_low  = res.x;
}

/**
 * this method calculates big_num_lstrg_t a / big_num_strg_t c (a higher, b lower word)
 * r = a / c and rest - remainder
 * @warning the c has to be suitably large for the result being keeped in one word,
 * if c is equal zero there'll be a hardware interruption (0)
 * and probably the end of your program
 * @param a 
 * @param b 
 * @param c
 * @param r 
 * @param rest 
 */
void _big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, inout big_num_strg_t r, inout big_num_strg_t rest)
{

	big_num_lstrg_t ab = big_num_lstrg_t(a) << BIG_NUM_BITS_PER_STRG;
	ab += big_num_lstrg_t(b);

	r    = big_num_strg_t(ab / c);
	rest = big_num_strg_t(ab % c);
}




/**
 * an auxiliary method for moving bits into the left hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param rest_bits 
 * @param last_c 
 * @param bits 
 * @param c carry
 */
void _big_uint_rcl_move_all_words(inout big_uint_t self, inout size_t rest_bits, inout big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits      		= bits % BIG_NUM_BITS_PER_STRG;
	size_t all_words 	= bits / BIG_NUM_BITS_PER_STRG;
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_STRG : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && rest_bits == 0 )
			last_c = self.table[0] & 1;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<TABLE_SIZE ; ++i)
			self.table[i] = mask;

		rest_bits = 0;
	}
	else
	if( all_words > 0 ) {
		// 0 < all_words < TABLE_SIZE
		ssize_t first, second;
		last_c = self.table[TABLE_SIZE - all_words] & 1; // all_words is greater than 0

		// copying the first part of the value
		for(first = TABLE_SIZE-1, second=ssize_t(first-all_words) ; second>=0 ; --first, --second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first>=0 ; --first )
			self.table[first] = mask;
	}
}

/**
 * an auxiliary method for moving bits into the right hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param rest_bits 
 * @param last_c 
 * @param bits 
 * @param c 
 */
void _big_uint_rcr_move_all_words(inout big_uint_t self, inout size_t rest_bits, inout big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits      		= bits % BIG_NUM_BITS_PER_STRG;
	size_t all_words 	= bits / BIG_NUM_BITS_PER_STRG;
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_STRG : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && rest_bits == 0 )
			last_c = (self.table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<TABLE_SIZE ; ++i)
			self.table[i] = mask;

		rest_bits = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < TABLE_SIZE

		ssize_t first, second;
		last_c = (self.table[all_words - 1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0; // all_words is > 0

		// copying the first part of the value
		for(first=0, second=ssize_t(all_words) ; second<TABLE_SIZE ; ++first, ++second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first<TABLE_SIZE ; ++first )
			self.table[first] = mask;
	}
}


/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param ss2 the other big num object
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_mul1_ref(inout big_uint_t self, const big_uint_t ss2)
{
	big_uint_t ss1 = self;
	big_uint_set_zero(self);

	for(size_t i=0; i < TABLE_SIZE*BIG_NUM_BITS_PER_STRG ; ++i) {
		if( 0 != big_uint_add(self, self, 0) ) {
			return 1;
		}

		if( 0 != big_uint_rcl(ss1, 1, 0) ) {
			if( 0 != big_uint_add(self, ss2, 0) ) {
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
big_num_carry_t _big_uint_mul1(inout big_uint_t self, const big_uint_t ss2)
{
	return _big_uint_mul1_ref(self, ss2);
}


/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor 
 * @param m 
 * @param n 
 * @param remainder 
 * @return div_std_test_t 
 */
div_std_test_t _big_uint_div_standard_test(inout big_uint_t self, const big_uint_t divisor, inout size_t m, inout size_t n, inout big_uint_t remainder){
	switch( _big_uint_div_calculating_size(self, divisor, m, n) ) {
	case 4: // 'this' is equal divisor
		big_uint_set_zero(remainder);

		big_uint_set_one(self);
		return BIG_NUM_STD_DIV_NONE;
	case 3: // 'this' is smaller than divisor
		remainder = self; // copy

		big_uint_set_zero(self);
		return BIG_NUM_STD_DIV_NONE;
	case 2: // 'this' is zero
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
 * @param m 
 * @param n 
 * @return div_calc_test_t 
 */
div_calc_test_t _big_uint_div_calculating_size(inout big_uint_t self, const big_uint_t divisor, inout size_t m, inout size_t n){
	m = n = TABLE_SIZE-1;

	for( ; n!=0 && divisor.table[n]==0 ; --n);

	if( n==0 && divisor.table[n]==0 )
		return BIG_NUM_CALC_DIV_ZERO;

	for( ; m!=0 && self.table[m]==0 ; --m);

	if( m==0 && self.table[m]==0 )
		return BIG_NUM_CALC_SELF_ZERO;

	if( m < n )
		return BIG_NUM_CALC_SELF_LST;
	else if( m == n ) {
		size_t i;
		for(i = n ; i!=0 && self.table[i]==divisor.table[i] ; --i);
		
		if( self.table[i] < divisor.table[i] )
			return BIG_NUM_CALC_SELF_LST;
		else if (self.table[i] == divisor.table[i] )
			return BIG_NUM_CALC_SELF_EQ;
	}

	return BIG_NUM_CALC_DIV_OK;
}

/**
 * 
 * @param[in, out] self the big num object
 * @param divisor the value to divide self by
 * @param remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t remainder){
	size_t m,n;
	div_std_test_t test;

	test = _big_uint_div_standard_test(self, divisor, m, n, remainder);
	switch (test) {
		case BIG_NUM_STD_DIV_NONE: return BIG_NUM_DIV_OK;
		case BIG_NUM_STD_DIV_ZERO: return BIG_NUM_DIV_ZERO;
		default: break;
	}

	return _big_uint_div1_calculate(self, divisor, remainder);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1_calculate(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t rest){
	return _big_uint_div1_calculate_ref(self, divisor, rest);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1_calculate_ref(inout big_uint_t self, const big_uint_t divisor, inout big_uint_t rest){
	big_num_sstrg_t loop;
	big_num_carry_t c;

	big_uint_set_zero(rest);
	loop = TABLE_SIZE * BIG_NUM_BITS_PER_STRG;
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
