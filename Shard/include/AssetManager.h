#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <unordered_map>
#include <string>

namespace Shard {
	class SHARD_API AssetManager {
	public:
        static AssetManager& getInstance() {
            static AssetManager instance;
            return instance;
        }

        AssetManager(AssetManager const&) = delete;
        AssetManager& operator=(AssetManager const&) = delete;

        void registerAssets();
        void walkDirectory(std::string directory);
        std::string getName(std::string path);
        std::string getAssetPath(std::string asset_name);
	
    private:
        AssetManager();
        
        std::unordered_map<std::string, std::string> assets_;
        std::string asset_path_;
	};
}