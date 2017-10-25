#version 150 core

precision highp float;

#define PI 3.14159265359

in vec4 position_velocity;
out vec4 target;

vec3 hsb2rgb(in vec3 c) {
	vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0,0.0,1.0);
	rgb = rgb*rgb*(3.0-2.0*rgb);
	return c.z*mix(vec3(1.0),rgb,c.y);
}

void main() {
	float max_speed = 5.;
	vec2 pos = position_velocity.st;
	vec2 speed = position_velocity.pq;
	float s;
	//s = smoothstep(0., 1., 1. - length(speed)/max_speed);
	s = clamp(0., 0.6, (1. - length(speed)/max_speed)*0.6);
	vec3 color = hsb2rgb(vec3(s, 1.0, 1.0));
	if(abs(dot(pos, speed)) > 0.1)
		discard;
	target = vec4(color, 1.0);
}

