#pragma once

#include "Camera.h"
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

class SmoothCamera
{
public:
	SmoothCamera(vec3 * pos, vec3 * target, vec3 * up, int WindowWidth, int WindowHeight, GLFWwindow * window);
	~SmoothCamera();
	glm::mat4 getViewMatrix();
	void init();
	void translateX(float x);
	void translateZ(float z);
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
	
    vec3 posDeltaV;
	int windowWidth, windowHeight;
	float angleH, angleV;
    bool OnUpperEdge, OnLowerEdge;
	float mouseX, mouseY;
	float middleX, middleY;
};
