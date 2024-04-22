#pragma once

#include <unordered_map>
#include <string>

#include "Helper.h"
#include "AudioHelper.h"

class AudioManager
{
	static inline std::unordered_map<std::string, Mix_Chunk*> audios;

public:

	static void LoadAudio();

	static void SetVolume(int channel, int volume);

	static void PlayAudio(int channel, const std::string& audiofilename, bool does_loop);

	static void StopAudio(int channel);
};

