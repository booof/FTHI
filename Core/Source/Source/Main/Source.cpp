// Entry Point to Engine

#define SHOW_CONSOLE

#ifdef SHOW_CONSOLE
#pragma comment(linker, "/SUBSYSTEM:console /ENTRY:mainCRTStartup")
#else
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include "Source/Rendering/Initialize.h"
#include "Source/Loops/Loop.h"
#include "Source/Rendering/Exit.h"

#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>
//#include <dr_mp3.h>

int main()
{
	// Initialize Program
	Source::Render::Initialize::initialize();
	
	// Execute Main Loop
	Loop::loop();

	// End Program
	Source::Render::Exit::exitProgram();

	return 0;
}