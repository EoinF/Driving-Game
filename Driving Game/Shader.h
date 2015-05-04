#pragma once
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <iostream>
#include "lighting.h"

using namespace std;

class Shader
{
public:
	Shader(string shader_name);
	~Shader();
	GLuint id;
	GLint M_loc, V_loc, P_loc;
	GLint tex_loc;
	GLint colour_loc;
	void updateMVP(float * M, float * V, float * P);
	void updateM(float * M);
	void updateColour(float * colour);
	struct {
		GLuint Colour;
		GLuint Direction;
		GLuint DiffuseIntensity;
	} m_dirLightLocation;
	void SetDirectionalLight(const DirectionalLight& Light);
};


void printErrorLog(char *title,GLuint shader);
bool parse_file_into_str (const char* file_name, char* shader_str, int max_len);

