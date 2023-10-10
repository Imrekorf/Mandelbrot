#version 460 core
#extension GL_ARB_gpu_shader_int64 : require
precision highp float;

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



#define PRECISION 	(3)
#define ARRAY_SIZE 	(PRECISION+1)

uniform float 	AP_BASE 		= 4294967296.0;
uniform uint 	AP_HALF_BASE 	= 2147483648u;

void ap_assign(inout uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE]);					// a = b
void ap_zero(inout uint a[ARRAY_SIZE]);												// a = 0
void ap_load(inout uint a[ARRAY_SIZE], in float load_value);									// a = v
void ap_shift(inout uint a[ARRAY_SIZE], in uint shift_n);									// a << n
void ap_negate(inout uint a[ARRAY_SIZE]);											// a = -a
void ap_add(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]);	// r = a + b
void ap_mul(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]);	// r = a * b

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform uint u_zoom[ARRAY_SIZE];
uniform uint u_offset_r[ARRAY_SIZE];
uniform uint u_offset_i[ARRAY_SIZE];

#define PI 				3.1415926538

#define MAX_ITTERATIONS (256)
#define COLOR_REPEAT	3
#define DEBUG_SQUARE

const float ln_max_ittr = log(MAX_ITTERATIONS+1);

vec4 	mandelbrot(in dvec2 c);
dvec2 	step_mandelbrot(in dvec2 z, in dvec2 c);
vec4 	integerToColor(in float i);

vec4 	mandelbrot_arbprec(in vec2 c, in uint offset_r[ARRAY_SIZE], in uint offset_i[ARRAY_SIZE], in uint zoom[ARRAY_SIZE]);
void 	step_mandelbrot_arb_prec(
			in uint z_r[ARRAY_SIZE], in uint z_i[ARRAY_SIZE], 
			in uint c_r[ARRAY_SIZE], in uint c_i[ARRAY_SIZE], 
			out uint nz_r[ARRAY_SIZE], out uint nz_i[ARRAY_SIZE]);

void main()
{
	vec2 translated = vec2((gl_FragCoord.x / u_resolution.x) - 0.5, (gl_FragCoord.y / u_resolution.y) - 0.5);

#ifdef DEBUG_SQUARE
	if (		all(greaterThan(translated.xy, vec2( 0.00,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.01)))){
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2( 0.00, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.00)))) {
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2(-0.01,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.01)))) {
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
		return;
	} else if (	all(greaterThan(translated.xy, vec2(-0.01, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.00)))) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		return;
	}
#endif

	FragColor = mandelbrot_arbprec(translated, u_offset_r, u_offset_i, u_zoom);
}

vec4 integerToColor(in float i)
{
	float angle = log(i+1.0) / log(256.0); // reduce to value between 0.0-1.0
	angle = angle * COLOR_REPEAT;
	angle = fract(angle);

	return vec4(
		0.5 * (1.0 + (cos((2.0*PI) * (angle            )))),
		0.5 * (1.0 - (cos((2.0*PI) * (angle - (1.0/3.0))))),
		0.5 * (1.0 + (cos((2.0*PI) * (angle + (1.0/3.0))))),
		1.0);
}

