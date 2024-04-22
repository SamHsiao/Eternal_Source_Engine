#include "AudioManager.h"

void AudioManager::SetVolume(int channel, int volume) {
	AudioHelper::Mix_Volume498(channel, volume);
}

void AudioManager::LoadAudio() {
	/* Load audio files in resources/audio */
	const std::string audioDirectoryPath = "resources/audio";

	// Fills up loadedScenes if the path exists
	if (std::filesystem::exists(audioDirectoryPath)) {
		for (const auto& audioFile : std::filesystem::directory_iterator(audioDirectoryPath)) {
			if (audioFile.path() != audioDirectoryPath + "/.DS_Store")
			{
				if (audios.find(audioFile.path().stem().stem().string() + audioFile.path().extension().string()) == audios.end()) {
					std::string audiofilepath = audioFile.path().string();
					AudioHelper::Mix_OpenAudio498(44100, AUDIO_S16SYS, 2, 1024);
					Mix_Chunk* mixchunk = AudioHelper::Mix_LoadWAV498(audiofilepath.c_str());
					audios[audioFile.path().stem().stem().string() + audioFile.path().extension().string()] = mixchunk;
				}
			}
		}
	}
}

void AudioManager::PlayAudio(int channel, const std::string& audiofilename, bool does_loop) {
	if (does_loop) {
		AudioHelper::Mix_PlayChannel498(channel, audios[audiofilename], -1);
	}
	else {
		AudioHelper::Mix_PlayChannel498(channel, audios[audiofilename], 0);
	}
}

void AudioManager::StopAudio(int channel) {
	AudioHelper::Mix_HaltChannel498(channel);
}

