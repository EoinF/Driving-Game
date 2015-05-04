#include "Terrain.h"

namespace Terrain
{
	float COLOUR_WHITE[4] = { 1, 1, 1, 1 };

	void Terminate();
	bool LoadHeightmap(const std::string& filename);
	bool LoadTexture(const std::string& filename);

	void GenerateVertices();
	void GenerateNormals();
	void GenerateVertexBuffers();

	typedef std::vector<vec3>  PositionBuffer;
	typedef std::vector<vec3>  NormalBuffer;
	typedef std::vector<vec2>  TexCoordBuffer;
	typedef std::vector<vec3>  VertexBuffer;

	PositionBuffer  m_PositionBuffer;
	NormalBuffer    m_NormalBuffer;
	TexCoordBuffer  m_Tex0Buffer;
	VertexBuffer     m_VertexBuffer;

	GLuint vao;

	// ID's for the VBO's
	GLuint  points_vbo;
	GLuint  normals_vbo;
	GLuint  texcoords_vbo;
	GLuint  texture_vbo;

	mat4 M;
	mat4 invM;
	Shader * shader;

	int m_HeightmapWidth;
	int m_HeightmapLength;

	// The height-map value will be multiplied by this value
	// before it is assigned to the vertex's Y-coordinate
	float   m_fHeightScale;
	// The vertex's X and Z coordinates will be multiplied by this
	// for each step when building the terrain
	float   m_fBlockScale;


	inline float GetPercentage(float value, const float min, const float max)
	{
		value = glm::clamp(value, min, max);
		return (value - min) / (max - min);
	}

	// Get's the size of the file in bytes.
	inline int GetFileLength(std::istream& file)
	{
		int pos = file.tellg();
		file.seekg(0, std::ios::end);
		int length = file.tellg();
		// Restore the position of the get pointer
		file.seekg(pos);

		return length;
	}


	inline void DeleteVertexBuffer(GLuint& vboID)
	{
		if (vboID != 0)
		{
			glDeleteBuffers(1, &vboID);
			vboID = 0;
		}
	}

	inline void CreateVertexBuffer(GLuint& vboID)
	{
		// Make sure we don't lose the reference to the previous VBO if there is one
		DeleteVertexBuffer(vboID);
		glGenBuffers(1, &vboID);
	}

	inline void DeleteTexture(GLuint& texID)
	{
		if (texID != 0)
		{
			glDeleteTextures(1, &texID);
			texID = 0;
		}
	}

	inline void CreateTexture(GLuint& texID)
	{
		DeleteTexture(texID);
		glGenTextures(1, &texID);
	}

	void Terrain::create_Terrain(Shader * terrain_shader, const std::string& filename_heightmap, const std::string& filename_texture, float heightScale /* = 500.0f */, float blockScale /* = 2.0f */)
	{
		m_fBlockScale = blockScale;
		m_fHeightScale = heightScale;
		shader = terrain_shader;
		LoadHeightmap(filename_heightmap);
		LoadTexture(filename_texture);

		GenerateVertexBuffers();

		std::cout << "Terrain has been loaded!" << std::endl;
	}

	void Terrain::delete_Terrain()
	{
		Terminate();
	}

	void Terminate()
	{
		DeleteVertexBuffer(points_vbo);
		DeleteVertexBuffer(normals_vbo);
		DeleteVertexBuffer(texcoords_vbo);

		DeleteTexture(texture_vbo);
	}

