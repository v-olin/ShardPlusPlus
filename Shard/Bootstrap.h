// Bootstrap.h - Description
#pragma once

#include <string>

namespace Shard {

	class Bootstrap {

	public:
		
		static const std::string DEFAULT_CONFIG;

		static bool checkEnvironmentVariable(std::string id);
		std::string getEnvironmentVariable(std::string id);

		static double timeElapsed();
		static double getDeltaTime();
		static long getCurrentMillis();
		static int getFPS();
		static int getSecondFPS();
		static int getCurrentFrame();
		static void main(std::string[]);

		static std::string getBaseDir();
		
		// TODO: Uncomment once modules areavailable
		/*
		static Display getDisplay();
		static Sound getSound();
		static InputSystem getInput();
		static AssetManager getAssetManager();
		static Game getRunningGame();
		*/

		static void setup();
		static void setupEnvironmentVariables(std::string path);


	private:

		// TODO: Uncomment when the modules are available
		/*
		static Game runningGame;
		static Display displayEngine;
		static Sound soundEngine;
		static InputSystem input;
		static PhysicsManager phys;
		static AssetManagerBase asset;
		*/

	};

}