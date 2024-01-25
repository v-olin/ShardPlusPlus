#pragma once

#ifdef _WINDLL
#define SHARD_API __declspec(dllexport)
#else
#define SHARD_API __declspec(dllimport)
#endif

#include <unordered_map>
#include <string>

namespace Shard {
	class AssetManager {
	public:
        static SHARD_API AssetManager& getInstance() {
            static AssetManager instance;
            return instance;
        }

        SHARD_API AssetManager(AssetManager const&) = delete;
        SHARD_API AssetManager& operator=(AssetManager const&) = delete;
        
        SHARD_API void registerAssets();
        SHARD_API void walkDirectory(std::string directory);
        SHARD_API std::string getName(std::string path);
        SHARD_API std::string getAssetPath(std::string asset_name);
	
    private:
        AssetManager();
        
        std::unordered_map<std::string, std::string> assets_;
        std::string asset_path_;
	};
}