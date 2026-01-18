#include <ddlc/characters.h>

#include <unordered_map>
#include <vector>
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

std::vector<std::string> get_behaviour_presets(ddlc_character character) {
    std::vector<std::string> monika_presets = {
        "postgame", "romantic", "confident", "obsessive", "shy"
    };
    std::vector<std::string> yuri_presets = {
        "postgame", "romantic", "obsessive", "shy"
    };
    std::vector<std::string> natsuki_presets = {
        "postgame", "romantic", "hurt", "playful"
    };
    std::vector<std::string> sayori_presets = {
        "postgame", "depressed", "happy", "romantic", "obsessive"
    };

    switch (character) {
    case ddlc_character::MONIKA:
        return monika_presets;
    case ddlc_character::YURI:
        return yuri_presets;
    case ddlc_character::NATSUKI:
        return natsuki_presets;
    case ddlc_character::SAYORI:
        return sayori_presets;
    default:
        return {};
    }
}

bool supports_behaviour_preset(ddlc_character character, const std::string& preset) {
    if (preset.empty() || preset == "default") {
        return true; // default preset
    }

	const auto& presets = get_behaviour_presets(character);
    if (std::find(presets.begin(), presets.end(), preset) != presets.end()) {
        return true;
	}
    return false;
}
