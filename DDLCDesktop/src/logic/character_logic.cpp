#include <logic/character_logic.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include <ddlc_desktop.h>
#include <behaviour/ai/character_ai.h>
#include <behaviour/character_interaction.h>
#include <behaviour/character_state.h>
#include <logic/states/idle_state.h>
#include <config/config.h>
#include <core/input.h>
#include <core/sys.h>
#include <core/window.h>
#include <ddlc/characters.h>
#include <error/ddlcd_runtime_error.h>
#include <error/error_stories.h>
#include <utility/string_utils.h>
#include <visual/character_visuals.h>
#include <visual/ui/button.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

character_logic::character_logic(window* window) {
    window_ = window;

    if (!fs::exists("manifest.json")) {
        create_default_manifest();
    }

    config_ = config::get();
    character_ = config_->character;

    // create visuals
    visuals = new character_visuals(window_->get_renderer(), character_);

    // create ai
    ai = std::make_unique<character_ai>();
    memory = std::make_unique<character_memory>();
    ai->set_memory(memory.get());
    ai->load_state();

    window_->on_mouse_click.push_back([this]() {
        character_interaction interaction(character_interaction::kind::CLICK);
        handle_interaction(interaction);
        return 0;
    });

    if (!fs::exists("config.json")) {
        
    }
    current_state_ = std::make_unique<idle_state>(this);
}

void character_logic::handle_interaction(const character_interaction &interaction) {
    // delegate to state
	current_state_->handle_interaction(interaction);
}

void character_logic::tick(float delta_time) {
    if (paused_) return;

    if (first_tick_) {
        awake();
        first_tick_ = false;
        return;
    }

    if (visuals->in_popup()) {
        return;
    }

    current_state_->tick(delta_time);
    visuals->tick(delta_time);

    if (auto ns = current_state_->next_state(); ns != nullptr) {
        transition_state(std::move(ns));
    }
}
void character_logic::awake() {
    
}

void character_logic::handle_error(const ddlcd_runtime_error& err) {

}

void character_logic::reset_chars_per_second() {
	visuals->set_chars_per_second(DEFAULT_CHARS_PER_SECOND);
}

void character_logic::transition_state(std::unique_ptr<iconversation_state> new_state) {
    if (current_state_) {
        current_state_->exit();
    }
	current_state_ = std::move(new_state);
    if (current_state_) {
        current_state_->enter();
	}
}

bool character_logic::update_available() {
    if (!fs::exists("Updater.exe")) {
        return false; // updater not present
    }

	wchar_t cmd[] = L"Updater.exe check";
    int available = run_cmd_hidden(cmd);
    return (available == 1);
}

void character_logic::reset_all() {
    ai->cancel_and_reset();
    visuals->reset(character_);

    first_tick_ = true;
}

void character_logic::create_default_manifest() {
    json manifest_json;

	manifest_json["version"] = DDLC_DESKTOP_VERSION;

    std::ofstream manifest_file("manifest.json");
    manifest_file << manifest_json.dump(4);
    manifest_file.close();
}

void character_logic::set_character(ddlc_character new_character) {
    if (character_ == new_character) {
        return;
    }

    character_ = new_character;
        config_->character = character_;

    if (!supports_behaviour_preset(character_, config_->behaviour_preset)) {
        config_->behaviour_preset = "default";
    }

    config::save();
    reset_all();
}

int character_logic::run_cmd_hidden(wchar_t* cmd, bool wait) {
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    BOOL ok = CreateProcessW(
        nullptr,
        cmd,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (ok)
    {
        int ec = 0;
        if (wait) {
            WaitForSingleObject(pi.hProcess, INFINITE);
            DWORD exit_code;
            GetExitCodeProcess(pi.hProcess, &exit_code);
			ec = static_cast<int>(exit_code);
        }

        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        return ec;
    }
}
