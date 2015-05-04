#include "TruckEntity.h"

#define DEFAULT_TERRAIN_TIMEOUT 5;

#define GRAVITY_MAG -0.0005
#define ENGINE_POWER 0.002f
#define REVERSE_POWER 0.0013f
#define DRAG_FORCE 0.999f

#define FRICTION_FORCE 0.0003f
#define BRAKING_POWER 0.0020f

vec3 getMaxHeightAtEnd(vec3 position, vec3 direction, vec3 direction_cross /* Pointing directly towards the side of the vehicle*/, float width, float length);

TruckEntity::TruckEntity(Mesh * body, Mesh * wheels, Texture * texture, Shader * shader, float scale, vec3 position, float width, float height, float length, float mass) 
	: GameObject(body, texture, shader, scale, position, width, height, length)
{
	children.push_back(GameObject(wheels, texture, shader, scale, position, 0, 0, 0));
	this->mass = mass;
	this->dirV = vec3(0, 0, 1);
}
TruckEntity::~TruckEntity()
{
}

void TruckEntity::Update(double TIME_STEP_SIZE)
{
	if (input::current.W) {
		engineV += getDirection() * ENGINE_POWER;
	}
	if (input::current.S)
	{
		engineV += getDirection() * -REVERSE_POWER;
	}
	if (input::current.Space)
	{
		isBraking = true;
	}
	else
	{
		isBraking = false;
	}

	if (terrainTimeout > 0 && glm::length(velV) != 0)
	{
		float friction = FRICTION_FORCE;
		if (isBraking)
		{
			friction += BRAKING_POWER;
		}

		if (glm::length(velV) <= friction)
			velV = vec3(0, 0, 0);
		else
			engineV -= normalize(velV) * friction; //"friction" slows down moving objects
	}

	vec3 a = calculateAcceleration_simple();

	velV += a;
	translate(velV);
	
	velV *= DRAG_FORCE; //"Drag" force slows down moving objects

	vec3 posV = getPos();
	//printf("pos = %f,%f,%f\n", posV.x, posV.y, posV.z);


	//
	//Calculate the weight distribution
	//
	float weight_dist;
	if (dot(engineV, velV) > 0) { //Vectors are pointing towards the same direction
		weight_dist = glm::length(engineV);
	}
	else { //Vectors are pointing in opposite directions
		weight_dist = -glm::length(engineV);
	}

	//
	//Calculate steering last because forward acceleration needs to be taken into account
	//since it shifts the weight to either the front or back wheels
	//
	if (input::current.A)
	{
		steerLeft(weight_dist);
	}
	if (input::current.D)
	{
		steerRight(weight_dist);
	}


	//Reset acceleration after every update
	engineV = vec3(0);

	terrainTimeout--;

	//Rotate the wheels ONLY
	//children[0].rotate(vec3(0.5, 0, 0));
}

vec3 TruckEntity::getVel()
{
	return velV;
}

vec3 TruckEntity::getDirection()
{
	//Create a rotation matrix
	mat4 R = glm::rotate(mat4(), -getRot().x, vec3(1, 0, 0));
	R = glm::rotate(R, -getRot().y, vec3(0, 1, 0));
	R = glm::rotate(R, -getRot().z, vec3(0, 0, 1));

	//Rotate the default forward direction (w = 0 because we want a vector and not a point)
	vec4 dir = (vec4(0, 0, 1, 0) * R);

	//Ignore the w result as we only need it to do the multiplication
	return vec3(dir.x, dir.y, dir.z);
}

vec3 TruckEntity::getDirection_side()
{
	//Create a rotation matrix
	mat4 R = glm::rotate(mat4(), getRot().x, vec3(1, 0, 0));
	R = glm::rotate(R, -getRot().y, vec3(0, 1, 0));
	R = glm::rotate(R, -getRot().z, vec3(0, 0, 1));

	//Rotate the default side direction (w = 0 because we want a vector and not a point)
	vec4 dir = (vec4(1, 0, 0, 0) * R);

	//Ignore the w result as we only need it to do the multiplication
	return vec3(dir.x, dir.y, dir.z);
}