vec4 mandelbrot_arbprec(in vec2 c, in uint offset_r[ARRAY_SIZE], in uint offset_i[ARRAY_SIZE], in uint zoom[ARRAY_SIZE])
{
	uint c_r[ARRAY_SIZE];
    uint c_i[ARRAY_SIZE];
    uint z_r[ARRAY_SIZE];
    uint z_i[ARRAY_SIZE];
	
	uint nz_r[ARRAY_SIZE];
    uint nz_i[ARRAY_SIZE];

	ap_load(c_r, c.x);
	ap_load(c_i, c.y);
	
	ap_mul(c_r, zoom, c_r);
	ap_mul(c_i, zoom, c_i);
	
	ap_negate(offset_r);
	ap_negate(offset_i);
	ap_add(c_r, offset_r, c_r);
	ap_add(c_i, offset_i, c_i);

	ap_zero(z_r);
	ap_zero(z_i);

	int itterations = 0;
	for (; itterations < MAX_ITTERATIONS; itterations++) {
		uint a_sqr[ARRAY_SIZE];
        uint b_sqr[ARRAY_SIZE];
        ap_mul(z_r, z_r, a_sqr);
        ap_mul(z_i, z_i, b_sqr);
        ap_add(a_sqr, b_sqr, a_sqr); // pretend a_sqr here is r_sqr
        
        if (a_sqr[1] > 4) { // pretend a_sqr here is r_sqr
            return integerToColor(itterations);
        }
        
		step_mandelbrot_arb_prec(z_r, z_i, c_r, c_i, nz_r, nz_i);
		ap_assign(z_r, nz_r);
		ap_assign(z_i, nz_i);
	}

	return vec4(0.0, 0.0, 0.0, 1.0);
}

void step_mandelbrot_arb_prec(
	in uint z_r[ARRAY_SIZE], in uint z_i[ARRAY_SIZE], 
	in uint c_r[ARRAY_SIZE], in uint c_i[ARRAY_SIZE], 
	out uint nz_r[ARRAY_SIZE], out uint nz_i[ARRAY_SIZE])
{
	uint tmp1[ARRAY_SIZE];
	uint tmp2[ARRAY_SIZE];
	// calculate 'z.real
	ap_mul(z_r, z_r, tmp1); 			// z.real^2
	ap_mul(z_i, z_i, tmp2); 			// z.imag^2
	ap_negate(tmp2);					// z.imag^2 * -1
	ap_add(tmp1, tmp2, tmp1);			// z.real^2 - z.imag^2
	ap_add(tmp1, c_r, nz_r);			// z.real^2 - z.imag^2 + c.real

	// calculate 'z.imag
	ap_load(tmp2, 2.0);
	ap_mul(z_r, tmp2, tmp1); 			// 2 * z.real
	ap_mul(tmp1, z_i, tmp2); 			// 2 * z.real * z.imag
	ap_add(tmp2, c_i, nz_i);			// 2 * z.real * z.imag + c.imag
}


// Arbitrary precision
// source: https://github.com/RohanFredriksson/glsl-arbitrary-precision

void ap_assign(inout uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE]) {
	for(int assign_i = 0; assign_i <= PRECISION; assign_i++)
		a[assign_i]=b[assign_i];
}

void ap_zero(inout uint a[ARRAY_SIZE]) {
	for(int zero_i = 0; zero_i <= PRECISION; zero_i++)
		a[zero_i] = 0u;
}

void ap_load(inout uint a[ARRAY_SIZE], in float load_value) {
	a[0] = int(load_value < 0.0);
	load_value *= load_value < 0.0 ? -1.0 : 1.0;

	for(int load_i = 1; load_i <= PRECISION; load_i++) {
		a[load_i] = uint(load_value);
		load_value -= a[load_i];
		load_value *= AP_BASE;
	}
}

void ap_shift(inout uint a[ARRAY_SIZE], in uint shift_n) {
	for(uint shift_i = shift_n+1; shift_i <= PRECISION; shift_i++)
		a[shift_i] = a[shift_i-shift_n];

	for(uint shift_i = 1; shift_i<=shift_n; shift_i++)
		a[shift_i] = 0u;
}

void ap_negate(inout uint a[ARRAY_SIZE]) {
	a[0] = a[0] == 0u ? 1u : 0u;
}

