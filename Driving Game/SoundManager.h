#pragma once

#include <irrklang\irrKlang.h>
#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

namespace sound
{
	void init();
	void end();
	void playSound(string filename, bool isLooped);

}