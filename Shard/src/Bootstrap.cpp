#include "common.h"

#include "SceneManager.h"

#include "Bootstrap.h"
#include "Logger.h"
#include "BaseFunctionality.h"
#include "GameObjectManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "imgui.h"

#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <thread>

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

    long long Bootstrap::getCurrentMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }

    float Bootstrap::getFPS() {
        double seconds = (getCurrentMillis() - start_time) / 1000.f;
        return frames / seconds;
    }

    float Bootstrap::getSecondFPS() {
        long long now = getCurrentMillis();

        //Logger::log(("Current frametime: " + std::to_string(frame_times.size())).c_str());


        if (frame_times.empty()) {
            return -1;
        }

        auto riter = frame_times.rbegin();
        size_t frame_count = 0;

        while (riter != frame_times.rend() && *(riter++) > (now - 1000) ) {
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

    const std::string Bootstrap::getBaseDir() {
        return base_dir;
    }

    InputManager& Bootstrap::getInput() {
        return input;
    }

    AssetManager& Bootstrap::getAssetManager() {
        return asset;
    }

    std::shared_ptr<Game> Bootstrap::getRunningGame() {
        return running_game;
    }

    void Bootstrap::setRunningGame(std::shared_ptr<Game> game) {
       running_game = game;
       target_frame_rate = running_game->getTargetFrameRate();
       millis_per_frame = 1000 / target_frame_rate;
       running_game_set = true;
    }
    void Bootstrap::setUsePathTracing(bool enabled) {
        use_path_tracing = enabled;
    }
    bool Bootstrap::getUsePathTracing() {
        return use_path_tracing;
    }

    void Bootstrap::setup() {

        /////////////////////
        // Initialize GLFW //
        /////////////////////
        if (!glfwInit()) {
            std::cout << "[LOG][GLFW_INIT] error calling 'glfwInit()'.\n";
            std::exit(1);
        }

        // TODO: why are shaders running 4.2 when the engine runs 4.6?
        // I'm moving back to 4.2 for testing
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        // TODO: don't hardcode size, this should be moved to the renderer
        m_Window = glfwCreateWindow(1280, 760, "ENGINE START... NO PROBLEM... 5 MINUTW... TIDIN TIDIN TIDIN... ENGINE KAPUTT!!!! (<game_name_here>)", 0, 0);

        if (m_Window == nullptr) {
            glfwTerminate();
            // TODO: change to logger
            std::cout << "[LOG][GLFW_INIT] Failed to create window with OpenGL context (glfwCreateWindow).\n";
            std::exit(1);
        }

        glfwMakeContextCurrent(m_Window);
        glfwSetFramebufferSizeCallback(m_Window, InputManager::FramebufferSizeCallback);
        glfwSetCursorPosCallback(m_Window, InputManager::MouseCallback);
        glfwSetMouseButtonCallback(m_Window, InputManager::MouseButtonCallback);
        glfwSetScrollCallback(m_Window, InputManager::ScrollCallback);
        glfwSetKeyCallback(m_Window, InputManager::KeyCallback);

        glfwSetWindowUserPointer(m_Window, reinterpret_cast<void*>(&input));

        // UNLIMITED FPS WHOOOOOOOOOOOO
        glfwSwapInterval(0);

        /////////////////////
        // Initialize GLAD //
        /////////////////////
        int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (version == 0) {
            std::cout << "[LOG][GLAD_INIT] Failed to initialize OpenGL context (gladLoadGL).\n";
            std::exit(1);
        }

        ////////////////////////////////////////////////////////////////////////////////

        std::string work_dir = std::filesystem::current_path().string();
        // GAME EXE SHOULD BE IN TOPMOST FOLDER!!!!!
        base_dir = work_dir;

        setupEnvironmentVariables(base_dir + "\\" + "envar.cfg");
        std::string config_path = base_dir + "\\" + DEFAULT_CONFIG;

        const char* msg = ("Current config path: " + config_path).c_str();
        Logger::log(msg);

        auto config = BaseFunctionality::getInstance().readConfigFile(config_path);
        bool bailOut = false;

        asset.loadAssetPath();

        bool display_engine_initialized = false;
        //bool running_game_initialized = false;

        // TODO: determine logic and structure of config file
        //       and implement here
        for (const auto& [class_name, formal_className] : config) {

            if (class_name.compare("display") == 0) {
                // TODO: do something
            }

            else if (class_name.compare("asset") == 0)
                asset.registerAssets();

            else if (class_name.compare("input") == 0)
                input.initialize();

        }

        ShaderManager& sm = ShaderManager::getInstance();
        sm.loadShader("collider", false);
        sm.loadShader("cubemap", false);
        sm.loadShader("background", false);
        sm.loadShader("simple", false);
        sm.loadShader("copyTexture", false);

        return;

        if (!display_engine_initialized) {
            Logger::log("display not initialized", LoggerLevel::LOG_LEVEL_ERROR);
            bailOut = true;
        }

        if (!running_game_set) {
            Logger::log("runningame not initialized", LoggerLevel::LOG_LEVEL_ERROR);
            bailOut = true;
        }

        // Missing key component(s), GET TO THE CHOPPA!!!
        if (bailOut)
            std::exit(0);

    }

    void Bootstrap::setupEnvironmentVariables(std::string path) {
        Logger::log(("Path is" + path).c_str());

        std::unordered_map<std::string, std::string> config = BaseFunctionality::getInstance().readConfigFile(path);

        for (const auto&[key, value] : config)
            en_vars[key] = value;
    }

    void Bootstrap::Main(std::string args[]) {
		long long time_in_milliseconds_start, last_tick, time_in_milliseconds_end;
		long interval;
		int sleep;
		int tfro = 1;
		bool phys_update = false;
		bool phys_debug = false;

        //setup the engine
        setup();
        start_time = getCurrentMillis();
        frames = 0;

        running_game->initalize();

        time_in_milliseconds_start = start_time;
        last_tick = start_time;

        phys.gravity_modifier = 10.f;

        phys_debug = getEnvironmentVariable("physics_debug") == "1";

        //SceneManager sm{};
        SceneManager& sm = SceneManager::getInstance();
        TextureManager& tm = TextureManager::getInstance();
        ShaderManager& shm = ShaderManager::getInstance();
        GUI gui{ m_Window, sm };
        Renderer renderer{ sm, tm, shm, gui, m_Window };

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        while (!glfwWindowShouldClose(m_Window)) {
            frames += 1;
            time_in_milliseconds_start = getCurrentMillis();

            running_game->update();

            if(running_game->isRunning()){

                renderer.m_usePathTracing = use_path_tracing;

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

                // physicsmanager should not do this, very bad!!
				//if (phys_debug) {
				//	// phys.drawDebugColliders();
				//}

                /////////////////
                // Render shit //
                /////////////////

                // Render code goes here
                renderer.render();

                // this should be done in InputManager, very bad!!
                glfwPollEvents();

                //////////////////////////////////////

                // Clean up objects that are to be deleted
                GameObjectManager::getInstance().cleanup();

                time_in_milliseconds_end = getCurrentMillis();

                frame_times.push_back(time_in_milliseconds_end);
                interval = time_in_milliseconds_end - time_in_milliseconds_start;
                time_elapsed += delta_time;
                sleep = (int)(millis_per_frame - interval);

                if (sleep >= 0)
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleep));
                    //SDL_Delay(sleep); 
                
                time_in_milliseconds_end = getCurrentMillis();
                delta_time = (time_in_milliseconds_end - time_in_milliseconds_start) / 1000.0f;
                last_tick = time_in_milliseconds_start;
                
            }
        }

        // TODO: cleanup!!
        // Deletes all ImGUI instances
        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();
        
        Logger::log("Bing!", LOG_LEVEL_FATAL);
    }
}