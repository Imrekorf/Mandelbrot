#version 460 core
precision highp float;

out vec4 FragColor;

uniform float u_time;
uniform vec2 u_resolution;
uniform float u_zoom = 2.0;
uniform vec2 u_offset;

#define MAX_ITTERATIONS 256

vec4 	mandlebrot(in dvec2 c);
dvec2 	step_mandlebrot(in dvec2 z, in dvec2 c);
vec4 	integerToColor(in int i, in int max_i);

void main()
{
	FragColor = mandlebrot(dvec2((gl_FragCoord.x / u_resolution.x - u_offset.x) * u_zoom, 	// real
								 (gl_FragCoord.y / u_resolution.y - u_offset.y) * u_zoom)); // imag
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