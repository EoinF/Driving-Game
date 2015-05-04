#pragma once

#include <GL/glew.h>
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Terrain.h"
#include "InputManager.h"

using namespace glm;

const float RADIANS_PER_DEGREE = 0.0174532925;

class TruckEntity : public GameObject
{
public:
	TruckEntity(Mesh * body, Mesh * wheels, Texture * texture, Shader * shader, float scale, vec3 position, float width, float height, float length, float mass = 0.5);
	~TruckEntity();
	void Update(double TIME_STEP_SIZE);
	vec3 getVel();
	vec3 getDirection();
	vec3 getDirection_side();
	
	void steerLeft(float acceleration);
	void steerRight(float acceleration);
private:
	int terrainTimeout;
	float mass;
	vec3 velV; //velocity
	vec3 dirV; //direction
	vec3 engineV;
	vec3 calculateAcceleration_simple();
	vec3 calculateAcceleration_slope();
	bool isBraking;
	void steer(float power);
};

