#version 460 core
#extension GL_ARB_gpu_shader_int64 : require

// Config

#define BIG_NUM_PREC	 	2 // amount of word's to allocate for uint and int, float using 2x this amount

/**
 * during some kind of calculating when we're making any long formulas
 * (for example Taylor series)
 * 
 * it's used in ExpSurrounding0(...), LnSurrounding1(...), Sin0pi05(...), etc.
 * 
 * note! there'll not be so many iterations, iterations are stopped when
 * there is no sense to continue calculating (for example when the result
 * still remains unchanged after adding next series and we know that the next
 * series are smaller than previous ones)
 */
#define BIG_NUM_ARITHMETIC_MAX_LOOP	1000

#define BIG_NUM_MUL_DEF		BIG_NUM_MUL1
#define BIG_NUM_DIV_DEF		BIG_NUM_DIV1

//===============
// Don't touch
//===============

//===============
// defines from BigNum.h
//===============

#define BIG_NUM_MAX_VALUE 			((1<<( 32     ))-1)
#define BIG_NUM_HIGHEST_BIT			((1<<( 32 - 1 )))
#define BIG_NUM_BITS_PER_UNIT		(32)

#define BIG_NUM_MUL1 				0
#define BIG_NUM_MUL2 				1
#define BIG_NUM_DIV1				0
#define BIG_NUM_DIV2				1
#define big_num_algo_t				uint

#define BIG_NUM_DIV_OK				0
#define BIG_NUM_DIV_ZERO			1
#define BIG_NUM_DIV_BUSY			2
#define big_num_div_ret_t			uint

#define BIG_NUM_OK					0
#define BIG_NUM_OVERFLOW			1
#define BIG_NUM_INVALID_ARG			2
#define BIG_NUM_LOG_INVALID_BASE	3
#define big_num_ret_t				uint

#define big_num_strg_t				uint
#define big_num_sstrg_t				int
#define big_num_lstrg_t				uint64_t
#define big_num_lsstrg_t			int64_t
#define big_num_carry_t				big_num_strg_t
// let glsl know about some C types to make C code a bit easier to port
#define size_t						uint
#define ssize_t						int

//===============
// BigUint.h
//===============

struct big_uint_t {
	big_num_strg_t			table[BIG_NUM_PREC];
};

struct big_big_uint_t {
	big_num_strg_t			table[2*BIG_NUM_PREC];
};

size_t	 			big_uint_size(inout big_uint_t self);
void 				big_uint_set_zero(inout big_uint_t self);
void 				big_uint_set_zero_big(inout big_big_uint_t self);
void 				big_uint_set_one(inout big_uint_t self);
void				big_uint_set_one_big(inout big_big_uint_t self);
void 				big_uint_set_max(inout big_uint_t self);
void 				big_uint_set_min(inout big_uint_t self);
void 				big_uint_swap(inout big_uint_t self, inout big_uint_t ss2);
void 				big_uint_set_from_table(inout big_uint_t self, big_num_strg_t temp_table[BIG_NUM_PREC], size_t temp_table_len);


big_num_carry_t		big_uint_add_uint(inout big_uint_t self, big_num_strg_t val);
big_num_carry_t 	big_uint_add(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c);
big_num_carry_t 	big_uint_add_big(inout big_big_uint_t self, big_big_uint_t ss2, big_num_carry_t c);
big_num_carry_t		big_uint_sub_uint(inout big_uint_t self, big_num_strg_t val);
big_num_carry_t 	big_uint_sub(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c);
big_num_carry_t 	big_uint_sub_big(inout big_big_uint_t self, big_big_uint_t ss2, big_num_carry_t c);
big_num_strg_t 		big_uint_rcl(inout big_uint_t self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcl_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr(inout big_uint_t self, size_t bits, big_num_carry_t c);
big_num_strg_t 		big_uint_rcr_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c);
size_t	 			big_uint_compensation_to_left(inout big_uint_t self);
size_t	 			big_uint_compensation_to_left_big(inout big_big_uint_t self);
bool 				big_uint_find_leading_bit(big_uint_t self, out size_t table_id, out size_t index);
bool 				big_uint_find_lowest_bit(big_uint_t self, out size_t table_id, out size_t index);
big_num_strg_t		big_uint_get_bit(big_uint_t self, size_t bit_index);
big_num_strg_t		big_uint_set_bit(inout big_uint_t self, size_t bit_index);
void 				big_uint_bit_and(inout big_uint_t self, big_uint_t ss2);
void 				big_uint_bit_or(inout big_uint_t self, big_uint_t ss2);
void 				big_uint_bit_xor(inout big_uint_t self, big_uint_t ss2);
void 				big_uint_bit_not(inout big_uint_t self);
void				big_uint_bit_not2(inout big_uint_t self);


big_num_carry_t		big_uint_mul_int(inout big_uint_t self, big_num_strg_t ss2);
big_num_carry_t 	big_uint_mul_int_big(inout big_big_uint_t self, big_num_strg_t ss2);
big_num_carry_t		big_uint_mul(inout big_uint_t self, big_uint_t ss2, big_num_algo_t algorithm);
void				big_uint_mul_no_carry(inout big_uint_t self, big_uint_t ss2, out big_big_uint_t result, big_num_algo_t algorithm);

big_num_div_ret_t 	big_uint_div_int(inout big_uint_t self, big_num_strg_t divisor, out big_num_strg_t remainder);
big_num_div_ret_t 	big_uint_div(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, big_num_algo_t algorithm);
big_num_div_ret_t 	big_uint_div_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t remainder, big_num_algo_t algorithm);

big_num_ret_t		big_uint_pow(inout big_uint_t self, big_uint_t _pow);
void 				big_uint_sqrt(inout big_uint_t self);


void 				big_uint_clear_first_bits(inout big_uint_t self, size_t n);
bool 				big_uint_is_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_highest_bit_set(big_uint_t self);
bool 				big_uint_is_only_the_lowest_bit_set(big_uint_t self);
bool 				big_uint_is_zero(big_uint_t self);
bool 				big_uint_are_first_bits_zero(big_uint_t self, size_t bits);


void 				big_uint_init_uint(inout big_uint_t self, big_num_strg_t value);
big_num_carry_t		big_uint_init_ulint(inout big_uint_t self, big_num_lstrg_t value);
void 				big_uint_init_big_uint(inout big_uint_t self, big_uint_t value);
big_num_carry_t		big_uint_init_int(inout big_uint_t self, big_num_sstrg_t value);
big_num_carry_t		big_uint_init_lint(inout big_uint_t self, big_num_lsstrg_t value);
big_num_carry_t		big_uint_to_uint(big_uint_t self, out big_num_strg_t result);
big_num_carry_t		big_uint_to_int(big_uint_t self, out big_num_sstrg_t result);
big_num_carry_t		big_uint_to_luint(big_uint_t self, out big_num_lstrg_t result);
big_num_carry_t		big_uint_to_lint(big_uint_t self, out big_num_lsstrg_t result);

bool 				big_uint_cmp_smaller(big_uint_t self, big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger(big_uint_t self, big_uint_t l, ssize_t index);
bool 				big_uint_cmp_equal(big_uint_t self, big_uint_t l, ssize_t index);
bool 				big_uint_cmp_smaller_equal(big_uint_t self, big_uint_t l, ssize_t index);
bool 				big_uint_cmp_bigger_equal(big_uint_t self, big_uint_t l, ssize_t index);

//===============
// BigUint.c
//===============

#define TABLE_SIZE 			BIG_NUM_PREC
#define BIG_TABLE_SIZE 		(2*TABLE_SIZE)

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

// protected functions
big_num_carry_t			_big_uint_sub_uint(inout big_uint_t self, big_num_strg_t value, size_t index);
big_num_carry_t			_big_uint_add_uint(inout big_uint_t self, big_num_strg_t value, size_t index);
big_num_carry_t 		_big_uint_add_uint_big(inout big_big_uint_t self, big_num_strg_t value, size_t index);
big_num_carry_t 		_big_uint_add_two_uints(inout big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index);
big_num_carry_t 		_big_uint_add_two_uints_big(inout big_big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index);
big_num_sstrg_t  		_big_uint_find_leading_bit_in_word(big_num_strg_t x);

// private functions
big_num_carry_t 		_big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, out big_num_strg_t result);
big_num_carry_t			_big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t carry, out big_num_strg_t result);
big_num_carry_t			_big_uint_rcl2_one(inout big_uint_t self, big_num_strg_t c);
big_num_carry_t 		_big_uint_rcl2_one_big(inout big_big_uint_t self, big_num_carry_t c);
big_num_carry_t 		_big_uint_rcr2_one(inout big_uint_t self, big_num_strg_t c);
big_num_carry_t 		_big_uint_rcr2_one_big(inout big_big_uint_t self, big_num_carry_t c);
big_num_carry_t			_big_uint_rcl2(inout big_uint_t self, size_t bits, big_num_strg_t c);
big_num_carry_t			_big_uint_rcl2_big(inout big_big_uint_t self, size_t bits, big_num_strg_t c);
big_num_carry_t			_big_uint_rcr2(inout big_uint_t self, size_t bits, big_num_strg_t c);
big_num_carry_t 		_big_uint_rcr2_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c);
big_num_sstrg_t 		_big_uint_find_lowest_bit_in_word(big_num_strg_t x);
big_num_strg_t 			_big_uint_set_bit_in_word(inout big_num_strg_t value, size_t bit);
void 					_big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, out big_num_strg_t result_high, out big_num_strg_t result_low);
void					_big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, out big_num_strg_t r, out big_num_strg_t rest);


