#pragma once

#include <memory>

#include <core/textbox.h>
#include <core/character.h>
#include <visual/character_visuals.h>
#include <behaviour/character_interaction.h>
#include <behaviour/ai/character_ai.h>
#include <ddlc/characters.h>
#include <config/config.h>
#include <error/ddlcd_runtime_error.h>
#include <logic/iconversation_state.h>
#include <behaviour/interactions/interaction_manager.h>
#include <settings/settings.h>

#include <nlohmann/json.hpp>

class window;

class character_logic {
public:
	static constexpr int INPUT_MAX_LENGTH = 170;
	static constexpr float AUTO_MODE_DELAY_SEC = 3.0f;
	static constexpr float DEFAULT_CHARS_PER_SECOND = 40.0f;

public:
	character_logic(widget* widget);

	void handle_interaction(const character_interaction& interaction);

	inline void pause() { paused_ = true; }
	inline void unpause() { paused_ = false; }
	void tick(float delta_time);
	void awake();

	void shutdown() const;

	void handle_error(const ddlcd_runtime_error& err);

	void reset_chars_per_second();

	void generate_ai_response();

	std::unique_ptr<character_ai> ai;
	std::unique_ptr<character_memory> memory;
	textbox* tbox = nullptr;
	character* chr = nullptr;
	settings* settingz = nullptr;

private:
	widget* widget_ = nullptr;
	config* config_ = nullptr;
	ddlc_character character_ = ddlc_character::MONIKA;
	bool first_tick_ = true;
	bool paused_ = false;

	std::unique_ptr<iconversation_state> current_state_;
	std::unique_ptr<interaction_manager> interaction_mgr_;

	void transition_state(std::unique_ptr<iconversation_state> new_state);

	bool update_available();

	// auto
	bool auto_mode_ = false;
	float auto_timer_ = 0.0f;

	void reset_all();

	void create_default_manifest();

	void set_character(ddlc_character new_character);

	int run_cmd_hidden(wchar_t* cmd, bool wait = true);

	void refresh_display();
};