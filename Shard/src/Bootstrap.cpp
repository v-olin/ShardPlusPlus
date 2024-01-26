#include "Bootstrap.h"
#include "Logger.h"
#include "BaseFunctionality.h"
#include "GameObjectManager.h"

#include <chrono>
#include <filesystem>
#include <unordered_map>

namespace Shard {

    bool Bootstrap::checkEnvironmentVariable(std::string id) {
        return en_vars.find(id) != en_vars.end();
    }

    std::string Bootstrap::getEnvironmentVariable(std::string id) {
        if (en_vars.find(id) != en_vars.end())
            return en_vars[id];

        return "";
    }

    double Bootstrap::timeElapsed() {
        return time_elapsed;
    }
    
    double Bootstrap::getDeltaTime() {
        return delta_time;
    }

    long Bootstrap::getCurrentMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    float Bootstrap::getFPS() {
        double seconds = (getCurrentMillis() - start_time) / 1000.f;
        return frames / seconds;
    }

    float Bootstrap::getSecondFPS() {
        long now = getCurrentMillis();

        Logger::log("Current frametime: " + frame_times.size(), LOG_LEVEL_ALL);

        if (frame_times.empty()) {
            return -1;
        }

        auto riter = frame_times.rbegin();
        size_t frame_count = 0;

        while (*(++riter) > (now - 1000) && riter != frame_times.rend()) {
            frame_count++;
        }

        if (riter != frame_times.rend()) {
            auto start = frame_times.begin();
            auto end = frame_times.begin();
            std::advance(end, frame_times.size() - frame_count);

            frame_times.erase(start, end);
        }

        return frame_count;
    }

    int Bootstrap::getCurrentFrame() {
        return frames;
    }

    std::string Bootstrap::getBaseDir() {
        return base_dir;
    }

    Display* Bootstrap::getDisplay() {
        return &display_engine;
    }

    Sound* Bootstrap::getSound() {
        return &sound_engine;
    }

    InputManager* Bootstrap::getInput() {
        return &input;
    }

    AssetManager* Bootstrap::getAssetManager() {
        return &asset;
    }

    Game* Bootstrap::getRunningGame() {
        return &running_game;
    }

    void Bootstrap::setup() {
        std::string work_dir = std::filesystem::current_path().string();
        // GAME EXE SHOULD BE IN TOPMOST FOLDER!!!!!
        base_dir = work_dir;

        setupEnvironmentVariables(base_dir + "\\" + "envar.cfg");
        std::string config_path = base_dir + "\\" + DEFAULT_CONFIG;

        Logger::log("Current config path: " + config_path);

        auto config = BaseFunctionality::getInstance().readConfigFile(config_path);
        bool bailOut = false;

        bool display_engine_initialized = false;
        bool running_game_initialized = false;
        bool sound_engine_initialized = false;

        // TODO: determine logic and structure of config file
        //       and implement here
        for (const auto& [class_name, formal_className] : config) {
            if (class_name.compare("display") == 0){
                display_engine.initialize();
                display_engine_initialized = true;
            }
            else if (class_name.compare("asset") == 0)
                asset.registerAssets();
            else if (class_name.compare("game") == 0){
                target_frame_rate = running_game.getTargetFrameRate();
                millis_per_frame = 1000 / target_frame_rate;
                running_game_initialized = true;
            }
            else if (class_name.compare("input") == 0)
                input.initialize();
        }

        if (!display_engine_initialized) {
            Logger::log("display not initialized", LoggerLevel::LOG_LEVEL_ERROR);
            bailOut = true;
        }

        if (!running_game_initialized) {
            Logger::log("runningame not initialized", LoggerLevel::LOG_LEVEL_ERROR);
            bailOut = true;
        }

        if (!sound_engine_initialized) {
            Logger::log("sound not initialized", LoggerLevel::LOG_LEVEL_ERROR);
            bailOut = true;
        }

        // Missing key component(s), GET TO THE CHOPPA!!!
        if (bailOut)
            std::exit(0);

    }

    void Bootstrap::setupEnvironmentVariables(std::string path) {
        Logger::log("Path is" + path);

        std::unordered_map<std::string, std::string> config = BaseFunctionality::getInstance().readConfigFile(path);

        for (const auto&[key, value] : config)
            en_vars[key] = value;
    }



    void Bootstrap::Main(std::string args[]){
		long time_in_milliseconds_start, last_tick, time_in_milliseconds_end;
		long interval;
		int sleep;
		int tfro = 1;
		bool phys_update = false;
		bool phys_debug = false;

        //setup the engine
        setup();
        start_time = getCurrentMillis();
        frames = 0;

        running_game.initalize();

        time_in_milliseconds_start = start_time;
        last_tick = start_time;

        phys.gravity_modifier = 0.1f;

        phys_debug = getEnvironmentVariable("phusics_debug") == "1";

        while (true) {
            frames += 1;
            time_in_milliseconds_start = getCurrentMillis();
            getDisplay()->clearDisplay();
            running_game.update();

            if(running_game.isRunning()){


				// Get input, which works at 50 FPS to make sure it doesn't interfere with the 
				// variable frame rates.
				input.getInput();

				// Update runs as fast as the system lets it.  Any kind of movement or counter 
				// increment should be based then on the deltaTime variable.
				GameObjectManager::getInstance().update();

				// This will update every 20 milliseconds or thereabouts.  Our physics system aims 
				// at a 50 FPS cycle.
				if (phys.willTick())
				{
					GameObjectManager::getInstance().prePhysicsUpdate();
				}

				// Update the physics.  If it's too soon, it'll return false.   Otherwise 
				// it'll return true.
				phys_update = phys.update();

				if (phys_update)
				{
					// If it did tick, give every object an update
					// that is pinned to the timing of the physics system.
					GameObjectManager::getInstance().physicsUpdate();
				}

				if (phys_debug) {
					phys.drawDebugColliders();
				}
            }
        }
        
    }
}