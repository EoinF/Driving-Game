#pragma once

#include "GameObject.h"
#include "Terrain.h"
#include <time.h>
#include "text.h"

namespace state
{ 
	enum GameState
	{
		InGame,
		GameLost,
		GameWon
	};

	extern int timeLeft;
	extern int score;

	extern GameState state;

	void init(GameObject * _arrow, GameObject * _package);
	void update(vec3 playerPos, vec3 camDir, vec3 playerVel);
	void drawObjects(mat4 V, mat4 P);
}