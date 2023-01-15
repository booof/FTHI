// Entry Point to Engine

#include "Source/Rendering/Initialize.h"
#include "Source/Loops/Loop.h"
#include "Source/Rendering/Exit.h"

#define DR_WAV_IMPLEMENTATION
#include <DRLIB/DRLIB/dr_wav.h>
//#include <dr_mp3.h>

// To Do:
// Make Alternate Function Keys for Shift
// Ex: F1 will build in debug mode with the .pdb file loaded, calls DebugVisualStudio
// Ex: F5 will regenerate project instead of simply recompiling it

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