#pragma once

#include <functional>
#include <memory>
#include <string>

#include <behaviour/character_interaction.h>
#include <ai/character_ai.h>
#include <config/config.h>
#include <ddlc/characters.h>
#include <error/ddlcd_runtime_error.h>
#include <visual/character_visuals.h>

#include <nlohmann/json.hpp>

enum class logic_state {
    IDLE,
    THINKING,
    TALKING,
    AWAITING_CHOICE,
    AWAITING_INPUT,
    AWAITING_INPUT_SETTINGS,
};

enum class error_state {
    NONE,
    NON_FATAL,
    FATAL,
};

enum class menu_state {
    MAIN,
    SETTINGS,
    SETUP,
    UPDATE,
};

class widget;
class window;

class character_logic {
public:
    static constexpr int INPUT_MAX_LENGTH = 170;

public:
    character_logic(widget* widget);
    ~character_logic();

    void handle_interaction(const character_interaction& interaction);

    inline void pause() { paused_ = true; }
    inline void unpause() { paused_ = false; }
    void tick(float delta_time);
    void awake();

    void handle_error(const ddlcd_runtime_error& error);

    character_visuals* visuals;
    character_ai* ai;
    imessage_repository* repo;
    character_state current_state;

private:
    struct settings_input_session {
        bool active = false;
        std::string prompt;
        std::string buffer;
        std::string* target = nullptr;
        std::function<void(bool, std::string)> callback;
        logic_state restore_state = logic_state::IDLE;
    };

	widget* widget_ = nullptr;
    window* window_ = nullptr;
    config* config_ = nullptr;
    ddlc_character character_ = ddlc_character::MONIKA;
    bool first_tick_ = true;
    bool paused_ = false;

    error_state error_state_ = error_state::NONE;
    logic_state state_ = logic_state::IDLE;
    unsigned int interaction_index_ = 0;

    bool input_mode_btn_disabled_ = true;
    bool custom_mode_ = false;

    std::string live_input_buffer_;
    settings_input_session settings_input_;

    bool in_update_ = false;
    void show_update_menu();
    void show_confirm_update();
    bool in_setup_ = false;
    unsigned int setup_step_ = 0;
    void show_setup(unsigned int step);

    bool update_available();

    bool auto_mode_ = false;
    float auto_timer_ = 0.0f;

    menu_state current_menu_ = menu_state::MAIN;
    void show_main_menu();
    void show_settings_menu();
    void show_settings_general_menu();
    void show_settings_api_menu();
    void show_settings_character_menu();
    void show_settings_character_change_menu();
    void show_settings_user_menu();

    void await_choice(bool show_immediate = false);
    void await_input();
    void await_input_custom(const std::string& prompt, const std::string& initial_value, const std::function<void(bool, const std::string&)>& callback);
    void finish_settings_input(bool success);

    float get_auto_delay_sec() const;

    int get_choice_input(int num_choices);

    void reset();
    void reset_fully(bool all_characters = false);

    void begin_think(const character_interaction& interaction);

    void display_think();
    void display_current_interaction();
    void advance_interaction();

    void create_default_manifest();

    void set_character(ddlc_character new_character, bool warn_preset = true);

    void update_rpc();

    int run_cmd_hidden(wchar_t* cmd, bool wait = true);

    void close();
    void refresh_display();
};