	bool LoadTexture(const std::string& filename)
	{
		DeleteTexture(texture_vbo);

		texture_vbo = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

		if (texture_vbo != 0)
		{
			glBindTexture(GL_TEXTURE_2D, texture_vbo);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		return (texture_vbo != 0);
	}

	bool LoadHeightmap(const std::string& filename)
	{
		int width, height, channels;
		unsigned char* heightMap = SOIL_load_image(filename.c_str(), &width, &height, &channels, 0);

		unsigned int numPixels = width * height;
		m_PositionBuffer.resize(numPixels);

		m_HeightmapWidth = width;
		m_HeightmapLength = height;

		// Size of the terrain in world units
		float terrainWidth = (width - 1) * m_fBlockScale;
		float terrainHeight = (height - 1) * m_fBlockScale;

		float halfTerrainWidth = terrainWidth * 0.5f;
		float halfTerrainHeight = terrainHeight * 0.5f;

		float colourData;

		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				//Multiply coordinate by 3, because there is one value of rgb respectively per byte
				colourData = (unsigned int)heightMap[(x + y * width) * 3];

				m_PositionBuffer[x + y * width] = vec3(x, colourData * m_fHeightScale, -y);
			}
		}
		GenerateVertices();
		GenerateNormals();

		return true;
	}

	void GenerateVertices()
	{
		if (m_HeightmapWidth < 2 || m_HeightmapLength < 2)
		{
			// Terrain hasn't been loaded, or is of an incorrect size
			return;
		}

		const unsigned int terrainWidth = m_HeightmapWidth;
		const unsigned int terrainHeight = m_HeightmapLength;

		// 2 triangles for every quad of the terrain mesh
		const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

		m_VertexBuffer.resize(numTriangles * 3);
		m_NormalBuffer.resize(numTriangles * 3);
		m_Tex0Buffer.resize(numTriangles * 3);

		unsigned int index = 0; // Index in the index buffer
		for (unsigned int y = 0; y < (terrainHeight - 1); ++y)
		{
			for (unsigned int x = 0; x < (terrainWidth - 1); ++x)
			{
				vec3 lowerLeft = m_PositionBuffer[x + y*terrainWidth];
				vec3 lowerRight = m_PositionBuffer[(x + 1) + y*terrainWidth];
				vec3 topLeft = m_PositionBuffer[x + (y + 1) * terrainWidth];
				vec3 topRight = m_PositionBuffer[(x + 1) + (y + 1) * terrainWidth];	

				//
				//Draw two triangles for each square pixel of heightmap data
				//

				//Triangle 0

				//vertices
				m_VertexBuffer[index] = topLeft;
				m_VertexBuffer[index + 1] = lowerLeft;
				m_VertexBuffer[index + 2] = lowerRight;

				//texture coordinates
				m_Tex0Buffer[index] = vec2(0, 0);
				m_Tex0Buffer[index + 1] = vec2(0, 1);
				m_Tex0Buffer[index + 2] = vec2(1, 1);

				//Triangle 1

				//vertices
				m_VertexBuffer[index + 3] = topLeft;
				m_VertexBuffer[index + 4] = lowerRight;
				m_VertexBuffer[index + 5] = topRight;

				//texture coordinates
				m_Tex0Buffer[index + 3] = vec2(0, 0);
				m_Tex0Buffer[index + 4] = vec2(1, 1);
				m_Tex0Buffer[index + 5] = vec2(1, 0);

				index += 6;
			}
		}
	}

	void GenerateNormals()
	{
		for (unsigned int i = 0; i < m_VertexBuffer.size(); i += 3)
		{
			glm::vec3 v0 = m_VertexBuffer[i];
			glm::vec3 v1 = m_VertexBuffer[i + 1];
			glm::vec3 v2 = m_VertexBuffer[i + 2];

			glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

			m_NormalBuffer[i] += normal;
			m_NormalBuffer[i + 1] += normal;
			m_NormalBuffer[i + 2] += normal;
		}

		for (unsigned int i = 0; i < m_NormalBuffer.size(); ++i)
		{
			m_NormalBuffer[i] = glm::normalize(m_NormalBuffer[i]);
		}
	}

	void GenerateVertexBuffers()
	{
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//
		//Points vbo
		//
		glGenBuffers(1, &points_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, points_vbo);

		// copy our points from the mesh into our VBO on graphics hardware
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_VertexBuffer.size(), &(m_VertexBuffer[0]), GL_STATIC_DRAW);

		//Pass in the vertex coordinates to the vertex shader
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//
		//Texture Coordinates vbo
		//
		glGenBuffers(1, &texcoords_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, texcoords_vbo);

		// copy our points from the mesh into our VBO on graphics hardware
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * m_Tex0Buffer.size(), &(m_Tex0Buffer[0]), GL_STATIC_DRAW);

		//Pass in the vertex coordinates to the vertex shader
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		//
		//Normals vbo
		//

		glGenBuffers(1, &normals_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);

		// copy our points from the mesh into our VBO on graphics hardware
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * m_NormalBuffer.size(), &(m_NormalBuffer[0]), GL_STATIC_DRAW);

		//Pass in the vertex coordinates to the vertex shader
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);


		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	float Terrain::GetHeightAt(const glm::vec3& position)
	{
		float height = 0;
		// Check if the terrain dimensions are valid
		if (m_HeightmapWidth < 2 || m_HeightmapLength < 2) return height;

		// Width and height of the terrain in world units
		float terrainWidth = (m_HeightmapWidth - 1) * m_fBlockScale;
		float terrainHeight = (m_HeightmapLength - 1) * m_fBlockScale;
		float halfWidth = terrainWidth * 0.5f;
		float halfHeight = terrainHeight * 0.5f;

		// Multiply the position by the inverse of the terrain matrix 
		// to get the position in terrain local space
		vec3 terrainPos = glm::vec3(invM * glm::vec4(position.x, position.y, -position.z, 1.0f));
		vec3 invBlockScale(1.0f / m_fBlockScale, 0.0f, 1.0f / m_fBlockScale);

		// Calculate an offset and scale to get the vertex indices
		vec3 offset(halfWidth, 0.0f, halfHeight);
		offset = vec3(0);

		// Get the 4 vertices that make up the triangle we're over
		vec3 vertexIndices = (terrainPos + offset) * invBlockScale;

		int u0 = (int)floorf(vertexIndices.x);
		int u1 = u0 + 1;
		int v0 = (int)floorf(vertexIndices.z);
		int v1 = v0 + 1;

		if (u0 >= 0 && u1 < m_HeightmapWidth && v0 >= 0 && v1 < m_HeightmapLength)
		{
			vec3 p00 = m_PositionBuffer[(v0 * m_HeightmapWidth) + u0];    // Top-left vertex
			vec3 p10 = m_PositionBuffer[(v0 * m_HeightmapWidth) + u1];    // Top-right vertex
			vec3 p01 = m_PositionBuffer[(v1 * m_HeightmapWidth) + u0];    // Bottom-left vertex
			vec3 p11 = m_PositionBuffer[(v1 * m_HeightmapWidth) + u1];    // Bottom-right vertex

			//We want positive z values for the calculation
			p00.z = -p00.z;
			p01.z = -p01.z;
			p10.z = -p10.z;
			p11.z = -p11.z;

			// Which triangle are we over?
			float percentU = vertexIndices.x - u0;
			float percentV = 1 - (vertexIndices.z - v0);

			vec3 dU, dV;
			if (percentU > percentV)
			{   // Top triangle
				dU = p11 - p01;
				dV = p10 - p11;
			}
			else
			{   // Bottom triangle
				dU = p10 - p00;
				dV = p00 - p01;
			}

			vec3 heightPos = p01 + (dU * percentU) + (dV * percentV);
			// Convert back to world-space by multiplying by the terrain's world matrix
			heightPos = vec3(M * glm::vec4(heightPos, 1));
			height = heightPos.y;
		}
		return height;
	}

	void Terrain::Render()
	{
		glUseProgram(shader->id);
		shader->updateM(value_ptr(M));
		shader->updateColour(COLOUR_WHITE);

		glBindVertexArray(vao);

		glBindTexture(GL_TEXTURE_2D, texture_vbo);

		glDrawArrays(GL_TRIANGLES, 0, m_VertexBuffer.size());

		
	}
}