void ap_add(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]) {
	uint add_buffer[PRECISION+1]; 
	bool add_pa = a[0] == 0u; 
	bool add_pb = b[0] == 0u; 
	
	if(add_pa == add_pb) {
		uint add_carry = 0u;

		for(int add_i=  PRECISION; add_i > 0; add_i--) {
			uint add_next = uint(a[add_i] + b[add_i] < a[add_i]);
			add_buffer[add_i] = a[add_i] + b[add_i] + add_carry;
			add_carry = add_next;
		}
		add_buffer[0] = uint(!add_pa);

	} else {
		bool add_flip=false;

		for(int add_i = 1; add_i <= PRECISION; add_i++) {
			if(b[add_i] > a[add_i]) {
				add_flip=true; 
				break;
			} 
			if(a[add_i] > b[add_i])
				break;
		}

		uint add_borrow = 0u;
		if(add_flip) {
			for(int add_i = PRECISION; add_i > 0; add_i--) {
				add_buffer[add_i] = b[add_i] - a[add_i] - add_borrow; 
				add_borrow = uint(b[add_i] < a[add_i] + add_borrow);
			}
		} else {
			for(int add_i = PRECISION; add_i > 0; add_i--) {
				add_buffer[add_i] = a[add_i] - b[add_i] - add_borrow; 
				add_borrow = uint(a[add_i] < b[add_i] || a[add_i] < b[add_i] + add_borrow);
			}
		}

		add_buffer[0] = uint(add_pa == add_flip);
	}

	ap_assign(r, add_buffer);
}

void ap_mul(in uint a[ARRAY_SIZE], in uint b[ARRAY_SIZE], out uint r[ARRAY_SIZE]) {
	uint mul_buffer[PRECISION+1];
	uint mul_product[2*PRECISION-1];

	ap_zero(mul_buffer);
	for (uint mul_i = 0; mul_i < 2*PRECISION-1; mul_i++)
		mul_product[mul_i] = 0u;

	for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
		uint mul_carry = 0u; 
		for(int mul_j = 0; mul_j < PRECISION; mul_j++) {
			uint mul_next = 0; 
			uint mul_value = a[PRECISION-mul_i] * b[PRECISION-mul_j]; 
			if (mul_product[mul_i+mul_j] + mul_value < mul_product[mul_i+mul_j])
				mul_next++;
			
			mul_product[mul_i+mul_j] += mul_value; 
			if(mul_product[mul_i+mul_j] + mul_carry < mul_product[mul_i+mul_j])
				mul_next++;
			
			mul_product[mul_i+mul_j] += mul_carry; 
			uint mul_lower_a = a[PRECISION-mul_i] & 0xFFFF; 
			uint mul_upper_a = a[PRECISION-mul_i] >> 16; 
			uint mul_lower_b = b[PRECISION-mul_j] & 0xFFFF; 
			uint mul_upper_b = b[PRECISION-mul_j] >> 16; 
			uint mul_lower = mul_lower_a * mul_lower_b; 
			uint mul_upper = mul_upper_a * mul_upper_b; 
			uint mul_mid = mul_lower_a * mul_upper_b; 
			mul_upper += mul_mid >> 16;
			mul_mid = mul_mid << 16;

			if (mul_lower+mul_mid<mul_lower)
				mul_upper++;

			mul_lower += mul_mid; 
			mul_mid = mul_lower_b * mul_upper_a;
			mul_upper += mul_mid >> 16;
			mul_mid = mul_mid << 16;
			
			if(mul_lower + mul_mid < mul_lower)
				mul_upper++;
			
			mul_carry = mul_upper + mul_next;
		}
		
		if(mul_i + PRECISION < 2*PRECISION-1)
			mul_product[mul_i+PRECISION] += mul_carry;

	}
	if(mul_product[PRECISION-2] >= AP_HALF_BASE) {
		for(int mul_i = PRECISION-1; mul_i < 2*PRECISION-1; mul_i++) {
			if(mul_product[mul_i] + 1 > mul_product[mul_i]) {
				mul_product[mul_i]++;
				break;
			}
			mul_product[mul_i]++;
		}
	}
	for(int mul_i = 0; mul_i < PRECISION; mul_i++) {
		mul_buffer[mul_i+1] = mul_product[2*PRECISION-2-mul_i];
	} if((a[0] == 0u) != (b[0] == 0u)) {
		mul_buffer[0] = 1u;
	}

	ap_assign(r, mul_buffer);
}