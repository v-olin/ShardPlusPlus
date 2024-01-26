// Bootstrap.h - Description
#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include "Game.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "Display.h"
#include "Sound.h"
#include "InputManager.h"
#include "PhysicsManager.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Shard {

	class Bootstrap {

	public:
		inline static SHARD_API const std::string DEFAULT_CONFIG = "config.cfg";
		static SHARD_API bool checkEnvironmentVariable(std::string id);
		static SHARD_API std::string getEnvironmentVariable(std::string id);
		static SHARD_API double timeElapsed();
		static SHARD_API double getDeltaTime();
		static SHARD_API long getCurrentMillis();
		static SHARD_API float getFPS();
		static SHARD_API float getSecondFPS();
		static SHARD_API int getCurrentFrame();
		static SHARD_API void Main(std::string[]);

		static std::string getBaseDir();
		
		static SHARD_API Display* getDisplay();
		static SHARD_API Sound* getSound();
		static SHARD_API InputManager* getInput();
		static SHARD_API AssetManager* getAssetManager();
		static SHARD_API Game* getRunningGame();

		static SHARD_API void setup();
		static SHARD_API void setupEnvironmentVariables(std::string path);


	private:
		static SHARD_API void setup(std::string config);

		static Game running_game;
		static Display display_engine;
		static Sound sound_engine;
		static InputManager input;
		static PhysicsManager phys;
		static AssetManager asset;

		static int target_frame_rate;
        static int millis_per_frame;
        static double delta_time;
        static double time_elapsed;
        static int frames;
        static std::vector<long> frame_times;
        static long start_time;
        static std::string base_dir;
		static std::unordered_map<std::string, std::string> en_vars;


	};

}