#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

using namespace glm;

struct BaseLight
{
    vec3 Color;
    //float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    vec3 Colour;
    //float AmbientIntensity;
    vec3 Direction;
    float DiffuseIntensity;
	DirectionalLight(vec3 colour, vec3 direction, float diffuseIntensity)
	{
		Colour = colour;
		Direction = direction;
		DiffuseIntensity = diffuseIntensity;
	}

};