void 					_big_uint_rcl_move_all_words(inout big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_strg_t c);
void 					_big_uint_rcr_move_all_words(inout big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_strg_t c);
void 					_big_uint_rcl_move_all_words_big(inout big_big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_carry_t c);
void 					_big_uint_rcr_move_all_words_big(inout big_big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_strg_t c);

big_num_carry_t 		_big_uint_mul1(inout big_uint_t self, big_uint_t ss2);
void 					_big_uint_mul1_no_carry(inout big_uint_t self, big_uint_t ss2_, out big_big_uint_t result);
big_num_carry_t 		_big_uint_mul2(inout big_uint_t self, big_uint_t ss2);
void 					_big_uint_mul2_no_carry(inout big_uint_t self, big_uint_t ss2, out big_big_uint_t result);
void 					_big_uint_mul2_no_carry2(inout big_uint_t self, const big_num_strg_t ss1[BIG_NUM_PREC], const big_num_strg_t ss2[BIG_NUM_PREC], out big_big_uint_t result);
void					_big_uint_mul2_no_carry3(inout big_uint_t self, const big_num_strg_t ss1[BIG_NUM_PREC], const big_num_strg_t ss2[BIG_NUM_PREC], out big_big_uint_t result, size_t x1start, size_t x1size, size_t x2start, size_t x2size);


