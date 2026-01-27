#include <ddlc/poses.h>

#include <unordered_map>
#include <string>
#include <stdexcept>

#include <ddlc/characters.h>

std::string ddlc::get_pose_code_left(ddlc_character chr, const std::string& pose) {
    static const std::unordered_map<std::string, std::string> monika_poses = {
        {"neutral", "1"},
        {"one_hand_hip", "1"},
        {"one_hand_finger_point", "2"},
        {"hip_point", "2"}
    };
    static const std::unordered_map<std::string, std::string> yuri_poses = {
        {"neutral", "1"}, {"one_arm_at_chest", "1"}, {"chest_hand_fidget", "2"}
    };
    static const std::unordered_map<std::string, std::string> natsuki_poses = {
        {"neutral", "1"},
        {"left_hand_hip", "2"},
        {"right_hand_hip", "1"},
        {"hands_on_hips", "2"}
    };
    static const std::unordered_map<std::string, std::string> sayori_poses = {
        {"neutral", "1"},
        {"left_arm_raised", "2"},
        {"right_arm_raised", "1"},
        {"arms_raised", "2"},
    };

    const auto& pose_map =
        (chr == ddlc_character::MONIKA) ? monika_poses
        : (chr == ddlc_character::YURI) ? yuri_poses
        : (chr == ddlc_character::NATSUKI) ? natsuki_poses
        : (chr == ddlc_character::SAYORI) ? sayori_poses
        : throw std::runtime_error("Unknown character when getting pose code");

    auto it = pose_map.find(pose);
    if (it != pose_map.end()) {
        return it->second;
    }

    return "1"; // default fallback
}
std::string ddlc::get_pose_code_right(ddlc_character chr, const std::string& pose) {
    static const std::unordered_map<std::string, std::string> monika_poses = {
        {"neutral", "1"},
        {"one_hand_hip", "2"},
        {"one_hand_finger_point", "1"},
        {"hip_point", "2"}
    };
    static const std::unordered_map<std::string, std::string> yuri_poses = {
        {"neutral", "1"}, {"one_arm_at_chest", "2"}, {"chest_hand_fidget", "2"}
    };
    static const std::unordered_map<std::string, std::string> natsuki_poses = {
        {"neutral", "1"},
        {"left_hand_hip", "1"},
        {"right_hand_hip", "2"},
        {"hands_on_hips", "2"}
    };
    static const std::unordered_map<std::string, std::string> sayori_poses = {
        {"neutral", "1"},
        {"left_arm_raised", "1"},
        {"right_arm_raised", "2"},
        {"arms_raised", "2"},
    };

    const auto& pose_map =
        (chr == ddlc_character::MONIKA) ? monika_poses
        : (chr == ddlc_character::YURI) ? yuri_poses
        : (chr == ddlc_character::NATSUKI) ? natsuki_poses
        : (chr == ddlc_character::SAYORI) ? sayori_poses
        : throw std::runtime_error("Unknown character when getting pose code");

    auto it = pose_map.find(pose);
    if (it != pose_map.end()) {
        return it->second;
    }

    return "1"; // default fallback
}
