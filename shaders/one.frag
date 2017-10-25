#version 150 core

precision highp float;

#define PI 3.14159265359

uniform sampler2D tex0;

in vec2 coord;
out vec4 target;

float dt = 1./2048.;
float k = 1e0;
//vec2 position[] = {vec2(0.)};
vec2 position[] = {vec2(1., 0.)/2., vec2(cos(2.*PI/3.), sin(2.*PI/3.))/2., vec2(cos(4.*PI/3.), sin(4.*PI/3.))/2.};
uint len = 3;
int i;

void main() {
	vec2 st = coord.st;
	st += vec2(1.0);
	st /= 2.0;
	vec4 position_velocity = texture(tex0, st);
	vec2 pos = position_velocity.st;
	vec2 speed = position_velocity.pq;
	vec2 acc = vec2(0.);
	for(i = 0; i < len; i++) {
		//vec2 a = k / pow(length(position[i] - pos), 2.) * normalize(position[i] - pos);
		vec2 a;
		float epsilon = 1e-6, l = length(position[i] - pos);
		if(l > epsilon) {
			a = k * pow(l, -2.) * normalize(position[i] - pos);
		}
		else {
			float s = length(speed);
			if(s != 0.) {
				a = 60 * speed / s;
			}
			else {
				vec2 o = vec2(0.);
				a = k * pow(length(o - pos), -3.) * (o - pos);
			}
			
		}
		//acc += clamp(vec2(-60.), vec2(60), a);
		acc += a;
	}
	//pos += speed * dt;
	//speed += acc * dt;
	target = vec4(pos + speed * dt, speed + acc * dt);
	//target = texture(tex0, st);
	//target = position_velocity;
	//target = vec4(pos, vec2(2.0));
	//target = vec4(vec3(length(beta)), 1.);
}

