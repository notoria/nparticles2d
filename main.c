#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>

#define SIZE (1 << 10)
#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

int window;
//const int WIDTH = 512, HEIGHT = 512;
const int WIDTH = 768, HEIGHT = 768;
GLuint vao[] = {0, 0}, vbo[] = {0, 0}, fbo[] = {0, 0}, tex[] = {0, 0};
GLuint program[2] = {0};
GLfloat vertices[] = {
	-1.0, -1.0, 1.0, -1.0, 1.0, 1.0, // first triangle
	-1.0, -1.0, 1.0, 1.0, -1.0, 1.0 // seconde triangle
};
GLenum draw_buffers = {GL_COLOR_ATTACHMENT0};
GLfloat *result;
int next;

void check_error(char *s)
{
	int err = glGetError();
	switch(err) {
	//case GL_NO_ERROR:
	case GL_INVALID_ENUM:
		fprintf(stderr, "enum: %s\n", s);
		abort();
		break;
	case GL_INVALID_VALUE:
		fprintf(stderr, "value: %s\n", s);
		abort();
		break;
	case GL_INVALID_OPERATION:
		fprintf(stderr, "operation: %s\n", s);
		abort();
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		fprintf(stderr, "framebuffer: %s\n", s);
		abort();
		break;
	case GL_OUT_OF_MEMORY:
		fprintf(stderr, "out of memory: %s\n", s);
		abort();
		break;
	case GL_STACK_UNDERFLOW:
		fprintf(stderr, "stack underflow: %s\n", s);
		abort();
		break;
	case GL_STACK_OVERFLOW:
		fprintf(stderr, "stack overflow: %s\n", s);
		abort();
		break;
	default:
		break;
	}
}

GLuint compile_shader(char *src, GLenum ty)
{
	GLuint shader;
	GLint status = GL_FALSE;
	GLuint len = 0;
	GLchar *buf = NULL;
	check_error("begin compile_shader");
	shader = glCreateShader(ty);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if(status != GL_TRUE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		buf = malloc(sizeof(GLchar) * (len + 1));
		glGetShaderInfoLog(shader, len, NULL, buf);
		buf[len] = 0;
		fprintf(stderr, "%s\n", buf);
		abort();
	}
	return shader;
}

GLuint link_program(GLuint vs, GLuint fs)
{
	GLuint program;
	GLint status = GL_FALSE;
	GLuint len = 0;
	GLchar *buf = NULL;
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status != GL_TRUE) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
		buf = malloc(sizeof(GLchar) * (len + 1));
		glGetProgramInfoLog(program, len, NULL, buf);
		buf[len] = 0;
		fprintf(stderr, "%s\n", buf);
		abort();
	}
	return program;
}

char* read_file(char *name)
{
	GLchar *str = NULL;
	FILE *file = NULL;
	GLuint len = 0;
	file = fopen(name, "r");
	if(!file) {
		fprintf(stderr, "Error: can't open file %s\n", name);
		abort();
	}
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);
	str = malloc(sizeof(GLchar) * (len + 1));
	fread(str, sizeof(GLchar), len, file);
	fclose(file);
	str[len] = 0;
	return str;
}

