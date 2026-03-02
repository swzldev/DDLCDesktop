#include <manifest.h>

#include <fstream>
#include <string>
#include <filesystem>

#include <nlohmann/json.hpp>

bool manifest::load() {
	if (!std::filesystem::exists("manifest.json")) {
		return false;
	}

	std::ifstream manifest_file("manifest.json");
	if (!manifest_file.is_open()) {
		return false;
	}

	nlohmann::json json_manifest;
	manifest_file >> json_manifest;
	manifest_file.close();

	version = json_manifest.value("version", "");

	loaded = true;

	return true;
}
void manifest::save() {
	nlohmann::json json_manifest;
	json_manifest["version"] = version;

	std::ofstream manifest_file("manifest.json");
	if (!manifest_file.is_open()) {
		return;
	}

	manifest_file << json_manifest.dump();
	manifest_file.close();
}
