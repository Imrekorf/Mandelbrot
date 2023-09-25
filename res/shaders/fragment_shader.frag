#version 460 core
precision highp float;

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_zoom = 1.0;
uniform vec2 u_offset;

#define MAX_ITTERATIONS 256

vec4 	mandlebrot(in dvec2 c);
dvec2 	step_mandlebrot(in dvec2 z, in dvec2 c);
vec4 	integerToColor(in int i, in int max_i);

void main()
{
	vec2 translated = vec2((gl_FragCoord.x / u_resolution.x) - 0.5, (gl_FragCoord.y / u_resolution.y) - 0.5);

	if (		all(greaterThan(translated.xy, vec2( 0.00,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.01)))){
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	} else if (	all(greaterThan(translated.xy, vec2( 0.00, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.01,  0.00)))) {
		FragColor = vec4(0.0, 1.0, 0.0, 1.0);
	} else if (	all(greaterThan(translated.xy, vec2(-0.01,  0.00))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.01)))) {
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	} else if (	all(greaterThan(translated.xy, vec2(-0.01, -0.01))) &&
				all(lessThan(   translated.xy, vec2( 0.00,  0.00)))) {
		FragColor = vec4(1.0, 1.0, 1.0, 1.0);
	} else {
		FragColor = mandlebrot(dvec2((translated * u_zoom) - u_offset));
	}
}

vec4 mandlebrot(in dvec2 c)
{	
	int itterations = 0;

	dvec2 z = dvec2(0.0, 0.0);
	for (; itterations < MAX_ITTERATIONS; itterations++) {
		z = step_mandlebrot(z, c);
		if ((z.x * z.x + z.y * z.y) > (4.0) ) // check if |z| < 2.0
			return integerToColor(itterations, MAX_ITTERATIONS);
	}

	return vec4(0.0, 0.0, 0.0, 1.0);
}

dvec2 step_mandlebrot(in dvec2 z, in dvec2 c)
{
	return vec2(
		(z.x * z.x - z.y * z.y) + c.x,	// z.x = z.real^2 - z.imag^2 + c.real
		(2.0 * z.x * z.y) + c.y			// z.y = 2 * z.real * z.imag + c.imag
	);
}

vec4 integerToColor(in int i, in int max_i)
{
	float _i = i * 255 / max_i;
	return vec4(
		(sin(_i + 0) * 127 + 128) / 255, 
		(sin(_i + 2) * 127 + 128) / 255,
		(sin(_i + 4) * 127 + 128) / 255,
		1.0);
}