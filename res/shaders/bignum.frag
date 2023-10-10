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

#define BIG_NUM_MAX_VALUE 			(uint((1UL<<( 32UL       ))-1UL))
#define BIG_NUM_HIGHEST_BIT			(uint((1UL<<( 32UL - 1UL ))    ))
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

	for(i=index+1 ; i < TABLE_SIZE && (c != 0) ; ++i)
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

	for(i=index+1 ; i < BIG_TABLE_SIZE && (c != 0) ; ++i)
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

	for(i=index+2 ; i<TABLE_SIZE && (c != 0); ++i)
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

	for(i=index+2 ; i<BIG_TABLE_SIZE && (c != 0); ++i)
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

	for(i=index+1 ; i<TABLE_SIZE && (c != 0); ++i)
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

			for(i=x2+x1+2 ; i<BIG_TABLE_SIZE && (c != 0) ; ++i)
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



//===============
// BigFloat.h
//===============

#define big_float_info_t uint
struct big_float_t {
	big_int_t exponent;
	big_uint_t mantissa;
	big_float_info_t info;
};

void 				big_float_set_max(inout big_float_t self);
void 				big_float_set_min(inout big_float_t self);
void 				big_float_set_zero(inout big_float_t self);
void 				big_float_set_one(inout big_float_t self);
void 				big_float_set_05(inout big_float_t self);
void				big_float_set_e(inout big_float_t self);
void				big_float_set_ln2(inout big_float_t self);
void 				big_float_set_nan(inout big_float_t self);
void 				big_float_set_zero_nan(inout big_float_t self);
void 				big_float_swap(inout big_float_t self, inout big_float_t ss2);

bool 				big_float_is_zero(big_float_t self);
bool 				big_float_is_sign(big_float_t self);
bool 				big_float_is_nan(big_float_t self);

void 				big_float_abs(inout big_float_t self);
big_num_carry_t		big_float_round(inout big_float_t self);
void 				big_float_sgn(inout big_float_t self);
void 				big_float_set_sign(inout big_float_t self);

big_num_carry_t 	big_float_add(inout big_float_t self, big_float_t ss2, bool round);
big_num_carry_t 	big_float_sub(inout big_float_t self, big_float_t ss2, bool round);
big_num_carry_t 	big_float_mul_uint(inout big_float_t self, big_num_strg_t ss2);
big_num_carry_t 	big_float_mul_int(inout big_float_t self, big_num_sstrg_t ss2);
big_num_carry_t 	big_float_mul(inout big_float_t self, big_float_t ss2, bool round);

big_num_ret_t 		big_float_div(inout big_float_t self, big_float_t ss2, bool round);
big_num_ret_t 		big_float_mod(inout big_float_t self, big_float_t ss2);
big_num_strg_t 		big_float_mod2(big_float_t self);

big_num_ret_t 		big_float_pow_big_uint(inout big_float_t self, big_uint_t pow);
big_num_ret_t 		big_float_pow_big_int(inout big_float_t self, big_int_t pow);
big_num_ret_t 		big_float_pow_big_frac(inout big_float_t self, big_float_t pow);
big_num_ret_t		big_float_pow(inout big_float_t self, big_float_t pow);
big_num_ret_t 		big_float_sqrt(inout big_float_t self);
big_num_carry_t		big_float_exp(inout big_float_t self, big_float_t x);
big_num_ret_t 		big_float_ln(inout big_float_t self, big_float_t x);
big_num_ret_t 		big_float_log(inout big_float_t self, big_float_t x, big_float_t base);

void 				big_float_init_float(inout big_float_t self, float value);
void				big_float_init_double(inout big_float_t self, double value);
void 				big_float_init_uint(inout big_float_t self, big_num_strg_t value);
void 				big_float_init_int(inout big_float_t self, big_num_sstrg_t value);
void				big_float_init_big_float(inout big_float_t self, big_float_t value);
void				big_float_init_big_uint(inout big_float_t self, big_uint_t value);
void				big_float_init_big_int(inout big_float_t self, big_int_t value);
big_num_carry_t		big_float_to_double(big_float_t self, out double result);
big_num_carry_t		big_float_to_float(big_float_t self, out float result);
big_num_carry_t		big_float_to_uint(big_float_t self, out big_num_strg_t result);
big_num_carry_t		big_float_to_int(big_float_t self, out big_num_sstrg_t result);

bool 				big_float_cmp_smaller(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger(big_float_t self, big_float_t l);
bool 				big_float_cmp_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_smaller_equal(big_float_t self, big_float_t l);
bool 				big_float_cmp_bigger_equal(big_float_t self, big_float_t l);

//===============
// BBigFloat.c
//===============

#define BIG_FLOAT_INFO_SIGN 	(1 << 7)
#define BIG_FLOAT_INFO_NAN		(1 << 6)
#define BIG_FLOAT_INFO_ZERO		(1 << 5)

big_num_carry_t 	_big_float_check_carry(inout big_float_t self, big_num_carry_t c);
big_num_carry_t 	_big_float_standardizing(inout big_float_t self);
bool				_big_float_correct_zero(inout big_float_t self);
void				_big_float_clear_info_bit(inout big_float_t self, big_float_info_t bit);
void				_big_float_set_info_bit(inout big_float_t self, big_float_info_t bit);
bool				_big_float_is_info_bit(big_float_t self, big_float_info_t bit);

big_num_carry_t 	_big_float_round_half_to_even(inout big_float_t self, bool is_half, bool rounding_up);
void 				_big_float_add_check_exponents(inout big_float_t ss2, big_int_t exp_offset, out bool last_bit_set, out bool rest_zero, out bool do_adding, out bool do_rounding);
big_num_carry_t 	_big_float_add_mantissas(inout big_float_t self, big_float_t ss2, inout bool last_bit_set, inout bool rest_zero);
big_num_carry_t 	_big_float_add(inout big_float_t self, big_float_t ss2, bool round, bool adding);
bool 				_big_float_check_greater_or_equal_half(inout big_float_t self, big_num_strg_t tab[2*BIG_NUM_PREC], big_num_strg_t len);
big_num_ret_t 		_big_float_pow_big_float_uint(inout big_float_t self, big_float_t _pow);
big_num_ret_t 		_big_float_pow_big_float_int(inout big_float_t self, big_float_t _pow);
void 				_big_float_exp_surrounding_0(inout big_float_t self, big_float_t x, inout size_t steps);
void 				_big_float_ln_surrounding_1(inout big_float_t self, big_float_t x, inout size_t steps);
void 				_big_float_init_uint_or_int(inout big_float_t self, big_uint_t value, big_num_sstrg_t compensation);
big_num_carry_t 	_big_float_to_uint_or_int(big_float_t self, out big_num_strg_t result);
void 				_big_float_init_double_set_exp_and_man(inout big_float_t self, bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2);
double 				_big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan);
bool 				_big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2);
bool				_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2);
bool				_big_float_equal_without_sign(big_float_t self, big_float_t ss2);
void 				_big_float_skip_fraction(inout big_float_t self);
bool 				_big_float_is_integer(big_float_t self);



