#version 460 core
precision highp float;


// Arbitrary precision
// source: https://github.com/RohanFredriksson/glsl-arbitrary-precision
const int PRECISION = 3;
const int ARRAY_SIZE = (PRECISION+1);
const float BASE = 4294967296.0;
const uint HALF_BASE = 2147483648u;

#define assign(x, y) {for(int assign_i=0;assign_i<=PRECISION;assign_i++){x[assign_i]=y[assign_i];}}
#define zero(x) {for(int zero_i=0;zero_i<=PRECISION;zero_i++){x[zero_i]=0u;}}
#define load(x, v) {float load_value=(v); if (load_value<0.0) {x[0]=1u; load_value*=-1.0;} else {x[0]=0u;} for(int load_i=1; load_i<=PRECISION; load_i++) {x[load_i]=uint(load_value); load_value-=x[load_i]; load_value*=BASE;}}
#define shift(x, v) {int shift_n=(v); for(int shift_i=shift_n+1; shift_i<=PRECISION; shift_i++) {x[shift_i]=x[shift_i-shift_n];} for(int shift_i=1; shift_i<=shift_n; shift_i++) {x[shift_i]=0u;}};
#define negate(x) {x[0]=(x[0]==0u?1u:0u);}
#define add(a, b, r) {uint add_buffer[PRECISION+1]; bool add_pa=a[0]==0u; bool add_pb=b[0]==0u; if (add_pa==add_pb) {uint add_carry=0u; for(int add_i=PRECISION; add_i>0; add_i--) {uint add_next=0u; if(a[add_i]+b[add_i]<a[add_i]) {add_next=1u;} add_buffer[add_i]=a[add_i]+b[add_i]+add_carry; add_carry=add_next;} if(!add_pa) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} else {bool add_flip=false; for(int add_i=1; add_i<=PRECISION; add_i++) {if(b[add_i]>a[add_i]) {add_flip=true; break;} if(a[add_i]>b[add_i]) {break;}} if(add_flip) {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=b[add_i]-a[add_i]-add_borrow; if(b[add_i]<a[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} else {uint add_borrow=0u; for(int add_i=PRECISION; add_i>0; add_i--) {add_buffer[add_i]=a[add_i]-b[add_i]-add_borrow; if(a[add_i]<b[add_i]||a[add_i]<b[add_i]+add_borrow) {add_borrow=1u;} else {add_borrow=0u;}}} if(add_pa==add_flip) {add_buffer[0]=1u;} else {add_buffer[0]=0u;}} assign(r, add_buffer);}
#define mul(a, b, r) {uint mul_buffer[PRECISION+1]; zero(mul_buffer); uint mul_product[2*PRECISION-1]; for(int mul_i=0; mul_i<2*PRECISION-1; mul_i++) {mul_product[mul_i]=0u;} for(int mul_i=0; mul_i<PRECISION; mul_i++) {uint mul_carry=0u; for(int mul_j=0; mul_j<PRECISION; mul_j++) {uint mul_next=0; uint mul_value=a[PRECISION-mul_i]*b[PRECISION-mul_j]; if(mul_product[mul_i+mul_j]+mul_value<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_value; if(mul_product[mul_i+mul_j]+mul_carry<mul_product[mul_i+mul_j]) {mul_next++;} mul_product[mul_i+mul_j]+=mul_carry; uint mul_lower_a=a[PRECISION-mul_i]&0xFFFF; uint mul_upper_a=a[PRECISION-mul_i]>>16; uint mul_lower_b=b[PRECISION-mul_j]&0xFFFF; uint mul_upper_b=b[PRECISION-mul_j]>>16; uint mul_lower=mul_lower_a*mul_lower_b; uint mul_upper=mul_upper_a*mul_upper_b; uint mul_mid=mul_lower_a*mul_upper_b; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;} mul_lower+=mul_mid; mul_mid=mul_lower_b*mul_upper_a; mul_upper+=mul_mid>>16; mul_mid=mul_mid<<16; if(mul_lower+mul_mid<mul_lower) {mul_upper++;}; mul_carry=mul_upper+mul_next;} if(mul_i+PRECISION<2*PRECISION-1) {mul_product[mul_i+PRECISION]+=mul_carry;}} if(mul_product[PRECISION-2]>=HALF_BASE) {for(int mul_i=PRECISION-1; mul_i<2*PRECISION-1; mul_i++) {if(mul_product[mul_i]+1>mul_product[mul_i]) {mul_product[mul_i]++; break;} mul_product[mul_i]++;}} for(int mul_i=0; mul_i<PRECISION; mul_i++) {mul_buffer[mul_i+1]=mul_product[2*PRECISION-2-mul_i];} if((a[0]==0u)!=(b[0]==0u)) {mul_buffer[0]=1u;}; assign(r, mul_buffer);}
// end arbitrary precision

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_zoom = 1.0;
uniform vec2 u_offset;

