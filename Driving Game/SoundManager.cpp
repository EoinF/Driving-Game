#include "SoundManager.h"

namespace sound
{
	irrklang::ISoundEngine* engine;

	void init()
	{
		//engine = irrklang::createIrrKlangDevice();
	}

	void end()
	{
		//engine->drop();
	}
	
	void playSound(string filename, bool isLooped)
	{
		//engine->play2D(filename.c_str(), isLooped);
	}

	void stopSounds()
	{
		//engine->stopAllSounds();
	}
}