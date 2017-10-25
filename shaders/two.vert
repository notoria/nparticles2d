#version 150 core

in vec4 u_pos;
out vec4 position_velocity;

void main() {
	float k = 1/2.;
	mat4 zoom = mat4(mat3(k));
	position_velocity = u_pos;
	// xyzw = rgba = stpq
	//gl_Position = vec4(u_pos.pq, 0.0, 1.0); // wrong but interessing
	gl_Position = zoom * vec4(u_pos.st, 0.0, 1.0);
}