void init()
{
	GLchar *vs_src, *fs_src;
	GLuint vs, fs;
	GLint u_pos;
	GLfloat *data = NULL;
	FILE *file = NULL;
	int i;

	next = 0;

	for(i = 0; i < sizeof(vertices) / sizeof(GLfloat); i++) {
		vertices[i] *= SIZE;
	}

	// retrieve data
	data = malloc(sizeof(GLfloat) * SIZE * SIZE * 4);
	file = fopen("position_init", "r");
	if(!file || !data) {
		fprintf(stderr, "Error: can't initialize texture\n");
		abort();
	}
	fread(data, sizeof(GLfloat), SIZE * SIZE * 4, file);
	fclose(file);

	/*
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SIZE, SIZE, GL_RGBA_FLOAT32_ATI,
		GL_FLOAT, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	*/

	// Create textures
	glGenTextures(2, tex);
	for(i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		if(!i)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI, SIZE, SIZE, 0,
				GL_RGBA, GL_FLOAT, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA_FLOAT32_ATI, SIZE, SIZE, 0,
				GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	free(data);
	check_error("create texture");

	// Create framebuffers
	glGenFramebuffers(2, fbo);
	for(i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
		glViewport(0, 0, SIZE, SIZE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, tex[i], 0);
		glDrawBuffers(1, &draw_buffers);
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER)
			!= GL_FRAMEBUFFER_COMPLETE) {
			fprintf(stderr, "Framebuffer id %d\n", i);
			abort();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	check_error("create framebuffer");

	// Create programs
	vs_src = read_file("shaders/one.vert");
	fs_src = read_file("shaders/one.frag");
	vs = compile_shader(vs_src, GL_VERTEX_SHADER);
	fs = compile_shader(fs_src, GL_FRAGMENT_SHADER);
	program[0] = link_program(vs, fs);
	free(vs_src);
	free(fs_src);
	glDeleteShader(vs);
	glDeleteShader(fs);

	vs_src = read_file("shaders/two.vert");
	fs_src = read_file("shaders/two.frag");
	vs = compile_shader(vs_src, GL_VERTEX_SHADER);
	fs = compile_shader(fs_src, GL_FRAGMENT_SHADER);
	program[1] = link_program(vs, fs);
	free(vs_src);
	free(fs_src);
	glDeleteShader(vs);
	glDeleteShader(fs);
	check_error("create program");

	// Create vao
	glGenVertexArrays(2, vao);
	glGenBuffers(2, vbo);

	glUseProgram(program[0]);
	glBindFragDataLocation(program[0], 0, "target");
	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	u_pos = glGetAttribLocation(program[0], "u_pos");
	glEnableVertexAttribArray(u_pos);
	glVertexAttribPointer(u_pos, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
	
	glUseProgram(program[1]);
	glBindFragDataLocation(program[1], 0, "target");
	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * SIZE * SIZE, NULL,
		GL_DYNAMIC_DRAW);
	u_pos = glGetAttribLocation(program[1], "u_pos");
	glEnableVertexAttribArray(u_pos);
	glVertexAttribPointer(u_pos, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);
	glUseProgram(0);
	check_error("create vao");

	result = malloc(4 * sizeof(GLfloat) * SIZE * SIZE);
	if(!result) {
		fprintf(stderr, "Error: can't allocate enough memory %d bytes\n",
			SIZE * SIZE);
		abort();
	}
}

void display()
{
	GLint u_pos;
	int i;
	// Read
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[next]);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(0, 0, SIZE, SIZE, GL_RGBA, GL_FLOAT, result);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Switch
	//next = 1 - next;
	next = !next;

	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0., 0., 0., 1.);

	// Compute
	glBindFramebuffer(GL_FRAMEBUFFER, fbo[next]);
	glViewport(0, 0, SIZE, SIZE);
	glUseProgram(program[0]);
	glBindVertexArray(vao[0]);

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex[1 - next]);
	glBindTexture(GL_TEXTURE_2D, tex[!next]);
	glUniform1i(glGetUniformLocation(program[0], "tex0"), 0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDrawBuffers(1, &draw_buffers);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	// Draw
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WIDTH, HEIGHT);
	glUseProgram(program[1]);
	glBindVertexArray(vao[1]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * SIZE * SIZE * 4, result);

	glDrawArrays(GL_POINTS, 0, SIZE * SIZE);
	glFlush();
	glBindVertexArray(0);
	glUseProgram(0);

	for(i = 0; i < 4 * SIZE * SIZE; i++)
		result[i] = 0.;
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	if(key == 0x1b)
		exit(0);
}

void idle()
{
	glutSetWindow(window);
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
   	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WIDTH, HEIGHT);
	window = glutCreateWindow(argv[0]);

	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

