#include <ddlc/characters.h>

#include <unordered_map>
#include <string>

std::string ddlc_character_to_string(ddlc_character character) {
    static const std::unordered_map<ddlc_character, std::string> character_map = {
        { ddlc_character::MONIKA, "Monika" },
        { ddlc_character::YURI, "Yuri" },
        { ddlc_character::NATSUKI, "Natsuki" },
        { ddlc_character::SAYORI, "Sayori" },
	};
    auto it = character_map.find(character);
    if (it != character_map.end()) {
        return it->second;
    }
	return "Unknown";
}