vec3 TruckEntity::calculateAcceleration_simple()
{
	const vec3 g = vec3(0, -0.007, 0);

	vec3 F = engineV * mass;
	

	vec3 pos = getPos();
	vec3 dir = getDirection();
	vec3 dir_side = getDirection_side(); //Get the direction pointing towards the side of the truck (perpendicular to the up vector of the truck and the direction of the truck)
	
	vec3 F_dir;
	
	if (glm::length(F) != 0)
	{
		F_dir = normalize(F);
	}
	else
	{
		F_dir = normalize(dir);
	}

	float len = getLength(); float width = getWidth(); float height = getHeight();

	vec3 height_at_front = getMaxHeightAtEnd(pos, dir, dir_side, width, len);
	vec3 height_at_back = getMaxHeightAtEnd(pos, -dir, dir_side, width, len);

	//float height_at_left = getMaxHeightAtEnd(pos, dir_side, dir, len, width);
	//float height_at_right = getMaxHeightAtEnd(pos, -dir_side, dir, len, width);

	//This variable is used to prevent the entity from falling through the terrain
	float height_limit = (height_at_front.y + height_at_back.y) / 2;

	//Check if the entity is currently passing through the terrain and fix that
	vec3 currentPos = getPos() - vec3(0, height, 0);
	if (currentPos.y < height_limit)
	{
		currentPos.y = height_limit + height;
		setPos(currentPos);
		terrainTimeout = DEFAULT_TERRAIN_TIMEOUT;
	}
	
	float diff_in_height = height_at_front.y - height_at_back.y;
	vec3 acceleration = vec3(0,0,0);

	
	vec3 newBottom = getPos() + getVel() + g - vec3(0, height, 0);
	//Check if we will be intersecting with the terrain
	if (newBottom.y < height_limit) //about to go through terrain
	{
		acceleration.y -= getVel().y;
		terrainTimeout = DEFAULT_TERRAIN_TIMEOUT;
	}

	float distance_between_heights = glm::distance(height_at_front, height_at_back);

	if (terrainTimeout > 0)
	{
		//Set the rotation
		vec3 rotV = getRot();
		//Trigonometry
		rotV.x = -atan2(height_at_front.y - height_at_back.y, distance_between_heights) / RADIANS_PER_DEGREE;
		//rotV.z = atan2(height_at_left - height_at_right, width) / RADIANS_PER_DEGREE;
		setRot(rotV);

		//
		//Some applied maths to calculate the upward and forward forces
		//
		float sin_theta = diff_in_height / distance_between_heights;
		float cos_theta = glm::sqrt(1 - sin_theta * sin_theta);

		float f_mag = glm::length(F);
		float g_mag = glm::length(g);
		float accel_forward = (f_mag * cos_theta) - (g_mag * cos_theta * sin_theta);
		float accel_downward = (f_mag * sin_theta) + (g_mag * cos_theta * cos_theta) - g_mag;

		if (diff_in_height > 0) {
			printf("%f\n", accel_forward);
		}
		acceleration += vec3(F_dir.x * accel_forward, accel_downward, F_dir.z * accel_forward) / mass;

		//Can only accelerate while on the terrain
		return acceleration;
	}
	else
	{
		vec3 rotV = getRot();
		rotV.x *= 0.99f;
		setRot(rotV);

		//Not touching anything so only gravity affects us
		return g;
	}
}

void TruckEntity::steerLeft(float acc)
{
	float turn_resist = 70.0f * acc;
	float turn_power = max(0.0f, 0.8f - turn_resist);
	steer(turn_power);
}

void TruckEntity::steerRight(float acc)
{
	float turn_resist = 70.0f * acc;
	float turn_power = max(0.0f, 0.8f - turn_resist);
	steer(-turn_power);
}

void TruckEntity::steer(float power)
{
	vec3 vel = getVel();
	float forward_speed = glm::length(vel);
	float rotation_amount = 0;

	if (forward_speed > 0.01)
	{
		rotation_amount = power;
	}
	else if (forward_speed < -0.01)
	{
		rotation_amount = -power;
	}
	else //Not enough speed to steer
	{
		return;
	}

	this->rotate(vec3(0, rotation_amount, 0));

	//
	//Rotate the velocity as well
	//
	vec3 axis = vec3(0, 1, 0);

	quat rotation_quat = angleAxis(rotation_amount, axis);
	velV = glm::rotate(rotation_quat, vel);
}

inline vec3 getMaxHeightAtEnd(vec3 position, vec3 direction, vec3 direction_cross /* Pointing directly towards the side of the vehicle*/, float width, float length)
{
	vec3 highestpoint;

	vec3 offset = (direction_cross * width * 0.5f);
	//Check four different points for the height at the front. Pick the highest value to prevent the entity from passing through terrain
	
	vec3 terrainpos = position + offset + direction * (length * 0.8f);
	float max_height = Terrain::GetHeightAt(terrainpos);
	highestpoint = vec3(terrainpos.x, max_height, terrainpos.z);


	terrainpos = position - offset + direction * (length * 0.8f);
	float temp = Terrain::GetHeightAt(terrainpos);
	if (temp > max_height)
	{
		max_height = temp;
		highestpoint = vec3(terrainpos.x, max_height, terrainpos.z);
	}

	terrainpos = position + offset + direction * length;
	temp = Terrain::GetHeightAt(terrainpos);
	if (temp > max_height)
	{
		max_height = temp;
		highestpoint = vec3(terrainpos.x, max_height, terrainpos.z);
	}

	terrainpos = position - offset + direction * length;
	temp = Terrain::GetHeightAt(terrainpos);
	if (temp > max_height)
	{
		max_height = temp;
		highestpoint = vec3(terrainpos.x, max_height, terrainpos.z);
	}

	return highestpoint;
}