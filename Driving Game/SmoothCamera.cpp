#include "SmoothCamera.h"

SmoothCamera::SmoothCamera(vec3 * pos, vec3 * target, vec3 * up, int WindowWidth, int WindowHeight, GLFWwindow * Window)
{
	posV = *pos;
	targetV = normalize(*target);
	upV = normalize(*up);
	windowWidth = WindowWidth;
	windowHeight = WindowHeight;
	window = Window;
	init();
}


SmoothCamera::~SmoothCamera()
{
}

void SmoothCamera::init()
{
	angleH = 0;
    angleV = 0;

	middleX = windowWidth / 2;
    middleY = windowHeight / 2;

	mouseX = middleX;
    mouseY = middleY;

	glfwSetCursorPos ( window, middleX, middleY);
}

mat4 SmoothCamera::getViewMatrix()
{
	return lookAt(posV, targetV, upV);
}

void SmoothCamera::OnMouse(int x, int y)
{
    const int DeltaX = mouseX - x;
    const int DeltaY = mouseY - y;
	
	ChangeHeading(.08f * DeltaX);
	ChangePitch(.08f * DeltaY);
	
	
	//glfwSetCursorPos ( window, middleX, middleY);

    mouseX = middleX;
    mouseY = middleY;

	//Update();
}

void SmoothCamera::ChangeHeading(float degrees)
{
	const int max_heading_rate = 5;

	if (degrees < -max_heading_rate) {
		degrees = -max_heading_rate;
	} else if (degrees > max_heading_rate) {
		degrees = max_heading_rate;
	}
	//This controls how the heading is changed if the camera is pointed straight up or down
	//The heading delta direction changes
	if (angleV > 90 && angleV < 270 || (angleV < -90 && angleV > -270)) {
		angleH -= degrees;
	} else {
		angleH += degrees;
	}
	//Check bounds for the camera heading
	if (angleH > 360.0f) {
		angleH -= 360.0f;
	} else if (angleH < -360.0f) {
		angleH += 360.0f;
	}
}

void SmoothCamera::ChangePitch(float degrees) {

	const int max_pitch_rate = 5;
	//Check bounds with the max pitch rate so that we aren't moving too fast
	if (degrees < -max_pitch_rate) {
		degrees = -max_pitch_rate;
	} else if (degrees > max_pitch_rate) {
		degrees = max_pitch_rate;
	}
	angleV += degrees;

	//TODO: Prevent camera from going too far up or down

	//Check bounds for the camera pitch
	if (angleV > 360.0f) {
		angleV -= 360.0f;
	} else if (angleV < -360.0f) {
		angleV += 360.0f;
	}
}



void SmoothCamera::Update()
{
	if (input::current.UP)
	{
		translateZ(0.02);
	}
	if (input::current.DOWN)
	{
		translateZ(-0.02);
	}
		
	if (input::current.LEFT)
	{
		translateX(-0.02);
	}
	if (input::current.RIGHT)
	{
		translateX(0.02);
	}

	directionV = normalize(targetV - posV);

	vec3 axis = cross(directionV, upV);

	quat pitch_quat = angleAxis(angleV, axis);
	//determine heading quaternion from the camera up vector and the heading angle
	quat heading_quat = angleAxis(angleH, upV);
	//add the two quaternions
	quat temp = cross(pitch_quat, heading_quat);
	temp = normalize(temp);
	//update the direction from the quaternion
	directionV = rotate(temp, directionV);
	//add the camera delta
	posV += posDeltaV;
	//set the look at to be infront of the camera
	targetV = posV + directionV * 1.0f;
	//damping for smooth camera
	angleH *= .5;
	angleV *= .5;
	posDeltaV = posDeltaV * .8f;

}

void SmoothCamera::translateX(float x)
{
	posDeltaV += cross(directionV, upV) * x;
}
void SmoothCamera::translateZ(float z)
{
	posDeltaV += directionV * z;
}

vec3 SmoothCamera::getDirection()
{
	return directionV;
}