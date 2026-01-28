#pragma once

#include <string>

class updater {
public:
	void check_for_updates();
	void perform_update(const std::string& version);

private:
	std::string get_latest_version();
};