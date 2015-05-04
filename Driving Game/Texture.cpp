#include "Texture.h"


Texture::Texture(string filename)
{
	//
	// Load image from file
	//
	this->id = SOIL_load_OGL_texture("urbanbuilding.png", SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

	if (this->id == 0)
	{
		printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
	}

	//
	// Bind texture and set parameters
	//
	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture( GL_TEXTURE_2D, 0 );
}


Texture::~Texture()
{
}
