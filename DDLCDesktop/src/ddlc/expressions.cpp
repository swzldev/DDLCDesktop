#include <ddlc/expressions.h>

#include <unordered_map>
#include <string>
#include <stdexcept>

#include <ddlc/characters.h>

std::string ddlc::get_expression_code(ddlc_character chr, const std::string& expression) {
    static const std::unordered_map<std::string, std::string> monika_expressions = {
        {"smile", "a"},
        {"open_smile", "b"},
        {"neutral", "c"},
        {"neutral_open", "d"},
        {"soft_smile", "e"},
        {"neutral_raised", "f"},
        {"neutral_open_raised", "g"},
        {"serious", "h"},
        {"serious_open", "i"},
        {"warm_smile", "j"},
        {"joyful", "k"},
        {"nervous_laugh", "l"},
        {"nervous_smile", "m"},
        {"awkward_laugh", "n"},
        {"nervous", "o"},
        {"nervous_open", "p"},
        {"relaxed", "q"},
        {"open_relaxed", "r"}
    };
    static const std::unordered_map<std::string, std::string> yuri_expressions = {
        {"smile", "a"},
        {"head_tilt_look_forward", "a2"},
        {"open_smile", "b"},
        {"head_tilt_look_away", "b2"},
        {"warm_smile", "c"},
        {"head_tilt_face_red", "c2"},
        {"joyful", "d"},
        {"head_tilt_nervous", "d2"},
        {"ooh", "e"},
        {"head_tilt_face_red_smile", "e2"},
        {"amazed", "f"},
        {"serious", "g"},
        {"serious_open_slightly", "h"},
        {"eyes_dilated_open", "hisui"},
        {"serious_smile", "i"},
        {"serious_open_wide", "j"},
        {"breathe_eyes_closed", "k"},
        {"breathe_eyes_closed_2", "l"},
        {"smile_eyes_closed", "m"},
        {"nervous", "n"},
        {"nervous_look_away", "o"},
        {"distraught", "p"},
        {"nervous_laugh", "q"},
        {"angry_serious", "r"},
        {"relieved", "s"},
        {"nervous_breath", "t"},
        {"soft_smile", "u"},
        {"hesitant_worry", "v"},
        {"relieved_breath", "w"},
        {"obsessive_eyes_joy", "y1"},
        {"obsessive_nervous", "y2"},
        {"obsessive_manic_delight", "y3"},
        {"obsessive_ooh", "y4"},
        {"overjoyed", "y5"},
        {"soft_hesitation", "y6"},
        {"obsessive_angry", "y7"}
    };
    static const std::unordered_map<std::string, std::string> natsuki_expressions = {
        {"smile", "a"},
        {"talk", "b"},
        {"mouth_open", "c"},
        {"smile_open", "d"},
        {"talk_serious", "e"},
        {"frustrated", "f"},
        {"annoyed", "g"},
        {"flustered_open", "h"},
        {"flustered", "i"},
        {"smile_sincere", "j"},
        {"talk_sincere", "k"},
        {"joyful", "l"},
        {"concerned_open", "m"},
        {"concerned", "n"},
        {"angry", "o"},
        {"shout", "p"},
        {"look_away_open", "q"},
        {"look_away_angry", "r"},
        {"look_away", "s"},
        {"scream", "scream"},
        {"cool_smile_open", "t"},
        {"concerned_look_away", "u"},
        {"cute_moan", "v"},
        {"talk_eyes_closed", "w"},
        {"angry_eyes_closed", "x"},
        {"smartass", "y"},
        {"overjoyed", "z"}
    };
    static const std::unordered_map<std::string, std::string> sayori_expressions = {
        {"smile", "a"},
        {"neutral", "b"},
        {"talk", "c"},
        {"smile_sincere", "d"},
        {"hesitant", "e"},
        {"concern", "f"},
        {"concern_open", "g"},
        {"concern_talk", "h"},
        {"serious", "i"},
        {"serious_talk", "j"},
        {"soft_sorrow", "k"},
        {"nervous_laugh", "l"},
        {"surprised", "m"},
        {"amazed_ooh", "n"},
        {"serious_nervous", "o"},
        {"cute_moan", "p"},
        {"warm_smile", "q"},
        {"joyful", "r"},
        {"joyful_blush", "s"},
        {"sincere_cry", "t"},
        {"sad_cry", "u"},
        {"cry_blush", "v"},
        {"cry", "w"},
        {"happy", "x"},
        {"smile_sincere_blush", "y"}
    };

    const auto& expr_map =
        (chr == ddlc_character::MONIKA) ? monika_expressions
        : (chr == ddlc_character::YURI) ? yuri_expressions
        : (chr == ddlc_character::NATSUKI) ? natsuki_expressions
        : (chr == ddlc_character::SAYORI) ? sayori_expressions
        : throw std::runtime_error("Unknown character when getting expression code");

    auto it = expr_map.find(expression);
    if (it != expr_map.end()) {
        return it->second;
    }

    return "a"; // default fallback
}