	
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <vector>

#include "glm/glm.hpp"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "GameObject.h"
#include "TruckEntity.h"
#include "Camera.h"
#include "SmoothCamera.h"
#include "Terrain.h"
#include "text.h"

#include "InputManager.h"
#include "StateManager.h"
#include "SoundManager.h"

using namespace std;
using namespace glm;

#define TIME_STEP_SIZE 0.02	 // 50Hz


GLFWwindow* window = NULL;
vector<Mesh> meshes;
vector<Texture> textures;
vector<Shader> shaders;
vector<GameObject> gameobjects;
TruckEntity * player;

//
// dimensions of the window drawing surface
//
int gl_width = 800;
int gl_height = 600;

Camera * drivercam;
Camera * followcam;
SmoothCamera * smoothcam;
int cam_id = 0;
const int NUM_CAMS = 3;

mat4 P = perspective(90.0f, (float)gl_width / (float)gl_height, 0.01f, 100.0f);

double prev_time = 0;
double accum_sim_time = 0;

void Update();
void Draw();
void LoadContent();
static void MouseCallback(GLFWwindow * window, double x, double y);
void printErrorLog(char *title, GLuint shader);
bool parse_file_into_str (const char* file_name, char* shader_str, int max_len);

int main () {
	const GLubyte* renderer;
	const GLubyte* version;

	//
	// Start OpenGL using helper libraries
	// --------------------------------------------------------------------------
	if (!glfwInit ()) {
		fprintf (stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//window = glfwCreateWindow(gl_width, gl_height, "Ballinya Transport", glfwGetPrimaryMonitor(), NULL); //fullscreen
	window = glfwCreateWindow(gl_width, gl_height, "Ballinya Transport", NULL, NULL);
	

	if (!window) {
		fprintf (stderr, "ERROR: opening OS window\n");
		return 1;
	}
	glfwMakeContextCurrent (window);

	glewExperimental = GL_TRUE;
	glewInit ();

	/* get version info */
	renderer = glGetString (GL_RENDERER); /* get renderer string */
	version = glGetString (GL_VERSION); /* version as a string */
	printf ("Renderer: %s\n", renderer);
	printf ("OpenGL version supported %s\n", version);
	
	LoadContent();
	
	player = new TruckEntity(&meshes[1], &meshes[2], &textures[0], &shaders[0], 0.2, vec3(8, 1, -12), 1, 1.2, 2.3);

	gameobjects.push_back(GameObject(&meshes[0], &textures[0], &shaders[0], 0.2, vec3(5, 1, -10), 0.1, 0.1, 0.1, vec4(1, 1, 1, 0.05)));
	gameobjects.push_back(GameObject(&meshes[0], &textures[0], &shaders[0], 0.2, vec3(8, 0.3, -1), 0.1, 0.1, 0.1));


	glfwSetCursorPosCallback(window, MouseCallback);
	
	shaders[0].SetDirectionalLight(DirectionalLight(vec3(1, 1, 1), vec3(0.5, -0.5, -0.3), 0.8));
	shaders[2].SetDirectionalLight(DirectionalLight(vec3(1, 1, 1), vec3(0.5, -0.5, -0.3), 0.8));
	
	drivercam = new Camera(&(player->getPos() + vec3(1,1,-1)), &(player->getPos()), &vec3(0, 1, 0), gl_width, gl_height, window, player, vec3(-0.1, 0.2, 0.1));
	followcam = new Camera(&(player->getPos() + vec3(1,1,-1)), &(player->getPos()), &vec3(0, 1, 0), gl_width, gl_height, window, player, vec3(0, 1, -0.8));
	smoothcam = new SmoothCamera(&(player->getPos() + vec3(1,1,-1)), &(player->getPos()), &vec3(0, 1, 0), gl_width, gl_height, window);

	GameObject arrow = GameObject(&meshes[3], NULL, &shaders[1], 0.1, vec3(0, 0.8, 0), 0, 0, 0, vec4(1, 0.7, 0, 1));
	GameObject package = GameObject(&meshes[4], NULL, &shaders[2], 0.2, vec3(0, 0, 0), 1, 1, 2, vec4(0.9, 0, 1, 1));
	state::init(&arrow, &package);
	sound::init();
	sound::playSound("Nitro Fun - New Game.mp3", true);

	//
	// Start rendering
	// --------------------------------------------------------------------------
	// tell GL to only draw onto a pixel if the fragment is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glDisable(GL_CULL_FACE);
	glClearColor (0.1, 0.1, 0.1, 1.0);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	while (!glfwWindowShouldClose(window)) {
		Update();
		Draw();
		glfwPollEvents();
		glfwSetCursorPos (window, gl_width / 2, gl_height / 2);
		glfwSwapBuffers(window);
	}

	Terrain::delete_Terrain();
	return 0;
}

void Update()
{
	// work out how much time has passed
	double curr_time = glfwGetTime();
	double elapsed = curr_time - prev_time;
	prev_time = curr_time;
	accum_sim_time += elapsed;

	while (accum_sim_time > TIME_STEP_SIZE) {
		accum_sim_time -= TIME_STEP_SIZE;

		input::CheckInput(window);

		if (input::previous.C && !input::current.C) //Key was just pressed
		{
			cam_id++;
			cam_id %= NUM_CAMS;
		}

		player->Update(TIME_STEP_SIZE);
		followcam->Update();
		drivercam->Update();
		smoothcam->Update();

		vec3 camDir;
		switch (cam_id)
		{
		case 0:
			camDir = drivercam->getDirection();
			break;
		case 1:
			camDir = followcam->getDirection();
			break;
		case 2:
			camDir = smoothcam->getDirection();
			break;
		}
		state::update(player->getPos(), camDir, player->getVel());
	}

}

void Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gl_width, gl_height);

	mat4 V;
	
	switch (cam_id)
	{
	case 0:
		V = drivercam->getViewMatrix();
		break;
	case 1:
		V = followcam->getViewMatrix();
		break;
	case 2:
		V = smoothcam->getViewMatrix();
		break;
	}

	for (int e = 0; e < gameobjects.size(); e++)
	{
		gameobjects[e].draw(V, P);
	}

	player->draw(V, P);
	Terrain::Render();

	state::drawObjects(V, P);
}

void LoadContent()
{
	meshes.push_back(Mesh("urbanbuilding.obj"));
	meshes.push_back(Mesh("truck.obj"));
	meshes.push_back(Mesh("truckwheels.obj"));

	meshes.push_back(Mesh("arrow.obj"));
	meshes.push_back(Mesh("package.obj"));


	shaders.push_back(Shader("diffuse"));
	shaders.push_back(Shader("simple"));
	shaders.push_back(Shader("diffuse_notexture"));
	textures.push_back(Texture("urbanbuilding.png"));
	GameObject::BoundingCube = Mesh("simplecube.obj"); //Mesh shared between all game objects
	GameObject::BoundingColour = vec4(0.8, 0, 1, 0.5); //Colour of bounding box

	Terrain::create_Terrain(&shaders[0], "test_256x256.bmp", "grass-texture-2.jpg");
	
	init_text_rendering("freemono.png", "freemono.meta.txt", gl_width, gl_height);
}

static void MouseCallback(GLFWwindow * window, double x, double y)
{
	drivercam->OnMouse(x, y);
	followcam->OnMouse(x, y);
	if (cam_id == 2)
		smoothcam->OnMouse(x, y);
}