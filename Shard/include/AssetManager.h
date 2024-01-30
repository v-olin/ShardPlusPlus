#pragma once

#include <unordered_map>
#include <string>

namespace Shard {
	class AssetManager {
	public:
        static AssetManager& getInstance() {
            static AssetManager instance;
            return instance;
        }

         AssetManager(AssetManager const&) = delete;
         AssetManager& operator=(AssetManager const&) = delete;
        
         void loadAssetPath();
         void registerAssets();
         void walkDirectory(std::string directory);
         std::string getName(std::string path);
         const char* getAssetPath(const char* asset_name);
	
    private:
        AssetManager();
        
        std::unordered_map<std::string, std::string> assets_;
        std::string asset_path_;
	};
}