#include "Camera.h"

Camera::Camera(vec3 * pos, vec3 * target, vec3 * up, int WindowWidth, int WindowHeight, GLFWwindow * Window, TruckEntity * etarget, vec3 locationOffset)
{
	posV = *pos;
	targetV = normalize(*target);
	upV = normalize(*up);
	oldRotation = vec3(0);
	windowWidth = WindowWidth;
	windowHeight = WindowHeight;
	window = Window;
	this->etarget = etarget;
	this->locationOffset = locationOffset;
	init();
}


Camera::~Camera()
{
}

void Camera::init()
{
	angleH = 0;
    angleV = 0;

	middleX = windowWidth / 2;
    middleY = windowHeight / 2;

	mouseX = middleX;
    mouseY = middleY;

	glfwSetCursorPos ( window, middleX, middleY);
}

mat4 Camera::getViewMatrix()
{
	return lookAt(posV, targetV, upV);
}

void Camera::OnMouse(int x, int y)
{
    const int DeltaX = mouseX - x;
    const int DeltaY = mouseY - y;
	
	ChangeHeading(.08f * DeltaX);
	ChangePitch(.08f * DeltaY);

    mouseX = middleX;
    mouseY = middleY;

	Update();
}

void Camera::ChangeHeading(float degrees)
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

void Camera::ChangePitch(float degrees) {

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



void Camera::Update()
{
	directionV = normalize(targetV - posV);

	//Get the rotation change due to the entity target rotating
	vec3 newRotation = etarget->getRot();
	vec3 deltaRotation = newRotation - oldRotation;
	oldRotation = newRotation;

	vec3 axis = cross(directionV, upV);

	quat pitch_quat = angleAxis(angleV + deltaRotation.x, axis);
	//determine heading quaternion from the camera up vector and the heading angle
	quat heading_quat = angleAxis(angleH + deltaRotation.y, upV);
	//add the two quaternions
	quat temp = cross(pitch_quat, heading_quat);
	temp = normalize(temp);
	//update the direction from the quaternion
	directionV = rotate(temp, directionV);

	vec3 eDir = etarget->getDirection();
	vec3 eDir_side = etarget->getDirection_side();
	vec3 eUp = cross(eDir, eDir_side);
	posV = etarget->getPos() 
		+ locationOffset.x * eDir_side 
		+ locationOffset.y * eUp
		+ locationOffset.z * eDir;
	

	//set the look at to be infront of the camera
	targetV = posV + directionV * 1.0f;
	//damping for smooth camera
	angleH *= .5;
	angleV *= .5;
}

void Camera::setPos(vec3 newPos)
{
	posV = newPos;
}

vec3 Camera::getDirection()
{
	return directionV;
}