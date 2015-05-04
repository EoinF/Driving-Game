#pragma once;
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtx/std_based_type.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "soil/SOIL.h"
#include "Shader.h"

using namespace glm;

namespace Terrain
{
	extern int m_HeightmapWidth;
	extern int m_HeightmapLength;

    void create_Terrain(Shader * shader, const std::string& filename_texture,  const std::string& filename_heightmap, float heightScale = 0.02f, float blockScale = 1.0f);
    void delete_Terrain();
 
    // Get the height of the terrain at a position in world space
    float GetHeightAt( const vec3& position );

    void Render();
}