#pragma once
#ifndef AUDIO_H
#define AUDIO_H

#include "ExternalLibs.h"
#include "Globals.h"

namespace Source::Audio
{
	// Return the Current Audio Device
	void getCurrentAudioDevice();

	// Load Audio File
	void loadAudioFile(ALuint buffer, std::string path);
}

#endif
