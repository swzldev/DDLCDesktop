#pragma once

#include <string>

class manifest {
public:
	bool load();
	void save();

	std::string version;

	bool loaded = false;
};