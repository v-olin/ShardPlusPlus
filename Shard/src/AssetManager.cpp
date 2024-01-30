#include "AssetManager.h"
#include "Bootstrap.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace Shard {
	AssetManager::AssetManager() {
		// get from env var
		//asset_path_ = "some/path";
		//asset_path_ = Bootstrap::getEnvironmentVariable("assetpath");
	}

	void AssetManager::loadAssetPath() {
		asset_path_ = Bootstrap::getEnvironmentVariable("assetpath");
	}

	void AssetManager::registerAssets() {
		assets_.clear();
		walkDirectory(asset_path_);
	}

	void AssetManager::walkDirectory(std::string directory) {
		const auto& files = fs::directory_iterator(directory);

		for (const auto& entry : files) {
			if (entry.is_directory()) {
				walkDirectory(entry.path().generic_string());
			}
			else {
				std::string filepath{ entry.path().generic_string() };
				std::string filename = getName(filepath);

				assets_.insert(std::make_pair(filename, filepath));
			}
		}
	}

	std::string AssetManager::getName(std::string path) {
		return path.substr(path.find_last_of("/\\") + 1);
	}

	const char* AssetManager::getAssetPath(const char* asset) {
		if (assets_.find(asset) == assets_.end())
			return "";

		// debug log asset not found
		auto tmp = assets_[asset].c_str();

		return tmp;	
	}
}