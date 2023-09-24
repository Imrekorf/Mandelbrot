#version 330 core
out vec4 FragColor;

in vec4 vPos; // input variable from vs (same name and type)
uniform float u_time;
uniform vec2 u_resolution;

void main()
{
	vec2 st = gl_FragCoord.xy/u_resolution.xy;
	float fr = u_resolution.x/40.0;
	vec3 x = vec3(st.x);
	vec3 y = vec3(st.y);

	vec3 colorA = vec3(0.5, 0., 0.5);
	vec3 colorB = vec3(1.0, 0.41, 0.71);
	vec3 colorC = vec3(0.0, 1.0, 1.0);

	vec3 color1 = mix(colorA, colorB, fract(vPos.x*fr+cos(u_time)));
	vec3 color2 = mix(colorB, colorC, (vPos.y-sin(u_time))*0.5);
	vec3 color3 = mix(color1, color2, vPos.x*cos(u_time+2.));

	FragColor = vec4(color3, 1.0);
}