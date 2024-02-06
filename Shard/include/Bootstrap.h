// Bootstrap.h - Description
#pragma once

#include "Game.h"
#include "AssetManager.h"
#include "GameObject.h"
#include "Display.h"
#include "Sound.h"
#include "InputManager.h"
#include "PhysicsManager.h"

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

namespace Shard {

	class Bootstrap {

	public:
		inline static  const std::string DEFAULT_CONFIG = "config.cfg";
		static bool checkEnvironmentVariable(std::string id);
		static std::string getEnvironmentVariable(std::string id);
		static double timeElapsed();
		static double getDeltaTime();
		static long getCurrentMillis();
		static float getFPS();
		static float getSecondFPS();
		static int getCurrentFrame();
		static void Main(std::string[]);

		static const std::string getBaseDir();
		
		static std::shared_ptr<Game> getRunningGame();
		static Display* getDisplay();
		static Sound& getSound();
		static InputManager& getInput();
		static AssetManager& getAssetManager();
		static void setRunningGame(std::shared_ptr<Game> game);

		static void setup();
		static void setupEnvironmentVariables(std::string path);


	private:
		inline static bool running_game_set;
		inline static std::shared_ptr<Game> running_game;
		inline static Display* display_engine;
		inline static Sound sound_engine;
		inline static InputManager input;
		inline static PhysicsManager& phys{ PhysicsManager::getInstance() };
		inline static AssetManager& asset{ AssetManager::getInstance() };
		inline static int target_frame_rate;
        inline static int millis_per_frame;
        inline static double delta_time;
        inline static double time_elapsed;
        inline static int frames;
        inline static std::vector<long> frame_times;
        inline static long start_time;
        inline static std::string base_dir;
		inline static std::unordered_map<std::string, std::string> en_vars;
	};
}