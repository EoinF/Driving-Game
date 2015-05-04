#include "GameObject.h"

#define SHOW_BOUNDING 1


Mesh GameObject::BoundingCube; //Mesh shared between all game objects
vec4 GameObject::BoundingColour; //Colour of bounding box

GameObject::GameObject(Mesh * mesh, Texture * texture, Shader * shader, float scale, vec3 position, float width, float height, float length, vec4 colour)
{
	this->mesh = mesh;
	this->texture = texture;
	this->shader = shader;
	
	rotV = vec3(0,0,0);
	posV = position;
	setScale(vec3(scale, scale, scale));
	
	this->width = width;
	this->height = height;
	this->length = length;
	
	this->colour = colour;
}


GameObject::~GameObject()
{
}

//
// Properties
//
void GameObject::setPos(vec3 p)
{
	vec3 difference = p - posV;
	posV = p;
	for (int c = 0; c < children.size(); c++)
	{
		children[c].translate(difference);
	}
	regenerateModel();
}
void GameObject::setRot(vec3 r)
{
	vec3 difference = r - rotV;
	rotV = r;
	for (int c = 0; c < children.size(); c++)
	{
		children[c].rotate(difference);
	}
	regenerateModel();
}
void GameObject::setScale(vec3 s)
{
	vec3 difference = s - scaleV;
	scaleV = s;
	for (int c = 0; c < children.size(); c++)
	{
		children[c].scale(difference);
	}
	regenerateModel();
}
	
vec3 GameObject::getPos()
{
	return posV;
}
vec3 GameObject::getRot()
{
	return rotV;
}
vec3 GameObject::getScale()
{
	return scaleV;
}

float GameObject::getWidth()
{
	return width * scaleV.x;
}
float GameObject::getHeight()
{
	return height * scaleV.y;
}
float GameObject::getLength()
{
	return length * scaleV.z;
}

//
// Methods
//
void GameObject::translate(vec3 t)
{
	setPos(posV + t);
}
void GameObject::rotate(vec3 r)
{
	setRot(rotV + r);
}
void GameObject::scale(vec3 s)
{
	setScale(scaleV + s);
}

void GameObject::draw()
{
	glUseProgram(shader->id);

	//Draw the object
	shader->updateM(value_ptr(M));
	shader->updateColour(value_ptr(colour));

	glBindVertexArray(mesh->vao);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);

}

void GameObject::draw(mat4 V, mat4 P)
{
	glUseProgram (shader->id);

	//Draw the object
	shader->updateMVP(value_ptr(M), value_ptr(V), value_ptr(P));
	
	shader->updateColour(value_ptr(colour));

	glBindVertexArray(mesh->vao);

	if (texture != NULL) 
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
	
	//Draw the bounding box
#ifdef DEBUG
	mat4 newM = glm::scale(M, vec3(width, height, length));
		
	shader->updateM(value_ptr(newM));
	shader->updateColour(value_ptr(BoundingColour));

	glBindVertexArray(BoundingCube.vao);
	if (texture != NULL) 
	{
		glBindTexture(GL_TEXTURE_2D, texture->id);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glDrawArrays(GL_TRIANGLES, 0, BoundingCube.vertex_count);

#endif

	for (int c = 0; c < children.size(); c++)
	{
		children[c].draw(V, P);
	}
}


//
//Helper methods
//
void GameObject::regenerateModel()
{
	M = glm::translate(mat4(), posV);

	M = glm::rotate(M, rotV.y, vec3(0, 1, 0));
	M = glm::rotate(M, rotV.z, vec3(0, 0, 1));
	M = glm::rotate(M, rotV.x, vec3(1, 0, 0));

	M = glm::scale(M, scaleV);
}