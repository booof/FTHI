// Entry Point to Engine

#include "Source/Rendering/Initialize.h"
#include "Source/Loops/Loop.h"
#include "Source/Rendering/Exit.h"

#define DR_WAV_IMPLEMENTATION
#include <DRLIB/DRLIB/dr_wav.h>
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