/**
 * this method sets zero
 * @param[in, out] self the big num object
 */
void big_float_set_zero(inout big_float_t self)
{
	self.info = BIG_FLOAT_INFO_ZERO;
	big_int_set_zero(self.exponent);
	big_uint_set_zero(self.mantissa);
}

/**
 * this method sets one
 * @param[in, out] self the big num object
 */
void big_float_set_one(inout big_float_t self)
{
	self.info = 0;
	big_uint_set_zero(self.mantissa);
	self.mantissa.table[BIG_NUM_PREC-1] = BIG_NUM_HIGHEST_BIT;
	big_int_init_int(self.exponent, -big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - 1));
}

/**
 * this method sets 0.5
 * @param[in, out] self the big num object
 */
void big_float_set_05(inout big_float_t self)
{
	big_float_set_one(self);
	big_int_sub_int(self.exponent, 1);
}

/**
 * this method sets the value of e
 * (the base of the natural logarithm)
 * @param[in] self the big num object
 */
void big_float_set_e(inout big_float_t self)
{
	const big_num_strg_t temp_table[] = {
		0xadf85458, 0xa2bb4a9a, /*0xafdc5620, 0x273d3cf1, 0xd8b9c583, 0xce2d3695, 0xa9e13641, 0x146433fb, 
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
		*/
	// 0x2fe26dd4, ...
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};

	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using ExpSurrounding0(1) method
	// which took 1420 iterations
	// (the result was compared with e taken from http://antwrp.gsfc.nasa.gov/htmltest/gifcity/e.2mil)

	big_uint_set_from_table(self.mantissa, temp_table, BIG_NUM_PREC);
	big_int_init_int(self.exponent, -big_num_sstrg_t(BIG_NUM_PREC)*big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT) + 2);
	self.info = 0;
}

/**
 * this method sets the value of ln(2)
 * the natural logarithm from 2
 * @param[in] self the big num object
 */
void big_float_set_ln2(inout big_float_t self)
{
	const big_num_strg_t temp_table[] = {
		0xb17217f7, 0xd1cf79ab, /*0xc9e3b398, 0x03f2f6af, 0x40f34326, 0x7298b62d, 0x8a0d175b, 0x8baafa2b, 
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
		*/
	//0xcbb9ac40, ...
	// (the last word 0x1366a992 was rounded up because the next one is 0xcbb9ac40 -- first bit is one 0xc..)
	// 256 32bit words for the mantissa -- about 2464 valid decimal digits
	};	

	// above value was calculated using Big<1,400> type on a 32bit platform
	// and then the first 256 words were taken,
	// the calculating was made by using LnSurrounding1(2) method
	// which took 4035 iterations
	// (the result was compared with ln(2) taken from http://ja0hxv.calico.jp/pai/estart.html)

	big_uint_set_from_table(self.mantissa, temp_table, BIG_NUM_PREC);
	big_int_init_int(self.exponent, -big_num_sstrg_t(BIG_NUM_PREC)*big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT));
	self.info = 0;
}

/**
 * this method sets NaN flag
 * @param[in, out] self the big num object
 */
void big_float_set_nan(inout big_float_t self)
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method sets nan flag, also clears mantissa and exponent
 * @param[in, out] self the big num object
 */
void big_float_set_zero_nan(inout big_float_t self)
{
	big_float_set_zero(self);
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_NAN);
}

/**
 * this method swappes this for an argument
 * @param[in, out] self the big num object
 * @param[in, out] ss2 the big num to swap with
 */
void big_float_swap(inout big_float_t self, inout big_float_t ss2)
{
	big_float_info_t info_temp = self.info;
	self.info = ss2.info;
	ss2.info = info_temp;

	big_int_swap(self.exponent, ss2.exponent);
	big_uint_swap(self.mantissa, ss2.mantissa);
}

/**
 * this method sets the maximum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_max(inout big_float_t self)
{
	self.info = 0;
	big_int_set_max(self.mantissa);
	big_uint_set_max(self.exponent);
}

/**
 * this method sets the minimum value which can be held in this type
 * @param[in, out] self the big num object
 */
