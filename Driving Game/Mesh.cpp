#include "Mesh.h"


Mesh::Mesh()
{

}
Mesh::Mesh(string filename)
{
	GLfloat* vp = NULL; // array of vertex points
	GLfloat* vn = NULL; // array of vertex normals
	GLfloat* vt = NULL; // array of texture coordinates

	printf("Loading mesh: %s\n", filename.c_str());
	vertex_count = 0;
	assert (load_obj_file (filename.c_str(), vp, vt, vn, vertex_count, uv_count));
	normals_count = vertex_count;
	

	printf("points: %d\n", vertex_count);
	printf("uvs: %d\n", uv_count);
		
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
		
	GLuint points_vbo;
	GLuint texcoords_vbo;
	GLuint normals_vbo;

	//
	//Points vbo
	//
	glGenBuffers (1, &points_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
		
	// copy our points from the mesh into our VBO on graphics hardware
	glBufferData (GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(GLfloat), vp, GL_STATIC_DRAW);
		
	//Pass in the vertex coordinates
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
	if (uv_count > 0)
	{
		//
		//Texture Coordinates vbo
		//
		glGenBuffers (1, &texcoords_vbo);
		glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
		
		// copy our points from the mesh into our VBO on graphics hardware
		glBufferData (GL_ARRAY_BUFFER, uv_count * 2 * sizeof(GLfloat), vt, GL_STATIC_DRAW);
		
		//Pass in the vertex coordinates
		glEnableVertexAttribArray (1);
		glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	}
	//
	//Normals vbo
	//
	
	glGenBuffers (1, &normals_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
		
	// copy our points from the mesh into our VBO on graphics hardware
	glBufferData (GL_ARRAY_BUFFER, normals_count * 3 * sizeof(GLfloat), vn, GL_STATIC_DRAW);
		
	//Pass in the vertex coordinates
	glEnableVertexAttribArray (2);
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		

	free (vp);
	free (vn);
	free (vt);
}

Mesh::Mesh(GLfloat * vp, GLfloat * vt, GLfloat * vn, int vertex_count)
{

	this->vertex_count = vertex_count;
	uv_count = vertex_count * 3/2;
	normals_count = vertex_count;
		
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);
		
	GLuint points_vbo;
	GLuint texcoords_vbo;
	GLuint normals_vbo;

	//
	//Points vbo
	//
	glGenBuffers (1, &points_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, points_vbo);
		
	// copy our points from the mesh into our VBO on graphics hardware
	glBufferData (GL_ARRAY_BUFFER, vertex_count * 3 * sizeof(GLfloat), vp, GL_STATIC_DRAW);
		
	//Pass in the vertex coordinates to the vertex shader
	glEnableVertexAttribArray (0);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		
	//
	//Texture Coordinates vbo
	//
	glGenBuffers (1, &texcoords_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, texcoords_vbo);
		
	// copy our points from the mesh into our VBO on graphics hardware
	glBufferData (GL_ARRAY_BUFFER, uv_count * 2 * sizeof(GLfloat), vt, GL_STATIC_DRAW);
		
	//Pass in the vertex coordinates to the vertex shader
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//
	//Normals vbo
	//
	
	glGenBuffers (1, &normals_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, normals_vbo);
		
	// copy our points from the mesh into our VBO on graphics hardware
	glBufferData (GL_ARRAY_BUFFER, normals_count * 3 * sizeof(GLfloat), vn, GL_STATIC_DRAW);
		
	//Pass in the vertex coordinates to the vertex shader
	glEnableVertexAttribArray (2);
	glVertexAttribPointer (2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		

	free (vp);
	free (vn);
	free (vt);
}

Mesh::~Mesh()
{
}
