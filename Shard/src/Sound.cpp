#include "Sound.h"

#include "AssetManager.h"
#include "Logger.h"

#include <SDL_audio.h>

namespace Shard {
	void Sound::playSound(std::string file_name) {
        SDL_AudioSpec have, want;
        unsigned int length;
        unsigned char* wav_buffer;

        AssetManager& manager = AssetManager::getInstance();
        std::string file_path = manager.getAssetPath(file_name);

        SDL_LoadWAV(file_path.c_str(), &have, &wav_buffer, &length);
        const char* audio_device = SDL_GetAudioDeviceName(0, 0);
        SDL_AudioDeviceID dev_id = SDL_OpenAudioDevice(audio_device, 0, &have, &want, 0);

        int success = SDL_QueueAudio(dev_id, wav_buffer, length);
        if (success != 0) {
            std::string msg = "Couldn't queue audio: " + file_name;
            Logger::log(msg.c_str(), LOG_LEVEL_ERROR);
        }
        else
            SDL_PauseAudioDevice(dev_id, 0);
	}
}