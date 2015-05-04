#pragma once

#include <GL/glew.h>
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;


class GameObject
{
public:
	GameObject(Mesh * mesh, Texture * texture, Shader * shader, float scale, vec3 position, float width, float height, float length, vec4 colour = vec4(1));
	~GameObject();

	static Mesh BoundingCube; //Mesh shared between all game objects
	static vec4 BoundingColour; //Colour of bounding box

	void GameObject::draw();
	virtual void draw(mat4 View, mat4 Projection);
	Mesh * mesh;
	Texture * texture;
	Shader * shader;
	vector<GameObject> children;

	
	vec4 colour;

	void setPos(vec3 p);
	void setRot(vec3 r);
	void setScale(vec3 s);
	
	vec3 getPos();
	vec3 getRot();
	vec3 getScale();
	
	void translate(vec3 t);
	void rotate(vec3 r);
	void scale(vec3 s);

	float getWidth();
	float getHeight();
	float getLength();
private:
	float width, height, length;
	vec3 posV;
	vec3 rotV;
	vec3 scaleV;
	mat4 M;
	void regenerateModel();
};