void big_float_set_min(inout big_float_t self)
{
	self.info = 0;
	big_int_set_max(self.mantissa);
	big_uint_set_max(self.exponent);
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
 * -  	-1 . 1
 * -  	2  . 2
 * @param[in, out] self the big num object 
 */
void big_float_abs(inout big_float_t self)
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
big_num_carry_t	big_float_round(inout big_float_t self)
{
	big_float_t _half;
	big_num_carry_t c;
	if (big_float_is_nan(self))
		return 1;
	if (big_float_is_zero(self))
		return 0;
	
	big_float_set_05(_half);

	if (big_float_is_sign(self)) {
		// self < 0
		c = big_float_sub(self, _half, true);
	} else {
		// self > 0
		c = big_float_add(self, _half, true);
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
void big_float_sgn(inout big_float_t self)
{
	if (big_float_is_nan(self) )
		return;
	if (big_float_is_sign(self)) {
		big_float_set_one(self);
		big_float_set_sign(self);
	} else if (big_float_is_zero(self)) {
		big_float_set_zero(self);
	} else {
		big_float_set_one(self);
	}
}

/**
 * this method sets the sign
 * 
 * samples
 * -  	-1 . -1
 * -  	2  . -2
 * 
 * we do not check whether there is a zero or not, if you're using this method
 * you must be sure that the value is (or will be afterwards) different from zero
 * @param[in, out] self the big num object 
 */
void big_float_set_sign(inout big_float_t self)
{
	_big_float_set_info_bit(self, BIG_FLOAT_INFO_SIGN);
}

/**
 * this method changes the sign
 * when there is a value of zero then the sign is not changed
 * 
 * samples
 * -  	-1 .  1
 * -  	 2 . -2
 * @param[in, out] self the big num object 
 */
void big_float_change_sign(inout big_float_t self)
{
	// we don't have to check the NaN flag here
	if( big_float_is_zero(self) )
		return;

	if( big_float_is_sign(self) )
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
big_num_carry_t big_float_add(inout big_float_t self, big_float_t ss2, bool round)
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
big_num_carry_t big_float_sub(inout big_float_t self, big_float_t ss2, bool round)
{
	return _big_float_add(self, ss2, round, false);
}

/**
 * Multiplication this = this * ss2 (ss2 is uint)
 * @param[in, out] self the big num object 
 * @param[in] ss2 ss2 without a sign
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul_uint(inout big_float_t self, big_num_strg_t ss2)
{
	big_big_uint_t man_result;
	size_t i;
	big_num_carry_t c = 0;

	if (big_float_is_nan(self))
		return 1;
	if (big_float_is_zero(self))
		return 0;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		man_result.table[i] = self.mantissa.table[i];
	for (; i < 2*BIG_NUM_PREC; ++i)
		man_result.table[i] = 0;
	big_num_carry_t man_c = big_uint_mul_int_big(man_result, ss2);

	big_num_sstrg_t bit = _big_uint_find_leading_bit_in_word(man_result.table[BIG_NUM_PREC]);

	if( bit!=-1 && big_num_strg_t(bit) > (BIG_NUM_BITS_PER_UNIT/2) ) {
		// 'i' will be from 0 to BIG_NUM_BITS_PER_UNIT
		i = big_uint_compensation_to_left_big(man_result);
		c = big_int_add_int(self.exponent, BIG_NUM_BITS_PER_UNIT - i);

		for(i=0 ; i < BIG_NUM_PREC ; ++i)
			self.mantissa.table[i] = man_result.table[i+1];
	} else {
		if( bit != -1 ) {
			big_uint_rcr_big(man_result, bit+1, 0);
			c += big_int_add_int(self.exponent, bit+1);
		}

		for(i=0 ; i < BIG_NUM_PREC ; ++i)
			self.mantissa.table[i] = man_result.table[i];
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
big_num_carry_t big_float_mul_int(inout big_float_t self, big_num_sstrg_t ss2)
{
	if (big_float_is_nan(self))
		return 1;
	if (ss2 == 0) {
		big_float_set_zero(self);
		return 0;
	}
	if (big_float_is_zero(self))
		return 0;
	if (big_float_is_sign(self) == (ss2<0)) {
		// the signs are the same (both are either - or +), the result is positive
		big_float_abs(self);
	} else {
		// the signs are different, the result is negative
		big_float_set_sign(self);
	}

	if (ss2 < 0)
		ss2 = -ss2;

	return big_float_mul_uint(self, big_num_strg_t(ss2));
}

/**
 * multiplication this = this * ss2
 * @param[in, out] self the big num object 
 * @param[in] ss2 the big num to multiply self by
 * @param[in] round true if result should be rounded
 * @return big_num_carry_t 
 */
big_num_carry_t big_float_mul(inout big_float_t self, big_float_t ss2, bool round)
{
	big_big_uint_t man_result;
	big_num_carry_t c = 0;
	size_t i;

	if( big_float_is_nan(self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(self) )
		return 0;

	if( big_float_is_zero(ss2) ) {
		big_float_set_zero(self);
		return 0;
	}

	// man_result = mantissa * ss2.mantissa
	big_uint_mul_no_carry(self.mantissa, ss2.mantissa, man_result, BIG_NUM_MUL_DEF);

	// 'i' will be from 0 to man*BIG_NUM_BITS_PER_UNIT
	// because mantissa and ss2.mantissa are standardized 
	// (the highest bit in man_result is set to 1 or
	// if there is a zero value in man_result the method CompensationToLeft()
	// returns 0 but we'll correct this at the end in Standardizing() method)
	i = big_uint_compensation_to_left_big(man_result);
	big_num_sstrg_t exp_add = big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - i);

	if( exp_add != 0 )
		c += big_int_add_int(self.exponent, exp_add );

	c += big_int_add(self.exponent, ss2.exponent);

	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		self.mantissa.table[i] = man_result.table[i+BIG_NUM_PREC];

	if( round && ((man_result.table[BIG_NUM_PREC-1] & BIG_NUM_HIGHEST_BIT) != 0) ) {
		bool is_half = _big_float_check_greater_or_equal_half(self, man_result.table, BIG_NUM_PREC);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(self) == big_float_is_sign(ss2) ) {
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
big_num_ret_t big_float_div(inout big_float_t self, big_float_t ss2, bool round)
{
	big_big_uint_t man1;
	big_big_uint_t man2;
	size_t i;
	big_num_carry_t c = 0;
		
	if( big_float_is_nan(self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( big_float_is_zero(self) )
		return 0;

	for(i=0 ; i<BIG_NUM_PREC ; ++i) {
		man1.table[i] 				= 0;
		man1.table[i+BIG_NUM_PREC] 	= self.mantissa.table[i];
		man2.table[i]     			= ss2.mantissa.table[i];
		man2.table[i+BIG_NUM_PREC] 	= 0;
	}

	big_big_uint_t remainder;
	big_uint_div_big(man1, man2, remainder, BIG_NUM_DIV_DEF);

	i = big_uint_compensation_to_left_big(man1);

	if( i != 0)
		c += big_int_sub_int(self.exponent, i);

	c += big_int_sub(self.exponent, ss2.exponent);
	
	for(i=0 ; i<BIG_NUM_PREC ; ++i)
		self.mantissa.table[i] = man1.table[i+BIG_NUM_PREC];

	if( round && ((man1.table[BIG_NUM_PREC-1] & BIG_NUM_HIGHEST_BIT) != 0) )
	{
		bool is_half = _big_float_check_greater_or_equal_half(self, man1.table, BIG_NUM_PREC);
		c += _big_float_round_half_to_even(self, is_half, true);
	}

	if( big_float_is_sign(self) == big_float_is_sign(ss2) )
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
big_num_ret_t big_float_mod(inout big_float_t self, big_float_t ss2)
{
	big_num_carry_t c = 0;

	if( big_float_is_nan(self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(ss2) ) {
		big_float_set_nan(self);
		return 2;
	}

	if( !_big_float_smaller_without_sign_than(self, ss2) )
	{
		big_float_t temp = self;

		c = big_float_div(temp, ss2, true);
		_big_float_skip_fraction(temp);
		c += big_float_mul(temp, ss2, true);
		c += big_float_sub(self, temp, true);

		if( !_big_float_smaller_without_sign_than(self, ss2 ) )
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
	big_int_set_zero(zero);
	big_int_init_int(negative_bits, -big_num_sstrg_t(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT));

	if( big_int_cmp_bigger(self.exponent, zero) || big_int_cmp_smaller_equal(self.exponent, negative_bits) )
		return 0;

	big_num_sstrg_t exp_int;
	big_int_to_int(self.exponent, exp_int);
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
big_num_ret_t big_float_pow_big_uint(inout big_float_t self, big_uint_t _pow)
{
	if( big_float_is_nan(self) )
		return 1;

	if( big_float_is_zero(self) ) {
		if( big_uint_is_zero(_pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	big_float_t start = self;
	big_float_t result;
	big_float_set_one(result);
	big_num_carry_t c = 0;

	while( c == 0 ) {
		if( (_pow.table[0] & 1) != 0 )
			c += big_float_mul(result, start, true);

		big_uint_rcr(_pow, 1, 0);

		if( big_uint_is_zero(_pow) )
			break;

		c += big_float_mul(start, start, true);
	}

	self = result;

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
big_num_ret_t big_float_pow_big_int(inout big_float_t self, big_int_t _pow)
{
	if( big_float_is_nan(self) )
		return 1;

	if ( !big_int_is_sign(_pow) )
		return big_float_pow_big_uint(self, _pow);
	
	if ( big_float_is_zero(self) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_num_carry_t c = big_int_change_sign(_pow);
	big_float_t t = self;
	c += big_float_pow_big_uint(t, _pow); // here can only be a carry (return:1)

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
big_num_ret_t big_float_pow_big_frac(inout big_float_t self, big_float_t _pow)
{
	if( big_float_is_nan(self) || big_float_is_nan(_pow) )
		return _big_float_check_carry(self, 1);

	big_float_t temp;
	big_float_set_one(temp);
	big_num_carry_t c = big_float_ln(temp, self);

	if( c != 0 ) { // can be 2 from big_float_ln()
		big_float_set_nan(self);
		return c;
	}

	c += big_float_mul(temp, _pow, true);
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
big_num_ret_t big_float_pow(inout big_float_t self, big_float_t _pow)
{
	if (big_float_is_nan(self) || big_float_is_nan(_pow))
		return _big_float_check_carry(self, 1);

	if (big_float_is_zero(self)) {
		// 0^pow will be 0 only for pow>0
		if( big_float_is_sign(_pow) || big_float_is_zero(_pow) ) {
			big_float_set_nan(self);
			return 2;
		}

		big_float_set_zero(self);

		return 0;
	}

	big_int_t zero, negative_bits;
	big_int_set_zero(zero);
	big_int_init_int(negative_bits, -big_num_sstrg_t(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT));

	if ( big_int_cmp_bigger(_pow.exponent, negative_bits) && big_int_cmp_smaller_equal(_pow.exponent, zero)) {
		if ( _big_float_is_integer(_pow) )
			return _big_float_pow_big_float_int(self, _pow);
	}

	return big_float_pow_big_frac(self, _pow);
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
big_num_ret_t big_float_sqrt(inout big_float_t self)
{
	if( big_float_is_nan(self) || big_float_is_sign(self)) {
		big_float_set_nan(self);
		return 2;
	}

	if (big_float_is_zero(self))
		return 0;

	big_float_t old = self;
	big_float_t ln; // ln get's set in big_float_ln()
	big_num_carry_t c = 0;

	// we're using the formula: sqrt(x) = e ^ (ln(x) / 2)
	c += big_float_ln(ln, self);
	c += big_int_sub_int(ln.exponent, 1); // ln = ln / 2
	c += big_float_exp(self, ln);

	// above formula doesn't give accurate results for some integers
	// e.g. Sqrt(81) would not be 9 but a value very closed to 9
	// we're rounding the result, calculating result*result and comparing
	// with the old value, if they are equal then the result is an integer too


	if ( (c == 0) && _big_float_is_integer(old) && !_big_float_is_integer(self)) {
		big_float_t temp = self;
		c += big_float_round(temp);

		big_float_t temp2 = temp;
		c += big_float_mul(temp, temp2, true);

		if (big_float_cmp_equal(temp, old))
			self = temp2;
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
big_num_carry_t big_float_exp(inout big_float_t self, big_float_t x)
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
	big_int_init_int(m.exponent, -big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));

	// 'e_' will be the value of '2^exponent'
	//   e_.mantissa.table[man-1] = TTMATH_UINT_HIGHEST_BIT;  and
	//   e_.exponent.Add(1) mean:
	//     e_.mantissa.table[0] = 1;
	//     e_.Standardizing();
	//     e_.exponent.Add(man*TTMATH_BITS_PER_UINT)
	//     (we must add 'man*TTMATH_BITS_PER_UINT' because we've taken it from the mantissa)
	big_float_t e_ = x;
	big_int_set_zero(e_.mantissa);
	e_.mantissa.table[BIG_NUM_PREC-1] = BIG_NUM_HIGHEST_BIT;
	c += big_int_add_int(e_.exponent, 1);
	big_float_abs(e_);

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
	big_float_set_one(one);
	size_t steps;
	if (big_float_cmp_smaller_equal(e_, one)) {
		big_float_mul(m, e_, true);
		_big_float_exp_surrounding_0(self, m, steps);
	} else {
		_big_float_exp_surrounding_0(self, m, steps);
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
big_num_ret_t big_float_ln(inout big_float_t self, big_float_t x)
{
	if (big_float_is_nan(x))
		return _big_float_check_carry(self, 1);
	
	if ( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t exponent_temp, mantissa_compensation;
	big_float_init_big_int(exponent_temp, x.exponent);

	// m will be the value of the mantissa in range <1,2)
	big_float_t m = x;
	big_int_init_int(m.exponent, -big_num_sstrg_t(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT - 1));

	// we must add 'BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT-1' because we've taken it from the mantissa
	big_float_init_int(mantissa_compensation, BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT-1);
	big_num_carry_t c = big_float_add(exponent_temp, mantissa_compensation, true);

	size_t steps;
	_big_float_ln_surrounding_1(self, m, steps);

	big_float_t ln2;
	big_float_set_ln2(ln2);
	c += big_float_mul(exponent_temp, ln2, true);
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
big_num_ret_t big_float_log(inout big_float_t self, big_float_t x, big_float_t base)
{
	if( big_float_is_nan(x) || big_float_is_nan(base) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_sign(x) || big_float_is_zero(x) ) {
		big_float_set_nan(self);
		return 2;
	}

	big_float_t denominator;
	big_float_set_one(denominator);

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

	c += big_float_ln(denominator, base);
	c += big_float_div(self, denominator, true);

	return _big_float_check_carry(self, c);
}

/**
 * float constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_float(inout big_float_t self, float value)
{
	big_float_init_double(self, value);
}

/**
 * double constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_double(inout big_float_t self, double value)
{
	struct double_union_u {
		big_num_strg_t u[2]; // two 32bit words
	} temp;

	uvec2 double_vec = unpackDouble2x32(value);
	temp.u[0] = double_vec.x;
	temp.u[1] = double_vec.y;

	big_num_sstrg_t e = big_num_sstrg_t(( temp.u[1] & 0x7FF00000u) >> 20);
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
									ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
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
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
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
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
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
											new_c    		= (m[i] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
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
				big_num_sstrg_t(e - 1022 - BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT + 1 - moved), 
				0, m[1], m[0]);
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
void big_float_init_uint(inout big_float_t self, big_num_strg_t value)
{
	if (value == 0) {
		big_float_set_zero(self);
		return;
	}

	self.info = 0;

	for (size_t i = 0; i < BIG_NUM_PREC-1; ++i)
		self.mantissa.table[i] = 0;
	self.mantissa.table[BIG_NUM_PREC-1] = value;
	big_int_init_int(self.exponent, -big_num_sstrg_t(BIG_NUM_PREC-1) * big_num_sstrg_t(BIG_NUM_BITS_PER_UNIT));

	// there shouldn't be a carry because 'value' has the 'uint' type 
	_big_float_standardizing(self);
}

/**
 * int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_int(inout big_float_t self, big_num_sstrg_t value)
{
	bool is_sign = false;
	if (value < 0) {
		value = -value;
		is_sign = true;
	}

	big_float_init_uint(self, big_num_strg_t(value));

	if (is_sign)
		big_float_set_sign(self);
}

/**
 * big float constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_float(inout big_float_t self, big_float_t value)
{
	self = value;
}

/**
 * big uint constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_uint(inout big_float_t self, big_uint_t value)
{
	self.info = 0;
	big_num_sstrg_t compensation = big_num_sstrg_t(big_uint_compensation_to_left(value));
	_big_float_init_uint_or_int(self, value, compensation);
}

/**
 * big int constructor
 * @param[in, out] self the big num object
 * @param[in] value the value to set self to
 */
void big_float_init_big_int(inout big_float_t self, big_int_t value)
{
	self.info = 0;
	bool is_sign = false;
	if (big_int_is_sign(value)) {
		big_int_change_sign(value);
		is_sign = true;
	}

	big_num_sstrg_t compensation = big_num_sstrg_t(big_int_compensation_to_left(value));
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
big_num_carry_t	big_float_to_double(big_float_t self, out double result)
{
	if ( big_float_is_zero(self) ) {
		result = 0.0;
		return 0;
	}

	if ( big_float_is_nan(self) ) {
		result = _big_float_to_double_set_double(self, false, 2047, 0, false, true);
		return 0;
	}

	big_num_sstrg_t e_correction = big_num_sstrg_t(BIG_NUM_PREC*BIG_NUM_BITS_PER_UNIT) - 1;
	big_int_t _e_correction;
	big_int_init_int(_e_correction, 1024 - e_correction);
	if (big_int_cmp_bigger_equal(self.exponent, _e_correction) ) {
		// +/- infinity
		result = _big_float_to_double_set_double(self, big_float_is_sign(self), 2047, 0, true, false);
		return 1;
	} else {
		big_int_init_int(_e_correction, -1023 -52 - e_correction);
		if (big_int_cmp_smaller_equal(self.exponent, _e_correction)) {
			// too small value - we assume that there'll be a zero and return a carry
			result = 0.0;
			return 1;
		}
	}

	big_num_sstrg_t e;
	big_int_to_int(self.exponent, e);
	e += e_correction;

	if ( e <= -1023 ) {
		// -1023-52 < e <= -1023  (unnormalized value)
		result = _big_float_to_double_set_double(self, big_float_is_sign(self), 0, -(e + 1023), false, false);
	} else {
		// -1023 < e < 1024
		result = _big_float_to_double_set_double(self, big_float_is_sign(self), (e + 1023), -1, false, false);
	}
}

/**
 * float converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_float(big_float_t self, out float result)
{
	double result_double;
	big_num_carry_t c = big_float_to_double(self, result_double);
	result = float(result_double);

	if (result == -0.0f)
		result = 0.0f;
	
	if (c != 0)
		return 1;

	// although the result_double can have a correct value
	// but after converting to float there can be infinity
	
	// check for infinity
	if ( isinf(result) )
		return 1;
	
	if ( result == 0.0f && result_double != 0.0)
		return 1;
	
	return 0;
}

/**
 * uint converter
 * @param[in, out] self the big num object
 * @param[in] result the converted value
 * @return big_num_carry_t 
 */
big_num_carry_t	big_float_to_uint(big_float_t self, out big_num_strg_t result)
{
	if ( _big_float_to_uint_or_int(self, result) != 0 )
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
big_num_carry_t	big_float_to_int(big_float_t self, out big_num_sstrg_t result)
{
	big_num_strg_t result_uint;
	big_num_carry_t c = _big_float_to_uint_or_int(self, result_uint);
	result = big_num_sstrg_t(result_uint);

	if ( c != 0)
		return 1;
	
	big_num_strg_t mask = 0;
	if (big_float_is_sign(self)) {
		mask = BIG_NUM_MAX_VALUE;
		result = -1 * (result);
	}

	return ((result & BIG_NUM_HIGHEST_BIT) == (mask & BIG_NUM_HIGHEST_BIT)) ? 0 : 1;
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



/**
 * this method sets NaN if there was a carry (and returns 1 in such a case)
 * @param[in, out] self the big num object
 * @param[in] c can be 0, 1 or other value different from zero
 * @return big_num_carry_t 
 */
big_num_carry_t _big_float_check_carry(inout big_float_t self, big_num_carry_t c)
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
big_num_carry_t _big_float_standardizing(inout big_float_t self)
{
	if ( big_uint_is_the_highest_bit_set(self.mantissa) ) {
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_ZERO);
		return 0;
	}

	if ( _big_float_correct_zero(self) )
		return 0;
	
	big_int_t comp;
	big_int_init_int(comp, big_num_sstrg_t(big_uint_compensation_to_left(self.mantissa)));
	return big_int_sub(self.exponent, comp);
}

/**
 * if the mantissa is equal zero this method sets exponent to zero and
 * info without the sign
 * @param[in, out] self the big num object
 * @return true there was a correction
 * @return false there was no correction
 */
bool	_big_float_correct_zero(inout big_float_t self)
{
	if (big_uint_is_zero(self.mantissa)) {
		_big_float_set_info_bit(self, BIG_FLOAT_INFO_ZERO);
		_big_float_clear_info_bit(self, BIG_FLOAT_INFO_SIGN);
		big_int_set_zero(self.exponent);

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
void	_big_float_clear_info_bit(inout big_float_t self, big_float_info_t bit)
{
	self.info = self.info & (~bit);
}

/**
 * this method sets a specific bit in the 'info' variable
 * @param[in, out] self the big num object
 * @param[in] bit the bit to set
 */
void	_big_float_set_info_bit(inout big_float_t self, big_float_info_t bit)
{
	self.info = self.info | (bit);
}

/**
 * this method returns true if a specific bit in the 'info' variable is set
 * @param[in, out] self the big num object
 * @param[in] bit the bit to check
 * @return true bit was set
 * @return false bit was not set
 */
bool	_big_float_is_info_bit(big_float_t self, big_float_info_t bit)
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
big_num_carry_t _big_float_round_half_to_even(inout big_float_t self, bool is_half, bool rounding_up)
{
	big_num_carry_t c = 0;

	if( !is_half || big_uint_is_the_lowest_bit_set(self.mantissa) )
	{
		if( rounding_up )
		{
			if( big_uint_add_uint(self.mantissa, 1) != 0)
			{
				big_uint_rcr(self.mantissa, 1, 1);
				c = big_int_add_int(self.exponent, 1);
			}
		} else {
			big_uint_sub_uint(self.mantissa, 1);

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
void _big_float_add_check_exponents(inout big_float_t ss2, big_int_t exp_offset, out bool last_bit_set, out bool rest_zero, out bool do_adding, out bool do_rounding)
{
	big_int_t mantissa_size_in_bits;
	big_int_init_uint(mantissa_size_in_bits, BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT);

	if (big_int_cmp_equal(exp_offset, mantissa_size_in_bits)) {
		last_bit_set = big_uint_is_the_highest_bit_set(ss2.mantissa);
		rest_zero = big_uint_are_first_bits_zero(ss2.mantissa, BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT - 1);
		do_rounding = true;
	} else if (big_int_cmp_smaller(exp_offset, mantissa_size_in_bits)) {
		big_num_strg_t moved;
		big_int_to_int(exp_offset, moved); // how many times we must move ss2.mantissa
		rest_zero = true;

		if ( moved > 0 ) {
			last_bit_set = bool(big_uint_get_bit(ss2.mantissa, moved-1) );

			if (moved > 1)
				rest_zero = big_uint_are_first_bits_zero(ss2.mantissa, moved-1);
			
			// (2) moving 'exp_offset' times
			big_uint_rcr(ss2.mantissa, moved, 0);
		}

		do_adding = true;
		do_rounding = true;

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
big_num_carry_t _big_float_add_mantissas(inout big_float_t self, big_float_t ss2, inout bool last_bit_set, inout bool rest_zero)
{
	big_num_carry_t c = 0;
	if (big_float_is_sign(self) == big_float_is_sign(ss2)) {
		// values have the same signs
		if (big_uint_add(self.mantissa, ss2.mantissa, 0) != 0) {
			// we have one bit more from addition (carry)
			// now rest_zero means the old rest_zero with the old last_bit_set
			rest_zero    = (!last_bit_set && rest_zero);
			last_bit_set = big_uint_rcr(self.mantissa, 1,1) != 0;
			c += big_int_add_int(self.exponent, 1);
		}
	} else {
		// values have different signs
		// there shouldn't be a carry here because
		// (1) (2) guarantee that the mantissa of this
		// is greater than or equal to the mantissa of the ss2

		big_uint_sub(self.mantissa, ss2.mantissa, 0);
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
big_num_carry_t _big_float_add(inout big_float_t self, big_float_t ss2, bool round, bool adding)
{
	bool last_bit_set, rest_zero, do_adding, do_rounding, rounding_up;
	big_num_carry_t c = 0;

	if( big_float_is_nan(self) || big_float_is_nan(ss2) )
		return _big_float_check_carry(self, 1);

	if( !adding )
		big_float_change_sign(ss2); // subtracting

	// (1) abs(this) will be >= abs(ss2)
	if( _big_float_smaller_without_sign_than(self, ss2) )
		big_float_swap(self, ss2);

	if( big_float_is_zero(ss2) )
		return 0;

	big_int_t exp_offset = self.exponent;
	big_int_sub(exp_offset, ss2.exponent);

	if( big_int_abs(exp_offset) == 0 ) {
		// if there is a carry in Abs it means the value in exp_offset has only the lowest bit set
		// so the value is the smallest possible integer
		// and its Abs would be greater than mantissa size in bits
		// so the method AddCheckExponents would do nothing


		last_bit_set = rest_zero = do_adding = do_rounding = false;
		rounding_up = (big_float_is_sign(self) == big_float_is_sign(ss2));

		_big_float_add_check_exponents(ss2, exp_offset, last_bit_set, rest_zero, do_adding, do_rounding);
		
		if( do_adding )
			c += _big_float_add_mantissas(self, ss2, last_bit_set, rest_zero);

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
bool _big_float_check_greater_or_equal_half(inout big_float_t self, big_num_strg_t tab[2*BIG_TABLE_SIZE], big_num_strg_t len)
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
big_num_ret_t _big_float_pow_big_float_uint(inout big_float_t self, big_float_t _pow)
{
	if( big_float_is_nan(self) || big_float_is_nan(_pow) )
		return _big_float_check_carry(self, 1);

	if( big_float_is_zero(self) ) {
		if( big_float_is_zero(_pow) ) {
			// we don't define zero^zero
			big_float_set_nan(self);
			return 2;
		}

		// 0^(+something) is zero
		return 0;
	}

	if (big_float_is_sign(_pow))
		big_float_abs(_pow);

	big_float_t start = self;
	big_float_t result;
	big_float_t one;
	big_num_carry_t c = 0;
	big_float_set_one(one);
	result = one;

	while( c == 0 ) {
		if( big_float_mod2(_pow) != 0)
			c += big_float_mul(result, start, true);

		c += big_int_sub_int(_pow.exponent, 1);

		if( big_float_cmp_smaller(_pow, one) )
			break;

		c += big_float_mul(start, start, true);
	}

	self = result;

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
big_num_ret_t _big_float_pow_big_float_int(inout big_float_t self, big_float_t _pow)
{
	if( big_float_is_nan(self) )
		return 1;

	if ( !big_float_is_sign(_pow) )
		return _big_float_pow_big_float_uint(self, _pow);
	
	if ( big_float_is_zero(self) ) {
		// if 'p' is negative then
		// 'this' must be different from zero
		big_float_set_nan(self);
		return 2;
	}

	big_float_t temp = self;
	big_num_carry_t c = _big_float_pow_big_float_uint(temp, _pow); // here can only be a carry (return:1)

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
void _big_float_exp_surrounding_0(inout big_float_t self, big_float_t x, inout size_t steps)
{
	big_float_t denominator, denominator_i;
	big_float_t one, old_value, next_part;
	big_float_t numerator = x;

	big_float_set_one(one);
	self = one;
	denominator = one;
	denominator_i = one;

	size_t i;
	old_value = self;
	for (i = 1 ; i <= BIG_NUM_ARITHMETIC_MAX_LOOP ; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'
		next_part = numerator;
		if (big_float_div(next_part, denominator, true) != 0)
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if ( big_float_cmp_equal(old_value, self) )
				// we've added next few parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = self;
		}

		// we set the denominator and the numerator for a next part of the formula
		if (big_float_add(denominator_i, one, true) != 0)
			// if there is a carry here the result we return as good
			break;

		if (big_float_mul(denominator, denominator_i, true) != 0)
			break;
		
		if (big_float_mul(numerator, x, true) != 0)
			break;
	}

	steps = i;
}

/**
 * Natural logarithm this = ln(x) where x in range <1,2)
 * we're using the formula:
 * ln(x) = 2 * [ (x-1)/(x+1) + (1/3)((x-1)/(x+1))^3 + (1/5)((x-1)/(x+1))^5 + ... ]
 * @param[in, out] self the big num object
 * @param[in] x 
 * @param[out] steps 
 */
void _big_float_ln_surrounding_1(inout big_float_t self, big_float_t x, inout size_t steps)
{
	big_float_t old_value, next_part, denominator, one, two, x1, x2;
	x1 = x;
	x2 = x;
	big_float_set_one(one);

	if ( big_float_cmp_equal(x, one) ) {
		// LnSurrounding1(1) is 0
		big_float_set_zero(self);
		return;
	}

	big_float_init_uint(two, 2);

	big_float_sub(x1, one, true);
	big_float_add(x2, one, true);

	big_float_div(x1, x2, true);
	x2 = x1;
	big_float_mul(x2, x1, true);

	denominator = one;
	big_float_set_zero(self);

	old_value = self;
	size_t i;

	for (i = 1; i <= BIG_NUM_ARITHMETIC_MAX_LOOP; ++i) {
		bool testing = ((i & 3) == 0); // it means '(i % 4) == 0'

		next_part = x1;

		if (big_float_div(next_part, denominator, true) != 0)
			// if there is a carry here we only break the loop 
			// however the result we return as good
			// it means there are too many parts of the formula
			break;
		
		// there shouldn't be a carry here
		big_float_add(self, next_part, true);

		if (testing) {
			if (big_float_cmp_equal(old_value, self))
				// we've added next (step_test) parts of the formula but the result
				// is still the same then we break the loop
				break;
			else
				old_value = self;
		}

		if (big_float_mul(x1, x2, true) != 0)
			// if there is a carry here the result we return as good
			break;
		
		if (big_float_add(denominator, one, true) != 0)
			break;
	}

	// this = this * 2
	// ( there can't be a carry here because we calculate the logarithm between <1,2) )
	big_int_add_int(self.exponent, 1);

	steps = i;
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
void _big_float_init_uint_or_int(inout big_float_t self, big_uint_t value, big_num_sstrg_t compensation)
{
	big_int_init_int(self.exponent, - compensation);

	// copying the highest words
	size_t i;
	for(i=1 ; i<=BIG_NUM_PREC ; ++i)
		self.mantissa.table[BIG_NUM_PREC-i] = value.table[BIG_NUM_PREC-i];

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
big_num_carry_t _big_float_to_uint_or_int(big_float_t self, out big_num_strg_t result)
{
	result = 0;

	if ( big_float_is_zero(self) )
		return 0;

	big_num_sstrg_t max_bit = -big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT);
	big_int_t _max_bit;
	big_int_init_int(_max_bit, max_bit + BIG_NUM_BITS_PER_UNIT);

	if ( big_int_cmp_bigger(self.exponent, _max_bit) )
		// if exponent > (maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT)) the value can't be passed
		// into the 'big_num_sstrg_t' type (it's too big)
		return 1;
	
	big_int_init_int(_max_bit, max_bit);

	if ( big_int_cmp_smaller_equal(self.exponent, _max_bit) )
		// our value is from the range of (-1,1) and we return zero
		return 0;

	// exponent is from a range of (maxbit, maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT) >
	// and [maxbit + (big_num_sstrg_t)(BIG_NUM_BITS_PER_UNIT] <= 0
	big_num_sstrg_t how_many_bits;
	big_int_to_int(self.exponent, how_many_bits);

	// how_many_bits is negative, we'll make it positive
	how_many_bits = -how_many_bits;

	result = (self.mantissa.table[BIG_NUM_PREC-1] >> (how_many_bits % BIG_NUM_BITS_PER_UNIT));

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
void _big_float_init_double_set_exp_and_man(inout big_float_t self, bool is_sign, big_num_sstrg_t e, big_num_strg_t mhighest, big_num_strg_t m1, big_num_strg_t m2)
{
	self.exponent;
	big_int_init_int(self.exponent, e);

	if( BIG_NUM_PREC > 1 ) {
		self.mantissa.table[BIG_NUM_PREC-1] = m1 | mhighest;
		self.mantissa.table[big_num_sstrg_t(BIG_NUM_PREC-2)] = m2;
		// although man>1 we're using casting into sint
		// to get rid from a warning which generates Microsoft Visual:
		// warning C4307: '*' : integral constant overflow

		for(size_t i=0 ; i<BIG_NUM_PREC-2 ; ++i)
			self.mantissa.table[i] = 0;
	} else {
		self.mantissa.table[0] = m1 | mhighest;
	}

	self.info = 0;

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
double _big_float_to_double_set_double(big_float_t self, bool is_sign, big_num_strg_t e, big_num_sstrg_t move, bool infinity, bool nan)
{
	struct {
		uvec2 u; // two 32bit words
	} temp;

	
	temp.u[0] = temp.u[1] = 0;

	if (is_sign)
		temp.u[1] |= 0x80000000u;

	temp.u[1] |= (e << 20) & 0x7FF00000u;

	if ( nan ) {
		temp.u[0] |= 1;
		return packDouble2x32(temp.u);
	}

	if ( infinity )
		return packDouble2x32(temp.u);
	
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
					ssize_t all_words 	= ssize_t(bits / BIG_NUM_BITS_PER_UNIT);
					big_num_strg_t mask = 0;


					if( all_words >= 2 ) {
						if( all_words == 2 && rest_bits == 0 )
							last_c = (m[1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;
						// else: last_c is default set to 0

						// clearing
						for(size_t i = 0 ; i<2 ; ++i)
							m[i] = mask;

						rest_bits = 0;
					} else if( all_words > 0 ) {
						// 0 < all_words < 2

						ssize_t first, second;
						last_c = (m[all_words - 1] & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0; // all_words is > 0

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

						for(i=1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) != 0 ? BIG_NUM_HIGHEST_BIT : 0;
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

						for(i=1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) != 0 ? BIG_NUM_HIGHEST_BIT : 0;
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

						for(i=1 ; i>=0 ; --i) {
							new_c    		= (m[i] & 1) != 0 ? BIG_NUM_HIGHEST_BIT : 0;
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

						c = (c & BIG_NUM_HIGHEST_BIT) != 0 ? 1 : 0;

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

	return packDouble2x32(temp.u);
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
bool _big_float_smaller_without_sign_than(big_float_t self, big_float_t ss2)
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
bool	_big_float_greater_without_sign_than(big_float_t self, big_float_t ss2)
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
bool	_big_float_equal_without_sign(big_float_t self, big_float_t ss2)
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
void _big_float_skip_fraction(inout big_float_t self) {
	if (big_float_is_nan(self) || big_float_is_zero(self) )
		return;
	
	if ( !big_int_is_sign(self.exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return;
	
	big_int_t negative_bits;
	big_int_init_int(negative_bits, -big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));
	if (big_int_cmp_smaller_equal(self.exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		big_float_set_zero(self);
		return;
	}

	// exponent is in range (-BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self.exponent, e);
	big_uint_clear_first_bits(self.mantissa, -e);

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
bool _big_float_is_integer(big_float_t self)
{
	if (big_float_is_zero(self))
		return true;
	
	if (!big_int_is_sign(self.exponent))
		// exponent >= 0 -- the value doesn't have any fractions
		return true;

	big_int_t negative_bits;
	big_int_init_int(negative_bits, -big_num_sstrg_t(BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT));

	if (big_int_cmp_smaller_equal(self.exponent, negative_bits)) {
		// value is from (-1, 1), return zero
		return false;
	}

	// exponent is in range (-BIG_NUM_PREC * BIG_NUM_BITS_PER_UNIT, 0)
	big_num_sstrg_t e;
	big_int_to_int(self.exponent, e);
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