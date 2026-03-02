#include <updater.h>

#include <Windows.h>
#include <string>
#include <fstream>
#include <filesystem>

#include <output.h>

#include <nlohmann/json.hpp>
#include <zip.h>

namespace fs = std::filesystem;
using json = nlohmann::json;

bool updater::has_update() {
	std::string current_version = get_current_version();
	if (current_version.empty()) {
		output::print("Warn: failed to read current version information.");
		return true;
	}

	std::string latest_version = get_latest_version();
	if (latest_version.empty()) {
		output::print("Failed to fetch latest version information.");
		return false;
	}

	if (current_version != latest_version) {
		return true;
	}
	return false;
}
void updater::update_latest() {
	if (!has_update()) {
		output::print("No updates are available.");
		return;
	}

	std::string latest_version = get_latest_version();
	perform_update(latest_version);
}
void updater::perform_update(const std::string& version) {
	std::string v = version;
	if (v.empty() || v == "latest") {
		v = get_latest_version();
		if (v.empty()) {
			output::print("Failed to fetch latest version information.");
			return;
		}
	}

	output::print("Attempting to update to: {}", v);

	if (!validate_version(v)) {
		output::print("Invalid version specified for update: {}", v);
		return;
	}

	if (v == get_current_version()) {
		output::print("You are already using version: {}. Install anyway? (y/n)", v);

		while (true) {
			std::string input = output::readline();
			if (input == "n" || input == "N") {
				output::print("Update cancelled.");
				return;
			}
			else if (input == "y" || input == "Y") {
				break;
			}
			else {
				output::print("Please enter 'y' or 'n': ");
			}
		}
	}

	json tag_response =
		json::parse(requestor_.request("https://api.github.com/repos/swzldev/DDLCDesktop/releases/tags/" + v));

	std::string update_zip;
	for (const auto& asset : tag_response["assets"]) {
		std::string asset_name = asset.value("name", "");
		if (asset_name == "DDLCDesktop-" + v + ".zip") {
			update_zip = asset.value("browser_download_url", "");
			break;
		}
	}

	if (update_zip.empty()) {
		output::print("Failed to find update asset for version: {}", v);
		return;
	}

	// download zip file
	output::print("Downloading update package for version {}...", v);
	std::string zip_data = requestor_.request(update_zip);
	if (zip_data.empty()) {
		output::print("Failed to download update package for version: {}", v);
		return;
	}

	// create temporary folder for update files
	fs::path temp_dir = fs::temp_directory_path() / ("DDLCDesktop_Update_" + v);
	fs::create_directories(temp_dir);
	fs::remove_all(temp_dir);
	fs::path extract_dir = temp_dir / "extracted";
	fs::create_directories(extract_dir);

	// save zip file
	output::print("Saving update package...");
	fs::path zip_path = temp_dir / "update.zip";
	std::ofstream zip_file(zip_path, std::ios::binary);
	zip_file.write(zip_data.data(), zip_data.size());
	zip_file.close();

	output::print("Update to version {} downloaded to {}", v, zip_path.string());

	// extract zip file
	output::print("Extracting update package...");
	if (!extract_update_package(zip_path.string(), extract_dir.string())) {
		output::print("Failed to extract update package.");
		return;
	}

	output::print("Update package extracted to {}", extract_dir.string());

	// create update script
	output::print("Creating update script...");
	fs::path install_dir = fs::current_path();
	if (!create_update_script(extract_dir, install_dir)) {
		output::print("Failed to create update script.");
		return;
	}

	// apply update
	output::print("Update script created, applying update.");

	apply_update(extract_dir);
}

std::string updater::get_latest_version() {
	json json_response =
		json::parse(requestor_.request("https://api.github.com/repos/swzldev/DDLCDesktop/releases/latest"));

	return json_response.value("tag_name", "").substr(1);
	// ignore the 'v' prefix for consistency ^^
}
std::string updater::get_current_version() {
	if (!manifest_.loaded) {
		if (!manifest_.load()) return "unknown";
	}
	return manifest_.version;
}
bool updater::validate_version(const std::string& version) {
	if (version.empty()) {
		return false;
	}
	
	// invalid versions
	if (version == "1.0.0"
		|| version == "1.0.0-alpha"
		|| version == "1.0.0-beta"
		|| version == "1.1.0"
		|| version == "1.1.1") {
		output::print("Note: versions before 2.0.0 don't support the updater.");
		return false;
	}

	json json_response =
		json::parse(requestor_.request("https://api.github.com/repos/swzldev/DDLCDesktop/tags"));

	for (const auto& tag : json_response) {
		if (tag.value("name", "").substr(1) == version) {
			return true;
		}
	}

	return false;
}

void updater::apply_update(const fs::path& update_dir) {
	fs::path script_path = update_dir / "apply_update.bat";

	// start update script
	STARTUPINFOA si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	std::string cmd = "\"" + script_path.string() + "\"";

	if (CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, FALSE,
		CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// allow for update
		std::exit(0);
	}
}
bool updater::extract_update_package(const std::string& zip_path, const std::string& out_dir) {
	int err = 0;
	zip* za = zip_open(zip_path.c_str(), 0, &err);
	if (!za) {
		output::print("Failed to open downloaded zip file.");
		return false;
	}

	zip_int64_t num_entries = zip_get_num_entries(za, 0);
	for (zip_int64_t i = 0; i < num_entries; i++) {
		const char* name = zip_get_name(za, i, 0);
		if (!name) continue;

		fs::path file_path = fs::path(out_dir) / name;

		// Check if entry is a directory
		if (name[strlen(name) - 1] == '/') {
			fs::create_directories(file_path);
			continue;
		}

		// Create parent directories if needed
		fs::create_directories(file_path.parent_path());

		// Open file in zip
		zip_file* zf = zip_fopen_index(za, i, 0);
		if (!zf) {
			output::print("Failed to open file in zip: {}", name);
			continue;
		}

		// Read and write file
		std::ofstream out_file(file_path, std::ios::binary);
		char buffer[8192];
		zip_int64_t bytes_read;
		while ((bytes_read = zip_fread(zf, buffer, sizeof(buffer))) > 0) {
			out_file.write(buffer, bytes_read);
		}

		out_file.close();
		zip_fclose(zf);
	}

	zip_close(za);
	return true;
}
bool updater::create_update_script(const fs::path& update_dir, const fs::path& install_dir) {
	fs::path script_path = update_dir / "apply_update.bat";
	std::ofstream script(script_path);

	if (!script.is_open()) {
		return false;
	}

	std::string exe_name = "DDLCDesktop.exe";
	DWORD process_id = GetCurrentProcessId();

	script << "@echo off\n";
	script << "echo Waiting for application to close...\n";
	script << ":wait_loop\n";
	script << "tasklist /FI \"PID eq " << process_id << "\" 2>NUL | find /I /N \"" << exe_name << "\">NUL\n";
	script << "if \"%ERRORLEVEL%\"==\"0\" (\n";
	script << "    timeout /t 1 /nobreak >NUL\n";
	script << "    goto wait_loop\n";
	script << ")\n\n";

	script << "echo Applying update...\n";
	script << "xcopy /E /I /Y \"" << update_dir.string() << "\\*\" \"" << install_dir.string() << "\"\n\n";

	script << "echo Restarting application...\n";
	script << "start \"\" \"" << (install_dir / exe_name).string() << "\"\n\n";

	script << "echo Cleaning up...\n";
	script << "rd /s /q \"" << update_dir.parent_path().string() << "\"";
	script << "(goto) 2>nul & del \"%~f0\"\n";

	script.close();
	return true;
}
