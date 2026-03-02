#include <iostream>

#include <output.h>
#include <updater.h>

int main(int argc, char** argv) {
	if (argc > 3 || argc < 1) {
		std::cerr << "Usage: Updater <command> [options]" << std::endl;
		return 1;
	}

	updater updater;
	if (argc == 1) {
		output::print("DDLCDesktop version: {}", updater.get_current_version());

		output::print("\nCommands:");
		output::print("-- <none>: Displays this info message.");
		output::print("-- upgrade: Installs the latest version if one is available.");
		output::print("-- install [version]: Installs the version specified. If [version] is omitted the latest version is used.");
		output::print("-- check: Checks if an update is available. Does not print, mainly useful for CLI interfacing with the main application.");

		if (updater.has_update()) {
			output::print("\nNote: an update is available ({} -> {}). To install run `Updater upgrade`", updater.get_current_version(), updater.get_latest_version());
		}

		return 0;
	}

	std::string command = argv[1];

	if (command == "upgrade") {
		updater.update_latest();
	}
	else if (command == "install") {
		std::string version = (argc == 3) ? argv[2] : "";
		updater.perform_update(version);
		return 0; // *should* exit before return
	}
	else if (command == "check") {
		bool update_available = updater.has_update();
		if (update_available) {
			output::print("Note: an update is available ({} -> {}). To install run `Updater upgrade`", updater.get_current_version(), updater.get_latest_version());
			return 1;
		}
		return 0;
	}
	else {
		output::print("Unknown command: {}", command);
	}
}