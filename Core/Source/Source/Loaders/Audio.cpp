#include "Audio.h"

#include "Globals.h"
#include <DRLIB/DRLIB/dr_wav.h>

void Source::Audio::getCurrentAudioDevice()
{
	// Get the Device
	Global::audio_device = alcOpenDevice(nullptr);

	// If Current Device is Unavaliable, Throw Error
	if (Global::audio_device == nullptr)
		throw "ERROR";
}

void Source::Audio::loadAudioFile(ALuint buffer, std::string path)
{
    // Variables to Hold PCM
    unsigned int channels;
    unsigned int sampleRate;
    drwav_uint64 totalPCMFrameCount;

    // Read Audio File
    short* file_data = drwav_open_file_and_read_pcm_frames_s16(path.c_str(), &channels, &sampleRate, &totalPCMFrameCount, NULL);
    if (file_data == NULL)
        throw "INVALID_AUDIO_FILE_PATH";

    // Determine the Format of the File
    ALenum format = AL_FORMAT_STEREO16;
    if (channels == 1)
        format = AL_FORMAT_MONO16;

    // Store Audio File in Buffer
    alBufferData(buffer, format, file_data, (ALsizei)(totalPCMFrameCount * channels * sizeof(short)), sampleRate);

    // Free Audio Data
    drwav_free(file_data, NULL);
}
