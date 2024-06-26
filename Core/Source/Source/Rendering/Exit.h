#pragma once
#ifndef EXIT_H
#define EXIT_H

namespace Source::Rendering::Exit
{
	// Main Function to Uninitialize the Program
	void exitProgram();

	// Determines if Safe to Exit
	bool determineSafeToExit();
}

#endif
