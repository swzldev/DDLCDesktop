#pragma once

#include <string>
#include <filesystem>

#include <requestor.h>
#include <manifest.h>

namespace fs = std::filesystem;

class updater {
public:
	bool has_update();
	void update_latest();
	void perform_update(const std::string& version);

	std::string get_latest_version();
	std::string get_current_version();
	bool validate_version(const std::string& version);

private:
	requestor requestor_;
	manifest manifest_;

	void apply_update(const fs::path& update_dir);
	bool extract_update_package(const std::string& zip_path, const std::string& out_dir);
	bool create_update_script(const fs::path& update_dir, const fs::path& install_dir);
};