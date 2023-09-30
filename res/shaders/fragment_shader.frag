#version 460 core
precision highp float;

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