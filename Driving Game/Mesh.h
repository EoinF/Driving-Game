#pragma once
#include <GL/glew.h>
#include <iostream>
#include <string>
#include <assert.h>
#include "obj_parser.h"


using namespace std;

class Mesh
{
public:
	Mesh();
	Mesh(string filename);
	Mesh(GLfloat * vp, GLfloat * vt, GLfloat * vn, int vertex_count);
	~Mesh();
	int vertex_count;
	int uv_count;
	int normals_count;

	GLuint vao;
private:
};

