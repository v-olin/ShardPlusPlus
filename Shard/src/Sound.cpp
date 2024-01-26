#include "Sound.h"

#include "AssetManager.h"
#include "Logger.h"

#include <SDL_audio.h>

#define SYSTEM_DEFAULT_SPEAKER 0

namespace Shard {
	void Sound::playSound(std::string file_name) {
        SDL_AudioSpec have, want;
        unsigned int length;
        unsigned char* wav_buffer;

        AssetManager& manager = AssetManager::getInstance();
        std::string file_path = manager.getAssetPath(file_name.c_str());

        SDL_LoadWAV(file_path.c_str(), &have, &wav_buffer, &length);
        SDL_AudioDeviceID dev_id = SDL_OpenAudioDevice(SYSTEM_DEFAULT_SPEAKER, 0, &have, &want, 0);

        int error = SDL_QueueAudio(dev_id, wav_buffer, length);
        if (error) {
            std::string sdl_error{ SDL_GetError() };
            std::string msg{ "Could not play sound: " };
            Logger::log(msg + sdl_error, LOG_LEVEL_ALL);
        }

        SDL_PauseAudioDevice(dev_id, 0);
	}
}