div_std_test_t			_big_uint_div_standard_test(inout big_uint_t self, big_uint_t divisor, out size_t m, out size_t n, out big_uint_t remainder);
div_std_test_t 			_big_uint_div_standard_test_big(inout big_big_uint_t self, big_big_uint_t divisor, out size_t m, out size_t n, out big_big_uint_t remainder);
div_calc_test_t			_big_uint_div_calculating_size(inout big_uint_t self, big_uint_t divisor, out size_t m, out size_t n);
div_calc_test_t 		_big_uint_div_calculating_size_big(inout big_big_uint_t self, big_big_uint_t divisor, out size_t m, out size_t n);
big_num_div_ret_t 		_big_uint_div1(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder);
big_num_div_ret_t 		_big_uint_div1_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t remainder);
big_num_div_ret_t		_big_uint_div1_calculate(inout big_uint_t self, big_uint_t divisor, out big_uint_t rest);
big_num_div_ret_t 		_big_uint_div1_calculate_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t rest);
big_num_div_ret_t 		_big_uint_div2(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder);
big_num_div_ret_t		_big_uint_div2_calculate(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, out size_t bits_diff);
big_num_div_ret_t		_big_uint_div2_find_leading_bits_and_check(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, out size_t table_id, out size_t index, out size_t divisor_table_id, out size_t divisor_index);
bool 					_big_uint_div2_divisor_greater_or_equal(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, size_t table_id, size_t index, size_t divisor_index);


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
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_uint_set_zero_big(inout big_big_uint_t self)
{
	for(size_t i=0 ; i < BIG_TABLE_SIZE ; ++i)
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
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_uint_set_one_big(inout big_big_uint_t self)
{
	big_uint_set_zero_big(self);
	self.table[0] = 1;
}

/**
 * this method sets the max value which self class can hold
 * @param[in, out] self the big num object
 */
void big_uint_set_max(inout big_uint_t self)
{
	for(size_t i=0 ; i < TABLE_SIZE; ++i)
		self.table[i] = BIG_NUM_MAX_VALUE;
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
void big_uint_set_from_table(inout big_uint_t self, big_num_strg_t temp_table[BIG_NUM_PREC], size_t temp_table_len)
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
			if( self.table[0] != BIG_NUM_MAX_VALUE )
				++(self.table[0]);
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
 * this method adds val to the existing value
 * @param[in, out] self the big num object
 * @param[in] val the value to add to the existing vlaue
 * @return big_num_carry_t the carry, if there was one
 */
big_num_carry_t big_uint_add_uint(inout big_uint_t self, big_num_strg_t val)
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
big_num_carry_t big_uint_add(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;	

	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_add_two_words(self.table[i], ss2.table[i], c, self.table[i]);

	return c;
}

/**
 * this method adding ss2 to the this and adding carry if it's defined. (this = this + ss2 + c)
 * @param[in, out] self the big num object
 * @param[in] ss2  the big num object to add to self
 * @param[in] c    must be zero or one (might be a bigger value than 1), set to 1 if previous operation resulted in a carry
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_add_big(inout big_big_uint_t self, big_big_uint_t ss2, big_num_carry_t c)
{
	size_t i;	

	for(i=0 ; i<BIG_TABLE_SIZE ; ++i)
		c = _big_uint_add_two_words(self.table[i], ss2.table[i], c, self.table[i]);

	return c;
}

/**
 * this method subtracts val from the existing value
 * @param[in, out] self the big num object
 * @param[in] val the value to subtract from self
 * @return big_num_carry_t the carry, if there was one
 */
big_num_carry_t	big_uint_sub_uint(inout big_uint_t self, big_num_strg_t val)
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
big_num_carry_t big_uint_sub(inout big_uint_t self, big_uint_t ss2, big_num_carry_t c)
{
	size_t i;
	for(i=0 ; i<TABLE_SIZE ; ++i)
		c = _big_uint_sub_two_words(self.table[i], ss2.table[i], c, self.table[i]);

	return c;
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
big_num_carry_t big_uint_sub_big(inout big_big_uint_t self, big_big_uint_t ss2, big_num_carry_t c)
{
	size_t i;
	for(i=0 ; i<BIG_TABLE_SIZE ; ++i)
		c = _big_uint_sub_two_words(self.table[i], ss2.table[i], c, self.table[i]);

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
big_num_strg_t big_uint_rcl(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_strg_t last_c   = 0;
	size_t rest_bits 		= bits;

	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcl_move_all_words(self, rest_bits, last_c, bits, c);

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
big_num_strg_t big_uint_rcl_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_strg_t last_c   = 0;
	size_t rest_bits 		= bits;

	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcl_move_all_words_big(self, rest_bits, last_c, bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to BIG_NUM_BITS_PER_UNIT-1 now
	if( rest_bits == 1 ) {
		last_c = _big_uint_rcl2_one_big(self, c);
	} else if( rest_bits == 2 ) {
		// performance tests showed that for rest_bits==2 it's better to use _big_uint_rcl2_one twice instead of _big_uint_rcl2(2,c)
		_big_uint_rcl2_one_big(self, c);
		last_c = _big_uint_rcl2_one_big(self, c);
	}
	else {
		last_c = _big_uint_rcl2_big(self, rest_bits, c);
	}
	
	return last_c;
}

/**
 * moving all bits into the right side 'bits' times
 * c . self . return value
 * 
 * @param[in, out] self the big num object
 * @param[in] bits number of bits to shift, range of <0, man * BIG_NUM_BITS_PER_UNIT>, or it can be even bigger then all bits will be set to 'c'
 * @param[in] c bit to insert in MSb
 * @return big_num_strg_t state of the last moved bit
 */
big_num_strg_t big_uint_rcr(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_carry_t last_c  	= 0;
	size_t rest_bits 			= bits;
	
	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcr_move_all_words(self, rest_bits, last_c, bits, c);

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
 * moving all bits into the right side 'bits' times
 * c . self . return value
 * 
 * @param[in, out] self the big num object
 * @param[in] bits number of bits to shift, range of <0, man * BIG_NUM_BITS_PER_UNIT>, or it can be even bigger then all bits will be set to 'c'
 * @param[in] c bit to insert in MSb
 * @return big_num_strg_t state of the last moved bit
 */
big_num_strg_t big_uint_rcr_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c)
{
	big_num_carry_t last_c    = 0;
	size_t rest_bits = bits;
	
	if( bits == 0 )
		return 0;

	if( bits >= BIG_NUM_BITS_PER_UNIT )
		_big_uint_rcr_move_all_words_big(self, rest_bits, last_c, bits, c);

	if( rest_bits == 0 ) {
		return last_c;
	}

	// rest_bits is from 1 to TTMATH_BITS_PER_UINT-1 now
	if( rest_bits == 1 ) {
		last_c = _big_uint_rcr2_one_big(self, c);
	} else if( rest_bits == 2 ) {
		// performance tests showed that for rest_bits==2 it's better to use Rcr2_one twice instead of Rcr2(2,c)
		_big_uint_rcr2_one_big(self, c);
		last_c = _big_uint_rcr2_one_big(self, c);
	} else {
		last_c = _big_uint_rcr2_big(self, rest_bits, c);
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

	if( a != TABLE_SIZE-1 ) {
		moving += ( TABLE_SIZE-1 - a ) * BIG_NUM_BITS_PER_UNIT;

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

	moving2 = BIG_NUM_BITS_PER_UNIT - moving2 - 1;
	big_uint_rcl(self, moving2, 0);

	return moving + moving2;
}

/**
 * this method moves all bits into the left side
 * @param[in, out] self the big num object
 * @return size_t value how many bits have been moved
 */
size_t big_uint_compensation_to_left_big(inout big_big_uint_t self)
{
	size_t moving = 0;

	// a - index a last word which is different from zero
	ssize_t a;
	for(a=BIG_TABLE_SIZE-1 ; a>=0 && self.table[a]==0 ; --a);

	if( a < 0 )
		return moving; // all words in table have zero

	if( a != BIG_TABLE_SIZE-1 )
	{
		moving += ( BIG_TABLE_SIZE-1 - a ) * BIG_NUM_BITS_PER_UNIT;

		// moving all words
		ssize_t i;
		for(i=BIG_TABLE_SIZE-1 ; a>=0 ; --i, --a)
			self.table[i] = self.table[a];

		// setting the rest word to zero
		for(; i>=0 ; --i)
			self.table[i] = 0;
	}

	size_t moving2 = _big_uint_find_leading_bit_in_word( self.table[BIG_TABLE_SIZE-1] );
	// moving2 is different from -1 because the value table[BIG_TABLE_SIZE-1]
	// is not zero

	moving2 = BIG_NUM_BITS_PER_UNIT - moving2 - 1;
	big_uint_rcl_big(self, moving2, 0);

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
bool big_uint_find_leading_bit(big_uint_t self, out size_t table_id, out size_t index)
{
	for(table_id=TABLE_SIZE-1 ; (table_id)!=0 && self.table[table_id]==0 ; --(table_id));

	if( table_id==0 && self.table[table_id]==0 ) {
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
 * @param[out] index 'self' != 0: the index of self set bit in the word <0..BIG_NUM_BITS_PER_UNIT), else 0
 * @return true: self' is not zero
 * @return false: both 'table_id' and 'index' are zero
 */
bool big_uint_find_lowest_bit(big_uint_t self, out size_t table_id, out size_t index)
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
big_num_strg_t big_uint_get_bit(big_uint_t self, size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_UNIT;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_UNIT;

	big_num_strg_t temp = self.table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(temp, bit);

	return res;
}

/**
 * setting the 'bit_index' bit
 * @param[in, out] self the big num object
 * @param[in] bit_index bigger or equal zero
 * @return bool the last state of the bit 
 */
big_num_strg_t big_uint_set_bit(inout big_uint_t self, size_t bit_index)
{
	size_t index = bit_index / BIG_NUM_BITS_PER_UNIT;
	size_t bit   = bit_index % BIG_NUM_BITS_PER_UNIT;

	big_num_strg_t temp = self.table[index];
	big_num_strg_t res  = _big_uint_set_bit_in_word(temp, bit);

	return res;
}

/**
 * this method performs a bitwise operation AND 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_and(inout big_uint_t self, big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] &= ss2.table[x];
}

/**
 * this method performs a bitwise operation OR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_or(inout big_uint_t self, big_uint_t ss2)
{
	for(size_t x=0 ; x<TABLE_SIZE ; ++x)
		self.table[x] |= ss2.table[x];
}

/**
 * this method performs a bitwise operation XOR 
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 */
void big_uint_bit_xor(inout big_uint_t self, big_uint_t ss2)
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

		big_num_strg_t mask  = BIG_NUM_MAX_VALUE;
		size_t shift = BIG_NUM_BITS_PER_UNIT - index - 1;

		if(shift > 0)
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
 * multiplication: self = self * ss2
 * 
 * it can return a carry
 * @param[in, out] self the big num object
 * @param[in] ss2 number to multiply by
 * @return big_num_carry_t carry
 */
big_num_carry_t big_uint_mul_int_big(inout big_big_uint_t self, big_num_strg_t ss2)
{
	big_num_strg_t r1, r2;
	size_t x1;
	big_num_carry_t c = 0;

	big_big_uint_t u = self;
	big_uint_set_zero_big(self);

	if( ss2 == 0 ) {
		return 0;
	}

	for(x1=0 ; x1<BIG_TABLE_SIZE-1 ; ++x1) {
		_big_uint_mul_two_words(u.table[x1], ss2, r2, r1);
		c += _big_uint_add_two_uints_big(self, r2, r1, x1);
	}

	// x1 = BIG_TABLE_SIZE-1  (last word)
	_big_uint_mul_two_words(u.table[x1], ss2, r2, r1);
	c += (r2!=0) ? 1 : 0;
	c += _big_uint_add_uint_big(self, r1, x1);

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
big_num_carry_t big_uint_mul(inout big_uint_t self, big_uint_t ss2, big_num_algo_t algorithm)
{
	switch (algorithm) {
		case BIG_NUM_MUL1: return _big_uint_mul1(self, ss2); break;
		case BIG_NUM_MUL2: return _big_uint_mul2(self, ss2); break;
		default: _big_uint_mul1(self, ss2); break;
	}
}

void big_uint_mul_no_carry(inout big_uint_t self, big_uint_t ss2, out big_big_uint_t result, big_num_algo_t algorithm)
{
	switch(algorithm) {
		case BIG_NUM_MUL1: _big_uint_mul1_no_carry(self, ss2, result); break;
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
big_num_div_ret_t big_uint_div_int(inout big_uint_t self, big_num_strg_t divisor, out big_num_strg_t remainder)
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
 * @param[in] divisor 
 * @param[out] remainder 
 * @param[in] algorithm 
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_uint_div(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, big_num_algo_t algorithm)
{
	switch( algorithm ){
		default:
		case BIG_NUM_DIV1:
			return _big_uint_div1(self, divisor, remainder);

		case BIG_NUM_DIV2:
			return _big_uint_div2(self, divisor, remainder);
	}
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
big_num_div_ret_t big_uint_div_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t remainder, big_num_algo_t algorithm)
{
	switch( algorithm ){
		default:
		case 1:
			return _big_uint_div1_big(self, divisor, remainder);
	}
}

/**
 * power self = self ^ pow
 * binary algorithm (r-to-l)
 * 
 * @param[in, out] self the big num object
 * @param[in] pow The power to raise self to
 * @return big_num_ret_t 
 */
big_num_ret_t big_uint_pow(inout big_uint_t self, big_uint_t _pow)
{
	if(big_uint_is_zero(_pow) && big_uint_is_zero(self))
		// we don't define zero^zero
		return BIG_NUM_INVALID_ARG;

	big_uint_t start = self;
	big_uint_t result;
	big_uint_set_one(result);
	big_num_carry_t c = 0;

	while( c == 0 )
	{
		if( (_pow.table[0] & 1) != 0 )
			c += big_uint_mul(result, start, BIG_NUM_MUL_DEF);

		_big_uint_rcr2_one(_pow, 0);
		if( big_uint_is_zero(_pow) )
			break;

		c += big_uint_mul(start, start, BIG_NUM_MUL_DEF);
	}

	self = result;

	return (c==0) ? 0 : 1;
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
 * @param[in] n 
 */
void big_uint_clear_first_bits(inout big_uint_t self, size_t n)
{
	if( n >= TABLE_SIZE*BIG_NUM_BITS_PER_UNIT ) {
		big_uint_set_zero(self);
		return;
	}

	size_t p = 0;

	// first we're clearing the whole words
	while( n >= BIG_NUM_BITS_PER_UNIT ) {
		self.table[p++] = 0;
		n   -= BIG_NUM_BITS_PER_UNIT;
	}

	if( n == 0 ) {
		return;
	}

	// and then we're clearing one word which has left
	// mask -- all bits are set to one
	big_num_strg_t mask = BIG_NUM_MAX_VALUE;

	mask = mask << n;

	(self.table[p]) &= mask;
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
	size_t index = bits / BIG_NUM_BITS_PER_UNIT;
	size_t rest  = bits % BIG_NUM_BITS_PER_UNIT;
	size_t i;

	for(i=0 ; i<index ; ++i)
		if( self.table[i] != 0 )
			return false;

	if( rest == 0 )
		return true;

	big_num_strg_t mask = BIG_NUM_MAX_VALUE >> (BIG_NUM_BITS_PER_UNIT - rest);

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
void big_uint_init_uint(inout big_uint_t self, big_num_strg_t value)
{
	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		self.table[i] = 0;

	self.table[0] = value;
}

/**
 * a constructor for converting big_num_lstrg_t int to big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_ulint(inout big_uint_t self, big_num_lstrg_t value)
{
	self.table[0] = big_num_strg_t(value);

	if( TABLE_SIZE == 1 ) {
		big_num_carry_t c = ((value >> BIG_NUM_BITS_PER_UNIT) == 0) ? 0 : 1;

		return c;
	}

	self.table[1] = big_num_strg_t((value >> BIG_NUM_BITS_PER_UNIT));

	for(size_t i=2 ; i<TABLE_SIZE ; ++i)
		self.table[i] = 0;

	return 0;
}

/**
 * a copy constructor
 * @param[in, out] self the big num object
 * @param[in] u the other big num object
 */
void big_uint_init_big_uint(inout big_uint_t self, big_uint_t value)
{
	for(size_t i=0 ; i<TABLE_SIZE ; ++i)
		self.table[i] = value.table[i];
}

/**
 * a constructor for converting the big_num_sstrg_t to big_uint_t
 * 
 * @param[in, out] self the big num object
 * @param[in] value
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
 * a constructor for converting big_num_lsstrg_t to big_uint_t
 * @param[in, out] self the big num object
 * @param[in] value
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_init_lint(inout big_uint_t self, big_num_lsstrg_t value)
{
	big_num_carry_t c = big_uint_init_ulint(self, big_num_lstrg_t(value));
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
big_num_carry_t big_uint_to_uint(big_uint_t self, out big_num_strg_t result)
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
 * @param[out] result 
 * @return big_num_carry_t
 */
big_num_carry_t big_uint_to_int(big_uint_t self, out big_num_sstrg_t result)
{
	big_num_strg_t _result;
	big_num_carry_t c = big_uint_to_uint(self, _result);
	result = big_num_sstrg_t(_result);
	return c;
}

/**
 * this method converts the value to big_num_lstrg_t type (long integer)
 * can return a carry if the value is too long to store it in big_num_lstrg_t type
 * @param[in, out] self the big num object
 * @param[out] result 
 * @return big_num_carry_t 
 */
big_num_carry_t	big_uint_to_luint(big_uint_t self, out big_num_lstrg_t result)
{
	result = self.table[0];
	result += big_num_lstrg_t(self.table[1]) << BIG_NUM_BITS_PER_UNIT;

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
big_num_carry_t	big_uint_to_lint(big_uint_t self, out big_num_lsstrg_t result)
{
	big_num_lstrg_t _result;
	big_num_carry_t c = big_uint_to_luint(self, _result);
	result = big_num_lsstrg_t(_result);
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
bool big_uint_cmp_smaller(big_uint_t self, big_uint_t l, ssize_t index)
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
 * @param[in] l the other big num object
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_bigger(big_uint_t self, big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= big_num_sstrg_t(TABLE_SIZE) )
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
bool big_uint_cmp_equal(big_uint_t self, big_uint_t l, ssize_t index)
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
 * @param[in] l the other big num object
 * @param[in] index 
 * @return true 
 * @return false 
 */
bool big_uint_cmp_smaller_equal(big_uint_t self, big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= big_num_sstrg_t(TABLE_SIZE))
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
bool big_uint_cmp_bigger_equal(big_uint_t self, big_uint_t l, ssize_t index)
{
	big_num_sstrg_t i;

	if( index==-1 || index>= big_num_sstrg_t(TABLE_SIZE))
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
big_num_carry_t _big_uint_add_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, out big_num_strg_t result)
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
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_uint(inout big_uint_t self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index], value, 0, self.table[index]);

	for(i=index+1 ; i < TABLE_SIZE && c != 0 ; ++i)
		c = _big_uint_add_two_words(self.table[i], 0, c, self.table[i]);
	
	return c;
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
big_num_carry_t _big_uint_add_uint_big(inout big_big_uint_t self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index], value, 0, self.table[index]);

	for(i=index+1 ; i < BIG_TABLE_SIZE && c != 0 ; ++i)
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
 * @param[in] x1 lower word
 * @param[in] x2 higher word
 * @param[in] index index <= value_size-2
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_two_uints(inout big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index],   x1, 0, self.table[index]);
	c = _big_uint_add_two_words(self.table[index+1], x2, c, self.table[index+1]);

	for(i=index+2 ; i<TABLE_SIZE && c != 0; ++i)
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
 * @param[in] x1 lower word
 * @param[in] x2 higher word
 * @param[in] index index <= value_size-2
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_add_two_uints_big(inout big_big_uint_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_add_two_words(self.table[index],   x1, 0, self.table[index]);
	c = _big_uint_add_two_words(self.table[index+1], x2, c, self.table[index+1]);

	for(i=index+2 ; i<BIG_TABLE_SIZE && c != 0; ++i)
		c = _big_uint_add_two_words(self.table[i], 0, c, self.table[i]);
	
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
big_num_carry_t _big_uint_sub_two_words(big_num_strg_t a, big_num_strg_t b, big_num_carry_t carry, out big_num_strg_t result)
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
 * @param[in] value 
 * @param[in] index 
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_sub_uint(inout big_uint_t self, big_num_strg_t value, size_t index)
{
	size_t i;
	big_num_carry_t c;

	c = _big_uint_sub_two_words(self.table[index], value, 0, self.table[index]);

	for(i=index+1 ; i<TABLE_SIZE && c != 0; ++i)
		c = _big_uint_sub_two_words(self.table[i], 0, c, self.table[i]);

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
big_num_carry_t _big_uint_rcl2_one_big(inout big_big_uint_t self, big_num_carry_t c)
{
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = 1;

	for(i=0 ; i<BIG_TABLE_SIZE ; ++i) {
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
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2_one(inout big_uint_t self, big_num_carry_t c)
{
	big_num_sstrg_t i; // signed i
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_HIGHEST_BIT;

	for(i=big_num_sstrg_t(TABLE_SIZE)-1 ; i>=0 ; --i) {
		new_c    		= (self.table[i] & 1) != 0? BIG_NUM_HIGHEST_BIT : 0;
		self.table[i] 	= (self.table[i] >> 1) | c;
		c        		= new_c;
	}

	c = (c != 0)? 1 : 0;

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
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2_one_big(inout big_big_uint_t self, big_num_carry_t c)
{
	big_num_sstrg_t i; // signed i
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_HIGHEST_BIT;

	for(i=big_num_sstrg_t(BIG_TABLE_SIZE)-1 ; i>=0 ; --i) {
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
 * @param[in] bits 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcl2(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE >> move;

	for(i=0 ; i<TABLE_SIZE ; ++i) {
		new_c    		= self.table[i] >> move;
		self.table[i] 	= (self.table[i] << bits) | c;
		c        		= new_c;
	}

	return (c & 1);
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
big_num_carry_t _big_uint_rcl2_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	size_t i;
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE >> move;

	for(i=0 ; i<BIG_TABLE_SIZE ; ++i) {
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
 * @param[in] bits 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2(inout big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	ssize_t i; // signed
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE << move;

	for(i=TABLE_SIZE-1 ; i>=0 ; --i) {
		new_c    		= self.table[i] << move;
		self.table[i] 	= (self.table[i] >> bits) | c;
		c        		= new_c;
	}

	c = (c & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;

	return c;
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
 * @param[in] bits 
 * @param[in] c carry
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_rcr2_big(inout big_big_uint_t self, size_t bits, big_num_carry_t c)
{
	size_t move = BIG_NUM_BITS_PER_UNIT - bits;
	ssize_t i; // signed
	big_num_carry_t new_c;

	if( c != 0 )
		c = BIG_NUM_MAX_VALUE << move;

	for(i=BIG_TABLE_SIZE-1 ; i>=0 ; --i) {
		new_c    		= self.table[i] << move;
		self.table[i] 	= (self.table[i] >> bits) | c;
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

	big_num_sstrg_t bit = BIG_NUM_BITS_PER_UNIT - 1;
	
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
 * @param[in, out] value 
 * @param[in] bit the bit to set, between <0,BIG_NUM_BITS_PER_UNIT-1>
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
 * @param[in] a 
 * @param[in] b 
 * @param[out] result_high 
 * @param[out] result_low 
 */
void _big_uint_mul_two_words(big_num_strg_t a, big_num_strg_t b, out big_num_strg_t result_high, out big_num_strg_t result_low)
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
 * @param[in] a 
 * @param[in] b 
 * @param[in] c
 * @param[out] r 
 * @param[out] rest 
 */
void _big_uint_div_two_words(big_num_strg_t a, big_num_strg_t b, big_num_strg_t c, out big_num_strg_t r, out big_num_strg_t rest)
{
	big_num_lstrg_t ab_u = (big_num_lstrg_t(a) << BIG_NUM_BITS_PER_UNIT) | big_num_lstrg_t(b);

	r    = big_num_strg_t(ab_u / c);
	rest = big_num_strg_t(ab_u % c);
}




/**
 * an auxiliary method for moving bits into the left hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param[out] rest_bits 
 * @param[out] last_c 
 * @param[in] bits 
 * @param[in] c carry
 */
void _big_uint_rcl_move_all_words(inout big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits	      	= bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && rest_bits == 0 )
			last_c = self.table[0] & 1;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<TABLE_SIZE ; ++i)
			self.table[i] = mask;

		rest_bits = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < TABLE_SIZE
		ssize_t first, second;
		last_c = self.table[TABLE_SIZE - all_words] & 1; // all_words is greater than 0

		// copying the first part of the value
		for(first = TABLE_SIZE-1, second=first-all_words ; second>=0 ; --first, --second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first>=0 ; --first )
			self.table[first] = mask;
	}
}

/**
 * an auxiliary method for moving bits into the left hand side. This method moves only words
 * @param[in, out] self the big num object
 * @param[out] rest_bits 
 * @param[out] last_c 
 * @param[in] bits 
 * @param[in] c carry
 */
void _big_uint_rcl_move_all_words_big(inout big_big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits      		= bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= BIG_TABLE_SIZE ) {
		if( all_words == BIG_TABLE_SIZE && rest_bits == 0 )
			last_c = self.table[0] & 1;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<BIG_TABLE_SIZE ; ++i)
			self.table[i] = mask;

		rest_bits = 0;
	}
	else
	if( all_words > 0 ) {
		// 0 < all_words < BIG_TABLE_SIZE
		ssize_t first, second;
		last_c = self.table[BIG_TABLE_SIZE- all_words] & 1; // all_words is greater than 0

		// copying the first part of the value
		for(first = BIG_TABLE_SIZE-1, second=first-all_words ; second>=0 ; --first, --second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first>=0 ; --first )
			self.table[first] = mask;
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
void _big_uint_rcr_move_all_words(inout big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits      		= bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= TABLE_SIZE ) {
		if( all_words == TABLE_SIZE && rest_bits == 0 )
			last_c = (self.table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0? 1 : 0;
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
		for(first=0, second=all_words ; second<TABLE_SIZE ; ++first, ++second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first<TABLE_SIZE ; ++first )
			self.table[first] = mask;
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
void _big_uint_rcr_move_all_words_big(inout big_big_uint_t self, out size_t rest_bits, out big_num_carry_t last_c, size_t bits, big_num_carry_t c)
{
	rest_bits      		= bits % BIG_NUM_BITS_PER_UNIT;
	ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
	big_num_strg_t mask = ( c != 0 ) ? BIG_NUM_MAX_VALUE : 0;


	if( all_words >= BIG_TABLE_SIZE ) {
		if( all_words == BIG_TABLE_SIZE && rest_bits == 0 )
			last_c = (self.table[BIG_TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
		// else: last_c is default set to 0

		// clearing
		for(size_t i = 0 ; i<BIG_TABLE_SIZE ; ++i)
			self.table[i] = mask;

		rest_bits = 0;
	} else if( all_words > 0 ) {
		// 0 < all_words < BIG_TABLE_SIZE

		ssize_t first, second;
		last_c = (self.table[all_words - 1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0; // all_words is > 0

		// copying the first part of the value
		for(first=0, second=all_words ; second<BIG_TABLE_SIZE ; ++first, ++second)
			self.table[first] = self.table[second];

		// setting the rest to 'c'
		for( ; first<BIG_TABLE_SIZE ; ++first )
			self.table[first] = mask;
	}
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t carry
 */
big_num_carry_t _big_uint_mul1(inout big_uint_t self, big_uint_t ss2)
{
	big_uint_t ss1 = self;
	big_uint_set_zero(self);

	for(size_t i=0; i < TABLE_SIZE*BIG_NUM_BITS_PER_UNIT ; ++i) {
		if( big_uint_add(self, self, 0) != 0 ) {
			return 1;
		}

		if( big_uint_rcl(ss1, 1, 0) != 0 ) {
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
void _big_uint_mul1_no_carry(inout big_uint_t self, big_uint_t ss2_, out big_big_uint_t result)
{
	big_big_uint_t ss2;
	size_t i;

	// copying *this into result and ss2_ into ss2
	for(i=0 ; i<TABLE_SIZE ; ++i)
	{
		result.table[i] = self.table[i];
		ss2.table[i]    = ss2_.table[i];
	}

	// cleaning the highest bytes in result and ss2
	for( ; i < BIG_TABLE_SIZE ; ++i)
	{
		result.table[i] = 0;
		ss2.table[i]    = 0;
	}

	// multiply
	// (there will not be a carry)
	big_big_uint_t ss1 = result;
	big_uint_set_zero_big(result);

	for(size_t i=0; i < BIG_TABLE_SIZE*BIG_NUM_BITS_PER_UNIT ; ++i) {
		if( big_uint_add_big(result, result, 0) != 0 ) {
			return;
		}

		if( big_uint_rcl_big(ss1, 1, 0) != 0 ) {
			if( big_uint_add_big(result, ss2, 0) != 0 ) {
				return;
			}
		}
	}

	return;
}

/**
 * multiplication: self = self * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the other big num object
 * @return big_num_carry_t 
 */
big_num_carry_t _big_uint_mul2(inout big_uint_t self, big_uint_t ss2)
{
	big_big_uint_t result;
	size_t i;
	big_num_carry_t c = 0;

	_big_uint_mul2_no_carry(self, ss2, result);

	// copying result
	for(i=0 ; i<TABLE_SIZE ; ++i)
		self.table[i] = result.table[i];

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
void _big_uint_mul2_no_carry(inout big_uint_t self, big_uint_t ss2, out big_big_uint_t result)
{
	_big_uint_mul2_no_carry2(self, self.table, ss2.table, result);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] ss1 
 * @param[in] ss2 
 * @param[out] result 
 */
void _big_uint_mul2_no_carry2(inout big_uint_t self, const big_num_strg_t ss1[BIG_NUM_PREC], const big_num_strg_t ss2[BIG_NUM_PREC], out big_big_uint_t result)
{
	size_t x1size  = BIG_TABLE_SIZE, 	x2size  = BIG_TABLE_SIZE;
	size_t x1start = 0,       			x2start = 0;

	if( BIG_TABLE_SIZE > 2 ) {	
		// if the ss_size is smaller than or equal to 2
		// there is no sense to set x1size (and others) to another values

		for(x1size=BIG_TABLE_SIZE ; x1size>0 && ss1[x1size-1]==0 ; --x1size);
		for(x2size=BIG_TABLE_SIZE ; x2size>0 && ss2[x2size-1]==0 ; --x2size);

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
void _big_uint_mul2_no_carry3(inout big_uint_t self, const big_num_strg_t ss1[BIG_NUM_PREC], const big_num_strg_t ss2[BIG_NUM_PREC], out big_big_uint_t result, size_t x1start, size_t x1size, size_t x2start, size_t x2size)
{
	big_num_strg_t r2, r1;

	for(size_t i=0 ; i < BIG_TABLE_SIZE; ++i)
		result.table[i] = 0;

	if( x1size==0 || x2size==0 )
		return;

	for(size_t x1=x1start ; x1<x1size ; ++x1)
	{
		for(size_t x2=x2start ; x2<x2size ; ++x2)
		{
			_big_uint_mul_two_words(ss1[x1], ss2[x2], r2, r1);
			size_t i;
			big_num_carry_t c;

			c = _big_uint_add_two_words(result.table[x2+x1],   r1, 0, result.table[x2+x1]);
			c = _big_uint_add_two_words(result.table[x2+x1+1], r2, c, result.table[x2+x1+1]);

			for(i=x2+x1+2 ; i<BIG_TABLE_SIZE && c != 0 ; ++i)
				c = _big_uint_add_two_words(result.table[i], 0, c, result.table[i]);
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
div_std_test_t _big_uint_div_standard_test(inout big_uint_t self, big_uint_t divisor, out size_t m, out size_t n, out big_uint_t remainder)
{
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
 * @param[out] m 
 * @param[out] n 
 * @param[out] remainder 
 * @return div_std_test_t 
 */
div_std_test_t _big_uint_div_standard_test_big(inout big_big_uint_t self, big_big_uint_t divisor, out size_t m, out size_t n, out big_big_uint_t remainder)
{
	switch( _big_uint_div_calculating_size_big(self, divisor, m, n) ) {
	case 4: // 'this' is equal divisor
		big_uint_set_zero_big(remainder);

		big_uint_set_one_big(self);
		return BIG_NUM_STD_DIV_NONE;
	case 3: // 'this' is smaller than divisor
		remainder = self; // copy

		big_uint_set_zero_big(self);
		return BIG_NUM_STD_DIV_NONE;
	case 2: // 'this' is zero
		big_uint_set_zero_big(remainder);

		big_uint_set_zero_big(self);
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
div_calc_test_t _big_uint_div_calculating_size(inout big_uint_t self, big_uint_t divisor, out size_t m, out size_t n)
{
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
		ssize_t i;
		for(i = ssize_t(n) ; i!=0 && self.table[i]==divisor.table[i] ; --i);
		
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
 * @param[in] divisor
 * @param[out] m 
 * @param[out] n 
 * @return div_calc_test_t 
 */
div_calc_test_t _big_uint_div_calculating_size_big(inout big_big_uint_t self, big_big_uint_t divisor, out size_t m, out size_t n)
{
	m = n = BIG_TABLE_SIZE-1;

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
		for(i = ssize_t(n) ; i!=0 && self.table[i]==divisor.table[i] ; --i);
		
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
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder)
{
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
 * @param[out] remainder the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t remainder)
{
	size_t m,n;
	div_std_test_t test;

	test = _big_uint_div_standard_test_big(self, divisor, m, n, remainder);
	switch (test) {
		case BIG_NUM_STD_DIV_NONE: return BIG_NUM_DIV_OK;
		case BIG_NUM_STD_DIV_ZERO: return BIG_NUM_DIV_ZERO;
		default: break;
	}

	return _big_uint_div1_calculate_big(self, divisor, remainder);
}

/**
 * 
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1_calculate(inout big_uint_t self, big_uint_t divisor, out big_uint_t rest)
{
	big_num_sstrg_t loop;
	big_num_carry_t c;

	big_uint_set_zero(rest);
	loop = TABLE_SIZE * BIG_NUM_BITS_PER_UNIT;
	c = 0;

	for ( ;; ) {
		for ( ;; ) {
			// div_a
			c = big_uint_rcl(self, 1, c);
			c = big_uint_add(rest, rest, c);
			c = big_uint_sub(rest, divisor,c);

			c = c==0 ? 1 : 0;
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
 * @param[out] rest the remaining part of the division (whole number)
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t _big_uint_div1_calculate_big(inout big_big_uint_t self, big_big_uint_t divisor, out big_big_uint_t rest)
{
	big_num_sstrg_t loop;
	big_num_carry_t c;

	big_uint_set_zero_big(rest);
	loop = BIG_TABLE_SIZE * BIG_NUM_BITS_PER_UNIT;
	c = 0;

	for ( ;; ) {
		for ( ;; ) {
			// div_a
			c = big_uint_rcl_big(self, 1, c);
			c = big_uint_add_big(rest, rest, c);
			c = big_uint_sub_big(rest, divisor,c);

			c = c == 0 ? 1 : 0;
			if (c == 0)
				break; // goto div_d
			
			// div_b
			--loop;
			if (loop > 0)
				continue; // goto div_a

			c = big_uint_rcl_big(self, 1, c);
			return BIG_NUM_DIV_OK;
		}

		for ( ;; ) {
			// div_d
			--loop;
			if (loop != 0) {
				// div_c
				c = big_uint_rcl_big(self, 1, c);
				c = big_uint_add_big(rest, rest, c);
				c = big_uint_add_big(rest, divisor, 0);

				if (c != 0) {
					// "goto" div_b
					--loop;
					if (loop > 0)
						break; // goto div_a
						
					c = big_uint_rcl_big(self, 1, c);
					return 0;
				}
				else
					continue; // goto div_d
			}

			c = big_uint_rcl_big(self, 1, c);
			c = big_uint_add_big(rest, divisor, 0);

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
big_num_div_ret_t  _big_uint_div2(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder)
{
	size_t bits_diff;
	big_num_div_ret_t status = _big_uint_div2_calculate(self, divisor, remainder, bits_diff);
	if( status < BIG_NUM_DIV_BUSY )
		return status;

	if( big_uint_cmp_bigger_equal(self, divisor, -1) ) {
		_big_uint_div2(self, divisor, remainder);
		big_uint_set_bit(self, bits_diff);
	} else {
		remainder = self;

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
big_num_div_ret_t _big_uint_div2_calculate(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, out size_t bits_diff)
{
	size_t table_id, index;
	size_t divisor_table_id, divisor_index;

	big_num_div_ret_t status = _big_uint_div2_find_leading_bits_and_check(self,
									divisor, remainder,
									table_id, index,
									divisor_table_id, divisor_index);

	if( status < BIG_NUM_DIV_BUSY )
		return status;
	
	// here we know that 'this' is greater than divisor
	// then 'index' is greater or equal 'divisor_index'
	bits_diff = index - divisor_index;

	big_uint_t divisor_copy = divisor;
	big_uint_rcl(divisor_copy, bits_diff, 0);

	if( big_uint_cmp_smaller(self, divisor_copy, ssize_t(table_id)) )
	{
		big_uint_rcr(divisor_copy, 1, 0);
		--(bits_diff);
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
big_num_div_ret_t _big_uint_div2_find_leading_bits_and_check(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, out size_t table_id, out size_t index, out size_t divisor_table_id, out size_t divisor_index)
{
	if(false == big_uint_find_leading_bit(divisor, divisor_table_id, divisor_index) ) {
		// division by zero
		return BIG_NUM_DIV_ZERO;
	}

	if(	!big_uint_find_leading_bit(self, table_id, index) ) {
		// zero is divided by something
		big_uint_set_zero(self);

		big_uint_set_zero(remainder);

		return BIG_NUM_DIV_OK;
	}

	divisor_index += divisor_table_id * BIG_NUM_BITS_PER_UNIT;
	index         += table_id         * BIG_NUM_BITS_PER_UNIT;

	if( divisor_table_id == 0 ) {
		// dividor has only one 32-bit word

		big_num_strg_t r;
		big_uint_div_int(self, divisor.table[0], r);

		big_uint_set_zero(remainder);
		remainder.table[0] = r;

		return BIG_NUM_DIV_OK;
	}


	if( _big_uint_div2_divisor_greater_or_equal(self,
			divisor, remainder,
			table_id, index,
			divisor_index) ) {
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
bool _big_uint_div2_divisor_greater_or_equal(inout big_uint_t self, big_uint_t divisor, out big_uint_t remainder, size_t table_id, size_t index, size_t divisor_index)
{
	if( divisor_index > index ) {
		// divisor is greater than self

		remainder = self;

		big_uint_set_zero(self);

		return true;
	}

	if( divisor_index == index ) {
		// table_id == divisor_table_id as well

		ssize_t i;
		for(i = ssize_t(table_id) ; i!=0 && self.table[i]==divisor.table[i] ; --i);
		
		if( self.table[i] < divisor.table[i] )
		{
			// divisor is greater than 'this'

			remainder = self;

			big_uint_set_zero(self);

			return true;
		} else if( self.table[i] == divisor.table[i] ) {
			// divisor is equal 'this'

			big_uint_set_zero(remainder);

			big_uint_set_one(self);

			return true;
		}
	}

	return false;
}



//===============
// BigInt.h
//===============

#define big_int_t	big_uint_t

#define BIG_INT_SIGN_CHANGE_OK		0
#define BIG_INT_SIGN_CHANGE_FAILED	1
#define big_int_sign_ret_t			uint

void 				big_int_set_max(inout big_int_t self);
void 				big_int_set_min(inout big_int_t self);
void 				big_int_set_zero(inout big_int_t self);
void 				big_int_swap(inout big_int_t self, inout big_int_t ss2);

void				big_int_set_sign_one(inout big_int_t self);
big_int_sign_ret_t	big_int_change_sign(inout big_int_t self);
void				big_int_set_sign(inout big_int_t self);
bool				big_int_is_sign(big_int_t self);
big_num_strg_t		big_int_abs(inout big_int_t self);

big_num_carry_t 	big_int_add(inout big_int_t self, big_int_t ss2);
big_num_strg_t		big_int_add_int(inout big_int_t self, big_num_strg_t value);
big_num_carry_t 	big_int_sub(inout big_int_t self, big_int_t ss2);
big_num_strg_t		big_int_sub_int(inout big_int_t self, big_num_strg_t value);

size_t	 			big_int_compensation_to_left(inout big_int_t self);

big_num_carry_t		big_int_mul_int(inout big_int_t self, big_num_sstrg_t ss2);
big_num_carry_t		big_int_mul(inout big_int_t self, big_int_t ss2);

big_num_div_ret_t 	big_int_div_int(inout big_int_t self, big_num_sstrg_t divisor, out big_num_sstrg_t remainder);
big_num_div_ret_t 	big_int_div(inout big_int_t self, big_int_t divisor, out big_int_t remainder);

big_num_ret_t		big_int_pow(inout big_int_t self, big_int_t _pow);

big_num_carry_t		big_int_init_uint(inout big_int_t self, big_num_strg_t value);
big_num_carry_t		big_int_init_ulint(inout big_int_t self, big_num_lstrg_t value);
big_num_carry_t		big_int_init_big_uint(inout big_int_t self, big_uint_t value);
void				big_int_init_int(inout big_int_t self, big_num_sstrg_t value);
big_num_carry_t		big_int_init_lint(inout big_int_t self, big_num_lsstrg_t value);
void				big_int_init_big_int(inout big_int_t self, big_int_t value);
big_num_carry_t		big_int_to_uint(big_int_t self, out big_num_strg_t result);
big_num_carry_t		big_int_to_int(big_int_t self, out big_num_sstrg_t result);
big_num_carry_t		big_int_to_luint(big_int_t self, out big_num_lstrg_t result);
big_num_carry_t		big_int_to_lint(big_int_t self, out big_num_lsstrg_t result);

bool 				big_int_cmp_smaller(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger(big_int_t self, big_int_t l);
bool 				big_int_cmp_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_smaller_equal(big_int_t self, big_int_t l);
bool 				big_int_cmp_bigger_equal(big_int_t self, big_int_t l);

//===============
// BigInt.c
//===============

// private functions
big_num_carry_t		_big_int_correct_carry_after_adding(inout big_int_t self, bool p1_is_sign, bool p2_is_sign);
big_num_carry_t		_big_int_add_int(inout big_int_t self, big_num_strg_t value, size_t index);
big_num_carry_t 	_big_int_add_two_ints(inout big_int_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index);

big_num_carry_t		_big_int_correct_carry_after_subtracting(inout big_int_t self, bool p1_is_sign, bool p2_is_sign);
big_num_carry_t		_big_int_sub_int(inout big_int_t self, big_num_strg_t value, size_t index);
big_num_carry_t		_big_int_check_min_carry(inout big_int_t self, bool ss1_is_sign, bool ss2_is_sign);

big_num_carry_t		_big_int_pow2(inout big_int_t self, const big_int_t pow);

/**
 * this method sets the max value which this class can hold
 * (all bits will be one besides the last one)
 * @param[in, out] self the big num object
 */
void big_int_set_max(inout big_int_t self)
{
	big_uint_set_max(self);
	self.table[TABLE_SIZE-1] = ~BIG_NUM_HIGHEST_BIT;
}

/**
 * this method sets the min value which this class can hold
 * (all bits will be zero besides the last one which is one)
 * @param[in, out] self the big num object
 */
void big_int_set_min(inout big_int_t self)
{
	big_uint_set_zero(self);
	self.table[TABLE_SIZE-1] = BIG_NUM_HIGHEST_BIT;
}

/**
 * 
 * @param[in, out] self the big num object
 */
void big_int_set_zero(inout big_int_t self)
{
	big_uint_set_zero(self);
}

/**
 * this method swappes self and ss2
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the value to swap with self, ss2 contains self after swap
 */
void big_int_swap(inout big_int_t self, inout big_int_t ss2)
{
	big_uint_swap(self, ss2);
}

/**
 * this method sets -1 as the value
 * (-1 is equal the max value in an unsigned type)
 * @param[in, out] self the big num object
 */
void big_int_set_sign_one(inout big_int_t self)
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
big_int_sign_ret_t big_int_change_sign(inout big_int_t self)
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
	if( big_uint_is_only_the_highest_bit_set(self) )
		return 1;

	big_uint_t temp = self;
	big_uint_set_zero(self);
	big_uint_sub(self, temp, 0);

	return 0;
}

/**
 * this method sets the sign
 * samples
 * -	 1  . -1
 * - 	-2 . -2
 * from a positive value we make a negative value,
 * if the value is negative we do nothing
 * @param[in, out] self the big num object
 */
void big_int_set_sign(inout big_int_t self)
{
	if (big_int_is_sign(self))
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
big_num_strg_t big_int_abs(inout big_int_t self)
{
	if (!big_int_is_sign(self))
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
big_num_carry_t big_int_add(inout big_int_t self, big_int_t ss2)
{
	bool p1_is_sign = big_int_is_sign(self);
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
big_num_strg_t big_int_add_int(inout big_int_t self, big_num_strg_t value)
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
big_num_carry_t big_int_sub(inout big_int_t self, big_int_t ss2)
{
	bool p1_is_sign = big_int_is_sign(self);
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
big_num_strg_t big_int_sub_int(inout big_int_t self, big_num_strg_t value)
{
	return _big_int_sub_int(self, value, 0);
}

/**
 * this method moves all bits into the left side
 * @param[in, out] self the big num object
 * @return size_t value how many bits have been moved
 */
size_t big_int_compensation_to_left(inout big_int_t self)
{
	return big_uint_compensation_to_left(self);
}

/**
 * multiplication: this = this * ss2
 * @param[in, out] self the big num object
 * @param[in] ss2 the value to multiply self by
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_mul_int(inout big_int_t self, big_num_sstrg_t ss2)
{
	bool ss1_is_sign, ss2_is_sign;
	big_num_carry_t c;

	ss1_is_sign = big_int_is_sign(self);

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

	c  = big_uint_mul_int(self, big_num_strg_t(ss2));
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
big_num_carry_t	big_int_mul(inout big_int_t self, big_int_t ss2)
{
	bool ss1_is_sign, ss2_is_sign;
	big_num_carry_t c;

	ss1_is_sign = big_int_is_sign(self);
	ss2_is_sign = big_int_is_sign(ss2);

	/*
		we don't have to check the carry from Abs (values will be correct
		because next we're using the method Mul from the base class UInt
		which is without a sign)
	*/
	big_int_abs(self);
	big_int_abs(ss2);

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
 * -  	 20 /  3 -. result:  6   remainder:  2
 * -  	-20 /  3 -. result: -6   remainder: -2
 * -  	 20 / -3 -. result: -6   remainder:  2
 * -  	-20 / -3 -. result:  6   remainder: -2
 * 
 * in other words: this(old) = ss2 * this(new)(result) + remainder
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[in] remainder the remainder
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_int_div_int(inout big_int_t self, big_num_sstrg_t divisor, out big_num_sstrg_t remainder)
{
	bool self_is_sign, divisor_is_sign;

	self_is_sign = big_int_is_sign(self);

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
	big_num_div_ret_t c = big_uint_div_int(self, big_num_strg_t(divisor), rem);

	if( self_is_sign != divisor_is_sign )
		big_int_set_sign(self);

	if( self_is_sign )
		remainder = -big_num_sstrg_t(rem);
	else
		remainder = big_num_sstrg_t(rem);

	return c;
}

/**
 * division this = this / divisor
 * returned values:
 * -  0 - ok
 * -  1 - division by zero
 * 
 * for example: (result means 'this')
 * -  	 20 /  3 -. result:  6   remainder:  2
 * -  	-20 /  3 -. result: -6   remainder: -2
 * -  	 20 / -3 -. result: -6   remainder:  2
 * -  	-20 / -3 -. result:  6   remainder: -2
 * 
 * in other words: this(old) = divisor * this(new)(result) + remainder
 * @param[in, out] self the big num object
 * @param[in] divisor the value to divide self by
 * @param[out] remainder the remainder
 * @return big_num_div_ret_t 
 */
big_num_div_ret_t big_int_div(inout big_int_t self, big_int_t divisor, out big_int_t remainder)
{
	bool self_is_sign, divisor_is_sign;

	self_is_sign = big_int_is_sign(self);
	divisor_is_sign = big_int_is_sign(divisor);

	/*
		we don't have to test the carry from Abs as well as in Mul
	*/
	big_int_abs(self);
	big_int_abs(divisor);

	big_num_div_ret_t c = big_uint_div(self, divisor, remainder, BIG_NUM_DIV_DEF);

	if( self_is_sign != divisor_is_sign )
		big_int_set_sign(self);

	if( self_is_sign )
		big_int_set_sign(remainder);

	return c;
}

/**
 * power this = this ^ pow
 * @param[in, out] self the big num object
 * @param[in] pow the power to raise self to
 * @return big_num_ret_t 
 */
big_num_ret_t big_int_pow(inout big_int_t self, big_int_t _pow)
{
	bool was_sign = big_int_is_sign(self);
	big_num_strg_t c = 0;

	if( was_sign )
		c += big_int_abs(self);

	big_num_ret_t c_temp = big_uint_pow(self, _pow);
	if( c_temp > 0 )
		return c_temp; // c_temp can be: 0, 1 or 2
	
	if( was_sign && (_pow.table[0] & 1) == 1 )
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
big_num_carry_t big_int_init_uint(inout big_int_t self, big_num_strg_t value)
{
	for(size_t i = 1; i < TABLE_SIZE ; ++i)
		self.table[i] = 0;
	self.table[0] = value;

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
big_num_carry_t	big_int_init_ulint(inout big_int_t self, big_num_lstrg_t value)
{
	big_num_carry_t c = big_uint_init_ulint(self, value);

	if( c != 0)
		return 1;

	if( TABLE_SIZE == 1 )
		return ((self.table[0] & BIG_NUM_HIGHEST_BIT) == 0) ? 0 : 1;
	
	if( TABLE_SIZE == 2 )
		return ((self.table[1] & BIG_NUM_HIGHEST_BIT) == 0) ? 0 : 1;

	return 0;
}

/**
 * big_uint_t constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t 
 */
big_num_carry_t big_int_init_big_uint(inout big_int_t self, big_uint_t value)
{
	self = value;
	return (value.table[TABLE_SIZE-1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0; // check if highest bit set, if so there is a carry
}

/**
 * int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_int_init_int(inout big_int_t self, big_num_sstrg_t value)
{
	big_num_strg_t fill = ( value<0 ) ? BIG_NUM_MAX_VALUE : 0;

	for(size_t i=1 ; i<TABLE_SIZE ; ++i)
		self.table[i] = fill;

	self.table[0] = big_num_strg_t(value);
}

/**
 * long int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_init_lint(inout big_int_t self, big_num_lsstrg_t value)
{
	big_num_strg_t mask = (value < 0) ? BIG_NUM_MAX_VALUE : 0;

	self.table[0] = big_num_strg_t(big_num_lstrg_t(value));

	if( TABLE_SIZE == 1 ) {
		if( (big_num_strg_t(big_num_lstrg_t(value)) >> 32) != mask )
			return 1;

		return ((self.table[0] & BIG_NUM_HIGHEST_BIT) == (mask & BIG_NUM_HIGHEST_BIT)) ? 0 : 1;
	}

	self.table[1] = big_num_strg_t(big_num_lstrg_t(value) >> 32);

	for(size_t i=2 ; i < TABLE_SIZE ; ++i)
		self.table[i] = mask;

	return 0;
}

/**
 * big_int_t constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_int_init_big_int(inout big_int_t self, big_int_t value)
{
	self = value;
}

/**
 * uint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_uint(big_int_t self, out big_num_strg_t result)
{
	big_num_carry_t c = big_uint_to_uint(self, result);
	if (TABLE_SIZE == 1)
		return (result & BIG_NUM_HIGHEST_BIT) == 0 ? 0 : 1;
	return c;
}

/**
 * int converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_int(big_int_t self, out big_num_sstrg_t result)
{
	big_num_strg_t _result;
	big_num_carry_t c = big_int_to_uint(self, _result);
	result = big_num_sstrg_t(_result);
	return c;
}

/**
 * luint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_int_to_luint(big_int_t self, out big_num_lstrg_t result)
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
big_num_carry_t	big_int_to_lint(big_int_t self, out big_num_lsstrg_t result)
{
	if( TABLE_SIZE == 1 ) {
		result = big_num_lsstrg_t(big_num_sstrg_t(self.table[0]));
	} else {
		big_num_strg_t low  = self.table[0];
		big_num_strg_t high = self.table[1];

		result = big_num_lsstrg_t(low);
		result |= (big_num_lsstrg_t(high) << BIG_NUM_BITS_PER_UNIT);

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

	big_num_sstrg_t a1 = big_num_sstrg_t(self.table[i]);
	big_num_sstrg_t a2 = big_num_sstrg_t(l.table[i]);

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

	big_num_sstrg_t a1 = big_num_sstrg_t(self.table[i]);
	big_num_sstrg_t a2 = big_num_sstrg_t(l.table[i]);

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

	big_num_sstrg_t a1 = big_num_sstrg_t(self.table[i]);
	big_num_sstrg_t a2 = big_num_sstrg_t(l.table[i]);

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

	big_num_sstrg_t a1 = big_num_sstrg_t(self.table[i]);
	big_num_sstrg_t a2 = big_num_sstrg_t(l.table[i]);

	if (a1 != a2)
		return a1 > a2;

	for (--i ; i >= 0 ; --i) {
		if (self.table[i] != l.table[i])
			return self.table[i] > l.table[i];
	}

	return true;
}


/**
 * 
 * @param[in, out] self the big num object
 * @param[in] p1_is_sign 
 * @param[in] p2_is_sign 
 * @return big_num_carry_t 
 */
big_num_carry_t _big_int_correct_carry_after_adding(inout big_int_t self, bool p1_is_sign, bool p2_is_sign)
{
	if( !p1_is_sign && !p2_is_sign ) {
		if( big_uint_is_the_highest_bit_set(self) )
			return 1;
	}

	if( p1_is_sign && p2_is_sign ) {	
		if( ! big_uint_is_the_highest_bit_set(self) )
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
big_num_carry_t _big_int_add_int(inout big_int_t self, big_num_strg_t value, size_t index)
{
	bool p1_is_sign = big_int_is_sign(self);

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
big_num_carry_t _big_int_add_two_ints(inout big_int_t self, big_num_strg_t x2, big_num_strg_t x1, size_t index)
{
	bool p1_is_sign = big_int_is_sign(self);

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
big_num_carry_t _big_int_correct_carry_after_subtracting(inout big_int_t self, bool p1_is_sign, bool p2_is_sign)
{
	if( !p1_is_sign && p2_is_sign ) {
		if( big_uint_is_the_highest_bit_set(self) )
			return 1;
	}

	if( p1_is_sign && !p2_is_sign ) {	
		if( ! big_uint_is_the_highest_bit_set(self) )
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
big_num_carry_t _big_int_sub_int(inout big_int_t self, big_num_strg_t value, size_t index)
{
	bool p1_is_sign = big_int_is_sign(self);

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
big_num_carry_t _big_int_check_min_carry(inout big_int_t self, bool ss1_is_sign, bool ss2_is_sign)
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
	if( big_int_is_sign(self) )
	{
		if( ss1_is_sign != ss2_is_sign )
		{
			/*
				there can be one case where signs are different and
				the result will be equal the value from SetMin() (only the highest bit is set)
				(this situation is ok)
			*/
			if( !big_uint_is_only_the_highest_bit_set(self) )
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
big_num_ret_t _big_int_pow2(inout big_int_t self, const big_int_t pow)
{
	bool was_sign = big_int_is_sign(self);
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