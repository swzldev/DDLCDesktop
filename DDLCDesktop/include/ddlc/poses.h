#pragma once

#include <string>

#include <ddlc/characters.h>

namespace ddlc {
	std::string get_pose_code_left(ddlc_character chr, const std::string& pose);
	std::string get_pose_code_right(ddlc_character chr, const std::string& pose);
}
