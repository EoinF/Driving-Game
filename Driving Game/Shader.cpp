#include "Shader.h"

Shader::Shader(string shader_name)
{
	char* vertex_shader_str;
	char* fragment_shader_str;
		
	// allocate some memory to store shader strings
	vertex_shader_str = (char*)malloc (81920);
	fragment_shader_str = (char*)malloc (81920);

	// load shader strings from text files
	assert (parse_file_into_str ((shader_name + ".vert").c_str(), vertex_shader_str, 81920));
	assert (parse_file_into_str ((shader_name + ".frag").c_str(), fragment_shader_str, 81920));
	GLuint vs, fs;
	vs = glCreateShader (GL_VERTEX_SHADER);
	fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (vs, 1, (const char**)&vertex_shader_str, NULL);
	glShaderSource (fs, 1, (const char**)&fragment_shader_str, NULL);

	// free memory
	free (vertex_shader_str);
	free (fragment_shader_str);
		
	glCompileShader (vs);
	glCompileShader (fs);

	this->id = glCreateProgram ();
	glAttachShader (this->id, fs);
	glAttachShader (this->id, vs);
	glLinkProgram (this->id);

	printErrorLog("Vertex Shader: ", vs);
	printErrorLog("Fragment Shader: ", fs);

		
	M_loc = glGetUniformLocation (this->id, "M");
	V_loc = glGetUniformLocation (this->id, "V");
	P_loc = glGetUniformLocation (this->id, "P");
	tex_loc = glGetUniformLocation(this->id, "basic_texture");
	colour_loc = glGetUniformLocation(this->id, "colour");
	m_dirLightLocation.Colour = glGetUniformLocation(this->id, "gDirectionalLight.Colour");
	m_dirLightLocation.Direction = glGetUniformLocation(this->id, "gDirectionalLight.Direction");
	m_dirLightLocation.DiffuseIntensity = glGetUniformLocation(this->id, "gDirectionalLight.DiffuseIntensity");

	glUseProgram (this->id);
}


Shader::~Shader()
{
}

void Shader::updateMVP(float * M, float * V, float * P)
{
	glUseProgram(this->id);
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, M);
	glUniformMatrix4fv (V_loc, 1, GL_FALSE, V);
	glUniformMatrix4fv (P_loc, 1, GL_FALSE, P);
}

void Shader::updateM(float * M)
{
	glUseProgram(this->id);
	glUniformMatrix4fv (M_loc, 1, GL_FALSE, M);
}

void Shader::updateColour(float * colour)
{
	glUseProgram(this->id);
	glUniform4fv (colour_loc, 1, colour);
}


void Shader::SetDirectionalLight(const DirectionalLight& Light)
{
	glUseProgram(this->id);
    glUniform3f(m_dirLightLocation.Colour, Light.Colour.x, Light.Colour.y, Light.Colour.z);
    //glUniform1f(m_dirLightLocation.AmbientIntensity, Light.AmbientIntensity);
    vec3 Direction = normalize(Light.Direction);

    glUniform3f(m_dirLightLocation.Direction, Direction.x, Direction.y, Direction.z);
    glUniform1f(m_dirLightLocation.DiffuseIntensity, Light.DiffuseIntensity);
}



//
// copy a shader from a plain text file into a character array
bool parse_file_into_str (
	const char* file_name, char* shader_str, int max_len) {

	FILE* file = fopen (file_name , "r");
	int current_len = 0;
	char line[2048];

	shader_str[0] = '\0'; /* reset string */
	if (!file) {
		fprintf (stderr, "ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	strcpy (line, ""); /* remember to clean up before using for first time! */
	while (!feof (file)) {
		if (NULL != fgets (line, 2048, file)) {
			current_len += strlen (line); /* +1 for \n at end */
			if (current_len >= max_len) {
				fprintf (stderr, 
					"ERROR: shader length is longer than string buffer length %i\n",
					max_len
				);
			}
			strcat (shader_str, line);
		}
	}
	if (EOF == fclose (file)) { /* probably unnecesssary validation */
		fprintf (stderr, "ERROR: closing file from reading %s\n", file_name);
		return false;
	}
	return true;
}


void printErrorLog(char *title,GLuint shader)
{
	GLint status;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
	if(status==GL_FALSE)
	{
		char buffer[512]; 
		glGetShaderInfoLog(shader,512,NULL,buffer); 
		printf("%s:\n %s\n",title,buffer);
	}
}