#pragma once
#include <GL/glew.h>
#include <iostream>
#include <string>
#include "soil/SOIL.h"

using namespace std;

class Texture
{
public:
	Texture(string filename);
	~Texture(void);
	GLuint id;
};

