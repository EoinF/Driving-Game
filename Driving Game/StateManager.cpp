#include "StateManager.h"

#define SCORE_LIMIT 2
#define START_TIME 60
#define PACKAGE_ROTATE_SPEED 0.5
#define PACKAGE_SPAWN_HEIGHT_LIMIT 4

namespace state
{
	int timeLeft;
	int score;

	GameObject * arrow;
	GameObject * package;
	GameState state;
	
	int time_text;
	int win_text;
	int lose_text;

	int speed_text;

	void CreatePackage()
	{
		float x = 0;
		float y = 8;
		float z = 0;

		while (y >= PACKAGE_SPAWN_HEIGHT_LIMIT)
		{
			int r1 = rand();
			int r2 = rand();

			x = r1 % (int)Terrain::m_HeightmapLength;
			z = r2 % (int)Terrain::m_HeightmapWidth;
			y = Terrain::GetHeightAt(vec3(x, 0, -z));
		}

		package->setPos(vec3(x, y + 0.1, -z));
	}

	void init(GameObject * _arrow, GameObject * _package)
	{
		srand(time(NULL));
		score = 0;
		timeLeft = START_TIME * 60;
		arrow = _arrow;
		package = _package;

		time_text = add_text (
		"0",
		0.8f,
		1.0f,
		110.0f,
		0.9f,
		0.9f,
		0.0f,
		0.8f);
		
		win_text = add_text (
		"",
		-0.3f,
		0.2f,
		110.0f,
		0.0f,
		0.9f,
		0.0f,
		0.8f);
		
		lose_text = add_text (
		"",
		-0.3f,
		0.2f,
		110.0f,
		0.9f,
		0.0f,
		0.0f,
		0.8f);

		speed_text = add_text (
		"0 kph",
		0.6f,
		-0.8f,
		90.0f,
		0.7f,
		0.3f,
		0.9f,
		0.8f);

		CreatePackage();
	}

	void m_InGame(vec3 playerPos, vec3 camDir, vec3 playerVel)
	{
		timeLeft--;
		if (timeLeft > 0)
		{
			char tmp[32];
			if (timeLeft % 60 == 0)
			{
				sprintf (tmp, "%d", timeLeft / 60);
				update_text (time_text, tmp);
			}

			vec3 packagepos = package->getPos();
			//Use the dot and cross product to make the arrow point to the package
			vec3 towardPackage = normalize(vec3(packagepos.x - playerPos.x, 0, packagepos.z - playerPos.z));
			//Ignore height value when calculating direction of arrow
			camDir.y = 0;
			camDir = normalize(camDir);

			vec3 crossprod = cross(towardPackage, camDir);
			float dotprod = dot(towardPackage, camDir);

			//y is negative when angle is greater than 180 degrees
			int sign = -glm::sign(crossprod.y);

			float zRot = 90 + sign * degrees(acos(dotprod));
			arrow->setRot(vec3(90, 0, zRot));


			package->rotate(vec3(0, PACKAGE_ROTATE_SPEED, 0));
			package->colour = vec4(glm::sin(timeLeft / 30.0f) + 0.2f, glm::cos((float)(timeLeft % 5)) / 10, glm::cos(1 + timeLeft / 30.0f) + 0.3f, 1);

			if (distance2(playerPos, package->getPos()) < 1)
			{
				score++;
				if(score == SCORE_LIMIT)
				{
					state = GameWon;
				}
				else
				{
					CreatePackage();
					timeLeft += (int)distance2(playerPos, package->getPos()) / 20;
				}
			}

			
			sprintf (tmp, "%d kph", (int)(400 * glm::length(vec3(playerVel.x, 0, playerVel.z))));
			update_text (speed_text, tmp);
		}
		else
		{
			state = GameLost;
		}
	}
	
	void m_GameWon()
	{
		update_text (win_text, "You are win!");
	}
	void m_GameLost()
	{
		update_text (lose_text, "You are lose!");
	}

	void update(vec3 playerPos, vec3 camDir, vec3 playerVel)
	{
		switch (state)
		{
		case InGame:
			m_InGame(playerPos, camDir, playerVel);
			break;
		case GameWon:
			m_GameWon();
			break;
		case GameLost:
			m_GameLost();
			break;
		}
	}

	void drawObjects(mat4 V, mat4 P)
	{
		if (state == InGame)
		{
			arrow->draw();
			package->draw(V, P);
		}

		draw_texts();
	}


}