#define PI 				3.1415926538

#define MAX_ITTERATIONS (256)
#define COLOR_REPEAT	3
#define DEBUG_SQUARE

const float ln_max_ittr = log(MAX_ITTERATIONS+1);

vec4 	mandelbrot(in dvec2 c);
dvec2 	step_mandelbrot(in dvec2 z, in dvec2 c);
vec4 	integerToColor(in float i);

vec4 	mandelbrot_arbprec(in vec2 c, in vec2 offset, in float zoom_lvl);
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

	// FragColor = mandelbrot(dvec2((translated * u_zoom) - u_offset));
	FragColor = mandelbrot_arbprec(translated, u_offset, u_zoom);
}

vec4 mandelbrot(in dvec2 c)
{	
	int itterations = 0;

	dvec2 z = dvec2(0.0, 0.0);
	for (; itterations < MAX_ITTERATIONS; itterations++) {
		z = step_mandelbrot(z, c);
		if ((z.x * z.x + z.y * z.y) > (4.0) ) // check if |z| < 2.0
			return integerToColor(itterations);
	}

	return vec4(0.0, 0.0, 0.0, 1.0);
}

dvec2 step_mandelbrot(in dvec2 z, in dvec2 c)
{
	return vec2(
		(z.x * z.x - z.y * z.y) + c.x,	// z.x = z.real^2 - z.imag^2 + c.real
		(2.0 * z.x * z.y) + c.y			// z.y = 2 * z.real * z.imag + c.imag
	);
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

vec4 mandelbrot_arbprec(in vec2 c, in vec2 offset, in float zoom_lvl)
{
	uint c_r[ARRAY_SIZE];
    uint c_i[ARRAY_SIZE];
    uint z_r[ARRAY_SIZE];
    uint z_i[ARRAY_SIZE];
	
	uint nz_r[ARRAY_SIZE];
    uint nz_i[ARRAY_SIZE];

	uint offset_r[ARRAY_SIZE];
	uint offset_i[ARRAY_SIZE];
	uint zoom[ARRAY_SIZE];
	uint zoom_mult[ARRAY_SIZE];

	load(c_r, c.x);
	load(c_i, c.y);
	load(offset_r, offset.x);
	load(offset_i, offset.y);
	
	if (abs(zoom_lvl) > 0.01) {
		load(zoom, 1.0);
		load(zoom_mult, (zoom_lvl <= 0 ? 2.0 : 0.5));
		
		for (int i = 0; i < abs(floor(zoom_lvl)); i++){
			mul(zoom, zoom_mult, zoom); // zoom/(2 * zoom_lvl)
		}
		if (fract(zoom_lvl) > 0.01) {
			load(zoom_mult, pow(2, -1*fract(zoom_lvl)));
			mul(zoom, zoom_mult, zoom);
		}
		mul(c_r, zoom, c_r);
		mul(c_i, zoom, c_i);
	}
	
	negate(offset_r);
	negate(offset_i);
	add(c_r, offset_r, c_r);
	add(c_i, offset_i, c_i);

	zero(z_r);
	zero(z_i);

	int itterations = 0;
	for (; itterations < MAX_ITTERATIONS; itterations++) {
		uint a_sqr[ARRAY_SIZE];
        uint b_sqr[ARRAY_SIZE];
        mul(z_r, z_r, a_sqr);
        mul(z_i, z_i, b_sqr);
        add(a_sqr, b_sqr, a_sqr); // pretend a_sqr here is r_sqr
        
        if (a_sqr[1] > 4) { // pretend a_sqr here is r_sqr
            return integerToColor(itterations);
        }
        
		step_mandelbrot_arb_prec(z_r, z_i, c_r, c_i, nz_r, nz_i);
		assign(z_r, nz_r);
		assign(z_i, nz_i);
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
	mul(z_r, z_r, tmp1); 			// z.real^2
	mul(z_i, z_i, tmp2); 			// z.imag^2
	negate(tmp2);					// z.imag^2 * -1
	add(tmp1, tmp2, tmp1);			// z.real^2 - z.imag^2
	add(tmp1, c_r, nz_r);			// z.real^2 - z.imag^2 + c.real

	// calculate 'z.imag
	load(tmp2, 2.0);
	mul(z_r, tmp2, tmp1); 			// 2 * z.real
	mul(tmp1, z_i, tmp2); 			// 2 * z.real * z.imag
	add(tmp2, c_i, nz_i);			// 2 * z.real * z.imag + c.imag
}