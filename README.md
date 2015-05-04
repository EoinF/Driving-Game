# Driving-Game
Generic 3d collect-the-packages driving game.
The goal of the game is to collect a certain number of packages before the time runs out

This game was developed as a project for a graphics module in TCD Integrate Computer Science. 
It is a demonstration of the low level OpenGL techniques acquired in this module, such as:
  - Writing custom shaders
  - Scene management
  - Lighting techniques
  - Texture mapping
  
The models used in the project are also custom made using Blender

#Dependencies
- GLEW
- GLFW3
- GLM 0.9.5.4 (WARNING: DOES NOT WORK ON 0.9.6.x)
- Soil VC8 build
  - Compile the soil library by opening "Simple OpenGL Image Library\projects\VC8\SOIL.sln" with visual studio and clicking build
  - Locate the generated SOIL.lib file and place that with all the other libraries

#Setting up your dev environment
1. Create a folder called libs
2. Place each include folder(contains header files), from each dependency listed above, into libs/include
3. Go to Properties > Configuration Properties > C/C++ in visual studio and put the path to libs/include in "Additional Include Directories"
4. In the same screen go to Linker > General and put the path to libs in "Additional Library Directories"
5. In the same screen go to Linker > Input and put "SOIL.lib;irrKlang.lib;glew32.lib;glfw3.lib;opengl32.lib;" into Additonal Dependencies
6. Place glfw3.dll and glew32.dll into either "Debug" or "x64/Debug" (depending on how you are building the project)

Note: The project builds in either 32 bit or 64 bit but make sure all of the relevant libraries use the same number of bits
