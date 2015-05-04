#pragma once

#include "TruckEntity.h"
#include "GLFW\glfw3.h"
#include <math.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Camera
{
public:
	Camera(vec3 * pos, vec3 * target, vec3 * up, int WindowWidth, int WindowHeight, GLFWwindow * window, TruckEntity * etarget, vec3 locationOffset);
	~Camera();
	glm::mat4 getViewMatrix();
	void init();
	void setPos(vec3 newPos);
	void OnMouse(int x, int y);
	void OnRender();
	void Update();
	void ChangeHeading(float degrees);
	void ChangePitch(float degrees);
	vec3 getDirection();
private:
	GLFWwindow * window;

    vec3 posV;
    vec3 targetV;
    vec3 upV;

	vec3 directionV;
	TruckEntity * etarget;
	vec3 locationOffset;

	vec3 oldRotation;

	int windowWidth, windowHeight;
	float angleH, angleV;
    bool OnUpperEdge, OnLowerEdge;
	float mouseX, mouseY;
	float middleX, middleY;
};

const float PI = pi<float>();
const float PIOVER180 = PI / 180.0;
const int MARGIN = 10;