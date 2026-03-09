#include <behaviour/character_logic.h>

#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include <ddlc_desktop.h>
#include <ai/character_ai.h>
#include <behaviour/character_interaction.h>
#include <behaviour/character_state.h>
#include <persistance/json_message_repository.h>
#include <persistance/run_on_boot_helper.h>
#include <config/config.h>
#include <core/input.h>
#include <core/sys.h>
#include <core/window.h>
#include <core/widget.h>
#include <ddlc/characters.h>
#include <error/ddlcd_runtime_error.h>
#include <error/error_stories.h>
#include <utility/string_utils.h>
#include <visual/character_visuals.h>
#include <visual/ui/button.h>
#include <visual/ui/text_button.h>
#include <visual/ui/toggle_button.h>
#include <visual/ui/number_button.h>
#include <discord/discord_rpc.h>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace {
	bool checked_for_update_session = false;
}

character_logic::character_logic(widget* widget) {
	widget_ = widget;
	window_ = widget->get_window();

	create_default_manifest();

	if (!fs::exists("config.json")) {
		in_setup_ = true;
		setup_step_ = 0;
	}
	config_ = config::get();

	if (config_->rpc_enable) {
		widget_->get_discord().enable();
	}

	// set starting character
	set_character(config_->character, false);

	// create visuals
	visuals = new character_visuals(window_->get_renderer(), character_);

	// create repo
	fs::create_directories("data");
	repo = new json_message_repository("data/chat_sessions.json");

	// create ai
	ai = new character_ai();
	ai->set_repository(repo);

	window_->on_mouse_click.push_back([this]() {
		character_interaction interaction(character_interaction::kind::CLICK);
		handle_interaction(interaction);
		return 0;
	});
}
character_logic::~character_logic() {
	if (ai) {
		delete ai;
	}
	if (visuals) {
		delete visuals;
	}
	if (repo) {
		delete repo;
	}
}

void character_logic::handle_interaction(const character_interaction& interaction) {
	if (paused_ || visuals->in_popup()) {
		return;
	}

	if (interaction.get_kind() != character_interaction::kind::CLICK ||
		current_menu_ == menu_state::SETTINGS) {
		return;
	}

	if (state_ == logic_state::IDLE) {
		begin_think(interaction);
	}
	else if (state_ == logic_state::TALKING && visuals->is_speaking()) {
		auto_mode_ = false;
		visuals->finish_speaking(); // mouse down while speaking -> quick skip
	}
	else if (state_ == logic_state::TALKING) {
		// in conversations, mouse down means advance interaction
		advance_interaction();
	}
}

void character_logic::tick(float delta_time) {
	discord_rpc::run_discord_callbacks();

	if (paused_)
		return;

	if (first_tick_) {
		awake();
		first_tick_ = false;
	}

	if (visuals->in_popup()) {
		return;
	}

	if (current_menu_ != menu_state::MAIN) {
		if (state_ == logic_state::AWAITING_INPUT_SETTINGS && settings_input_.active) {
			std::string full = settings_input_.prompt + settings_input_.buffer + "_";
			visuals->set_saying_immediate(full);
		}

		visuals->tick(delta_time);
		if (current_menu_ != menu_state::SETUP) {
			return;
		}
	}

	if (state_ == logic_state::THINKING) {
		if (!visuals->is_speaking()) {
			// constantly think
			display_think();
		}
		if (ai->is_response_ready()) {
			visuals->set_chars_per_second(50.0f);
			current_state = ai->get_response();
			if (character_ == ddlc_character::MONIKA) {
				visuals->set_style("normal"); // monika doesnt have casual style
			}
			else
				visuals->set_style(current_state.style);

			// handle errors
			if (current_state.error_code != character_state::error::NONE) {
				switch (current_state.error_code) {
				case character_state::error::FAIL_PARSE_RESPONSE_JSON:
					handle_error(
						ddlcd_runtime_error(ddlcd_error::FAIL_PARSE_AI_RESPONSE,
							current_state.error_message));
					return;
				case character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN:
					handle_error(ddlcd_runtime_error(
						ddlcd_error::FAIL_PARSE_AI_RESPONSE,
							current_state.error_message));
					return;
				default:
					break;
				}
			}

			if (!current_state.interactions.empty()) {
				state_ = logic_state::TALKING;
				interaction_index_ = 0;
				display_current_interaction();
			}
			else {
				state_ = logic_state::IDLE;
				refresh_display();
			}
		}
	}
	else if (state_ == logic_state::TALKING) {
		if (!visuals->is_speaking() && auto_mode_) {
			auto_timer_ += delta_time;
		}
		if (auto_timer_ >= get_auto_delay_sec()) {
			auto_timer_ = 0.0f;
			advance_interaction();
		}
	}
	else if (state_ == logic_state::AWAITING_CHOICE) {
		int num_actions = (int)current_state.actions.size();
		int choice = get_choice_input(num_actions);

		if (choice != -1) {
			// user made a choice
			character_interaction choice_interaction(
				character_interaction::kind::CHOICE_MADE);
			choice_interaction.str_data = current_state.actions[choice];
			choice_interaction.int_data = choice + 1;

			begin_think(choice_interaction);
		}
	}
	else if (state_ == logic_state::AWAITING_INPUT) {
		// show current input
		std::string full = "You: " + live_input_buffer_ + "_";
		visuals->set_saying_immediate(full);
	}

	visuals->tick(delta_time);
}
void character_logic::awake() {
	if (in_setup_) {
		show_setup(setup_step_);
	}
	else {
		show_main_menu();

		if (!checked_for_update_session && update_available()) {
			in_update_ = true;
			show_update_menu();
			checked_for_update_session = true;
		}
		else {
			// window opened
			character_interaction interaction(
				character_interaction::kind::WINDOW_OPEN);
			begin_think(interaction);
		}
	}
}

void character_logic::handle_error(const ddlcd_runtime_error& error) {
	// cleanup
	current_state.interactions.clear();

	const int size = 1000;
	int x = (sys::display_width() / 2) - (size / 2);
	int y = (sys::display_height()) - size;

	visuals->set_position(x, y);
	visuals->set_scale(size);

	visuals->set_expression("a");
	visuals->set_pose("1", "1");
	visuals->set_character(ddlc_character::MONIKA);
	visuals->set_chars_per_second(50.0f);

	bool fatal = false;

	current_state.interactions = error_stories::error_story(config_->user_name, error.message);
	switch (error.kind) {
	case ddlcd_error::FAIL_AI_RESPONSE:
		fatal = true;
		break;
	default:
		break;
	}

	if (!fatal) {
		current_state.interactions.push_back(
			{ "The application will now reset.", "a", "1", "1" });
	}

	error_state_ = fatal ? error_state::FATAL : error_state::NON_FATAL;
	state_ = logic_state::TALKING;

	interaction_index_ = 0;
	display_current_interaction();
}

void character_logic::show_update_menu() {
	visuals->set_chars_per_second(50.0f);

	visuals->clear_buttons();

	current_state.interactions = error_stories::update_story(config_->user_name);
	state_ = logic_state::TALKING;
	current_menu_ = menu_state::UPDATE;
	interaction_index_ = 0;
	display_current_interaction();
}
void character_logic::show_confirm_update() {
	visuals->show_popup(
		"A new update is available! Download it now?",
		[this](int response) {
			if (response == 0) {
				// yes
				wchar_t cmd[] = L"Updater.exe upgrade";
				run_cmd_hidden(cmd, false);
				window_->close();
			}
			else {
				// no
				in_update_ = false;
				state_ = logic_state::IDLE;
				show_main_menu();
				handle_interaction(
					character_interaction(character_interaction::kind::CLICK)
				);
			}
		},
		{ "Yes", "No" });
}
void character_logic::show_setup(unsigned int step) {
	visuals->set_chars_per_second(30.0f);

	visuals->clear_buttons();
	visuals->add_button(
		std::make_unique<text_button>("Quit", [this]() { close(); })
	);

	if (step == 0) {
		// part 1
		const int size = 1000;
		int x = (sys::display_width() / 2) - (size / 2);
		int y = (sys::display_height()) - size;

		visuals->set_position(x, y);
		visuals->set_scale(size);

		current_state.interactions = error_stories::firstrun_story_p1();
		state_ = logic_state::TALKING;
		current_menu_ = menu_state::SETUP;
		interaction_index_ = 0;
		display_current_interaction();
	}
	else if (step == 1) {
		// collect name
		await_input_custom("What's your name?: ", "",
			[this](bool success, const std::string& value) {
				if (success && !value.empty()) {
					config_->user_name = value;
					setup_step_++;
					show_setup(setup_step_);
				}
				else {
					visuals->show_message("Please enter a valid name.");
					show_setup(setup_step_); // retry
				}
			});
	}
	else if (step == 2) {
		// collect pronouns
		await_input_custom("What are your pronouns? (e.g. he/him): ", "",
			[this](bool success, const std::string& value) {
				if (success && !value.empty()) {
					config_->pronouns = value;
					setup_step_++;
					show_setup(setup_step_);
				}
				else {
					visuals->show_message("Please enter valid pronouns.");
					show_setup(setup_step_); // retry
				}
			});
	}
	else if (step == 3) {
		// part 2
		current_state.interactions =
			error_stories::firstrun_story_p2(config_->user_name);
		state_ = logic_state::TALKING;
		interaction_index_ = 0;
		display_current_interaction();
	}
	else if (step == 4) {
		const int size = 750;
		int x = (sys::display_width() / 8 * 6) - (size / 2);
		int y = (sys::display_height()) - size;

		visuals->set_position(x, y);
		visuals->set_scale(size);

		// try open openrouter website
		HINSTANCE h = ShellExecuteW(NULL, L"open", L"https://www.openrouter.ai",
			NULL, NULL, SW_SHOWMAXIMIZED);

		if ((INT_PTR)h <= 32) {
			visuals->show_message("Failed to open web browser. Please visit "
				"https://www.openrouter.ai manually.");
		}
		setup_step_++;
		show_setup(setup_step_);
	}
	else if (step == 5) {
		// part 3
		current_state.interactions = error_stories::firstrun_story_p3();
		state_ = logic_state::TALKING;
		interaction_index_ = 0;
		display_current_interaction();
	}
	else if (step == 6) {
		// collect api key
		await_input_custom("Enter your OpenRouter API key: ", config_->api_key,
			[this](bool success, const std::string& value) {
				if (success && !value.empty()) {
					config_->api_key = string_utils::trim(value);
					setup_step_++;
					show_setup(setup_step_);
				}
				else {
					visuals->show_message("Please enter a valid API key.");
					show_setup(setup_step_); // retry
				}
			});
	}
	else if (step == 7) {
		// part 4
		const int size = 1000;
		int x = (sys::display_width() / 2) - (size / 2);
		int y = (sys::display_height()) - size;

		visuals->set_position(x, y);
		visuals->set_scale(size);

		current_state.interactions = error_stories::firstrun_story_p4();
		state_ = logic_state::TALKING;
		interaction_index_ = 0;
		display_current_interaction();
	}
	else {
		// done
		in_setup_ = false;
		config::save();
		reset_fully(true);
	}
}

bool character_logic::update_available() {
	if (!fs::exists("Updater.exe")) {
		// updater not present
		return false;
	}

	wchar_t cmd[] = L"Updater.exe check";
	int available = run_cmd_hidden(cmd);
	return (available == 1);
}

void character_logic::show_main_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::MAIN;

	// set buttons
	visuals->add_button(
		std::make_unique<text_button>("Close", [this]() { close(); })
	);
	visuals->add_button(
		std::make_unique<toggle_button>("Auto: ", nullptr, &auto_mode_)
	);
	visuals->add_button(
		std::make_unique<text_button>("Reset", [this]() {
			if (state_ == logic_state::THINKING) {
				visuals->show_message("Cannot reset while thinking. Please wait.");
				return;
			}
			reset_fully();
		})
	);

	// TODO: CACHE THIS!!
	auto input_mode_btn = std::make_unique<toggle_button>("Input Mode: ", [this](bool t) {
		if (t) await_choice(/*true*/);
		else await_input();
	});
	input_mode_btn->set_disabled_ptr(&input_mode_btn_disabled_);
	input_mode_btn->set_labels("Choice", "Text");

	visuals->add_button(std::move(input_mode_btn));

	visuals->add_button(
		std::make_unique<text_button>("Settings", [this]() { show_settings_menu(); })
	);

	refresh_display();
}
void character_logic::show_settings_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);
	visuals->set_saying("Choose an option...");
	
	// set buttons
	visuals->add_button(std::make_unique<text_button>("Application", [this]() { show_settings_application_menu(); }));
	visuals->add_button(std::make_unique<text_button>("Discord RPC", [this]() { show_settings_discord_menu(); }));
	visuals->add_button(std::make_unique<text_button>("API", [this]() { show_settings_api_menu(); }));
	visuals->add_button(std::make_unique<text_button>("Character", [this]() { show_settings_character_menu(); }));
	visuals->add_button(std::make_unique<text_button>("User", [this]() { show_settings_user_menu(); }));
	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_main_menu(); }));
}
void character_logic::show_settings_application_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);

	// set buttons
	auto run_bg_btn = std::make_unique<toggle_button>(
		"Run in background: ",
		[this](bool toggled) {
			if (toggled) {
				visuals->show_message("Closing will now minimise the app. It can be reshown via the system tray.");
				config_->run_in_background = true;
			}
			else {
				config_->run_in_background = false;
			}
			config::save();
		},
		config_->run_in_background
	);
	visuals->add_button(std::move(run_bg_btn));
	auto run_boot_btn = std::make_unique<toggle_button>(
		"Run on boot: ",
		[this](bool toggled) {
			if (toggled) {
				if (!run_on_boot_helper::enable_run_on_boot()) {
					visuals->show_message("Failed to enable run on boot. Please try again.");
					show_settings_application_menu(); // revert visual state
					return;
				}
				visuals->show_message("Run on boot enabled, the application will now open at startup.");
				config_->start_on_boot = true;
			}
			else {
				if (!run_on_boot_helper::disable_run_on_boot()) {
					visuals->show_message("Failed to disable run on boot. Please try again.");
					show_settings_application_menu(); // revert visual state
					return;
				}
				config_->start_on_boot = false;
			}
			config::save();
		},
		config_->start_on_boot
	);
	visuals->add_button(std::move(run_boot_btn));
	auto auto_speed_btn = std::make_unique<number_button>(
		"Auto Speed: ",
		nullptr,
		1, 5,
		&config_->auto_mode_speed
	);
	auto_speed_btn->set_value_labels({ "Very slow", "Slow", "Normal", "Fast", "Very fast", });
	visuals->add_button(std::move(auto_speed_btn));
	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_menu(); }));
}
void character_logic::show_settings_discord_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);

	// set buttons
	auto enable_rpc_button = std::make_unique<toggle_button>(
		"Discord Rich Presence: ",
		[this](bool toggled) {
			if (toggled) {
				widget_->get_discord().enable();
				update_rpc();
			}
			else {
				widget_->get_discord().disable();
			}
			config::save();
		},
		&config_->rpc_enable
	);
	visuals->add_button(std::move(enable_rpc_button));

	auto display_character_button = std::make_unique<toggle_button>(
		"Display character: ",
		[this](bool toggled) {
			update_rpc();
			config::save();
		},
		&config_->rpc_display_current_character
	);
	display_character_button->set_enabled_ptr(&config_->rpc_enable); // can only toggle if rpc enabled
	visuals->add_button(std::move(display_character_button));

	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_menu(); }));
}
void character_logic::show_settings_api_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);

	std::string api_str = (config_->api == api::OPENAI) ? "OpenAI"
		: (config_->api == api::OPENROUTER) ? "OpenRouter"
		: "Custom";

	std::string message = "API: " + api_str + " | Model: " + config_->model + "\n";
	if (config_->api == api::CUSTOM) {
		std::string endpoint_display = config_->custom_endpoint.empty()
			? "[Not set]"
			: config_->custom_endpoint;
		message += "Endpoint: " + endpoint_display + "\n";
	}
	message += "Choose an option...";
	visuals->set_saying(message);

	// set buttons
	visuals->add_button(std::make_unique<text_button>("API Mode", [this]() {
		await_input_custom("Enter your API (OpenRouter/OpenAI/Custom): ", "",
			[this](bool success, const std::string& value) {
				if (success) {
					std::string api_lower = string_utils::to_lower(value);
					if (api_lower == "openai") {
						config_->api = api::OPENAI;
					}
					else if (api_lower == "openrouter") {
						config_->api = api::OPENROUTER;
					}
					else if (api_lower == "custom") {
						config_->api = api::CUSTOM;
					}
					else {
						visuals->show_message("Invalid API mode. Supported: "
							"'OpenAI', 'OpenRouter', 'Custom'.");
					}
					config::save(); // save config
				}
				show_settings_api_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("Model", [this]() {
		await_input_custom("Enter model: ", config_->model,
			[this](bool success, const std::string& value) {
				if (success) {
					config_->model = value;
					config::save(); // save config
				}
				show_settings_api_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("API Key", [this]() {
		await_input_custom("Enter API key: ", config_->api_key,
			[this](bool success, const std::string& value) {
				if (success) {
					config_->api_key = string_utils::trim(value);
					config::save(); // save config
				}
				show_settings_api_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("Token Limit", [this]() {
		await_input_custom("Enter Token Limit (0 for infinite): ", std::to_string(config_->max_tokens),
			[this](bool success, const std::string& value) {
				if (success) {
					try {
						int tokens = std::stoi(value);
						if (tokens <= 0) {
							visuals->show_message("Please enter a positive integer for token limit.");
						}
						else {
							if (tokens != 0 && tokens < 500) {
								visuals->show_message("Warning: Setting a very low token limit (<500) may cause bugs, it is not recommended.");
							}

							config_->max_tokens = tokens;
							config::save(); // save config
						}
					}
					catch (const std::exception&) {
						visuals->show_message("Invalid input. Please enter a valid integer for token limit.");
					}
				}
				show_settings_api_menu();
			});
		}));
	if (config_->api == api::CUSTOM) {
		visuals->add_button(std::make_unique<text_button>("Endpoint", [this]() {
			await_input_custom("Enter endpoint URL: ", config_->custom_endpoint,
				[this](bool success, const std::string& value) {
					if (success) {
						config_->custom_endpoint = value;
						config::save(); // save config
					}
					show_settings_api_menu();
				});
			}));
	}
	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_menu(); }));
}
void character_logic::show_settings_character_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	std::string current_ch = ddlc_character_to_string(character_);
	std::string message = "Current character: " + current_ch + "\n";
	message += "Current preset: " + config_->behaviour_preset + "\n";
	message += "Choose an option...";

	visuals->set_chars_per_second(100.0f);
	visuals->set_saying(message);

	// set buttons
	visuals->add_button(std::make_unique<text_button>("Switch character", [this]() { show_settings_character_change_menu(); }));
	visuals->add_button(std::make_unique<text_button>("Preset", [this]() {
		await_input_custom("Enter new behaviour preset: ", config_->behaviour_preset,
			[this](bool success, const std::string& value) {
				if (success) {
					if (supports_behaviour_preset(character_, value)) {
						visuals->show_popup("Warning: Changing the preset will "
							"reset all progress. Continue?",
							[this, value](int result) {
								if (result == 0) {
									config_->behaviour_preset = value;
									reset_fully();
								}
							});
					}
					else {
						std::string supported;
						auto presets = get_behaviour_presets(character_);
						for (size_t i = 0; i < presets.size(); i++) {
							supported += "[ " + presets[i];
							if (i < presets.size() - 1) {
								supported += "', ";
							}
							else {
								supported += " ]";
							}
						}
						visuals->show_message("Invalid behaviour preset. Supported: " + supported);
					}
					config::save(); // save config
				}
				show_settings_character_menu();
			});
		}));

	auto window_ctrl_btn = std::make_unique<toggle_button>(
		"Window control: ",
		[this](bool toggled) {
			config_->enable_window_controls = toggled;
			config::save();
		},
		config_->enable_window_controls
	);
	visuals->add_button(std::move(window_ctrl_btn));

	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_menu(); }));
}
void character_logic::show_settings_character_change_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	std::string current_ch = ddlc_character_to_string(character_);
	std::string message = "Select a new character...\n";
	message += "Current: " + current_ch;

	visuals->set_chars_per_second(100.0f);
	visuals->set_saying(message);

	auto try_set_character = [this](ddlc_character ch) {
		if (character_ == ch) {
			visuals->show_message("You cannot change to the same character.");
		}
		else {
			config_->character = ch;
			config::save();
			set_character(ch);
			reset();
		}
	};

	// set buttons
	visuals->add_button(std::make_unique<text_button>("Monika", [this, try_set_character]() {
		try_set_character(ddlc_character::MONIKA);
	}));
	visuals->add_button(std::make_unique<text_button>("Yuri", [this, try_set_character]() {
		try_set_character(ddlc_character::YURI);
	}));
	visuals->add_button(std::make_unique<text_button>("Natsuki", [this, try_set_character]() {
		try_set_character(ddlc_character::NATSUKI);
	}));
	visuals->add_button(std::make_unique<text_button>("Sayori", [this, try_set_character]() {
		try_set_character(ddlc_character::SAYORI);
	}));
	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_character_menu(); }));
}
void character_logic::show_settings_user_menu() {
	visuals->clear_buttons();
	current_menu_ = menu_state::SETTINGS;

	visuals->set_chars_per_second(100.0f);

	std::string message = "User name: " + config_->user_name +
		" | Language: " + config_->language + "\n";
	message += "Pronouns: " + config_->pronouns + "\n";
	message += "Choose an option...";
	visuals->set_saying(message);

	// set buttons
	visuals->add_button(std::make_unique<text_button>("Name", [this]() {
		await_input_custom("Enter your name: ", config_->user_name,
			[this](bool success, const std::string& value) {
				if (success) {
					config_->user_name = value;
					config::save(); // save config
				}
				show_settings_user_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("Pronouns", [this]() {
		await_input_custom("Enter your pronouns: ", config_->pronouns,
			[this](bool success, const std::string& value) {
				if (success) {
					config_->pronouns = value;
					config::save(); // save config
				}
				show_settings_user_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("Language", [this]() {
		await_input_custom("Enter your language (does not affect UI): ", config_->language,
			[this](bool success, const std::string& value) {
				if (success) {
					config_->language = value;
					config::save(); // save config
				}
				show_settings_user_menu();
			});
		}));
	visuals->add_button(std::make_unique<text_button>("Back", [this]() { show_settings_menu(); }));
}

void character_logic::await_choice(bool show_immediate) {
	int num_actions = (int)current_state.actions.size();
	std::string message = "Choose an action:\n";
	for (int i = 0; i < num_actions; i++) {
		message += std::to_string(i + 1) + ") " + current_state.actions[i] + "    ";
	}
	if (show_immediate) {
		visuals->set_saying_immediate(message);
	}
	else {
		visuals->set_saying(message);
	}

	custom_mode_ = false;
	state_ = logic_state::AWAITING_CHOICE;
}
void character_logic::await_input() {
	live_input_buffer_.clear();

	input::begin_input_recording(&live_input_buffer_, INPUT_MAX_LENGTH, [this]() {
		character_interaction input_interaction(character_interaction::kind::CUSTOM_MESSAGE);
		input_interaction.str_data = live_input_buffer_;
		input::end_input_recording();
		begin_think(input_interaction);
		});

	custom_mode_ = true;
	state_ = logic_state::AWAITING_INPUT;
}
void character_logic::await_input_custom(const std::string& prompt, const std::string& initial_value, const std::function<void(bool, const std::string&)>& callback) {
	if (settings_input_.active) {
		finish_settings_input(false);
	}

	settings_input_.active = true;
	settings_input_.prompt = prompt;
	settings_input_.buffer = initial_value;
	settings_input_.callback = callback;
	settings_input_.restore_state = state_;

	visuals->clear_buttons();
	visuals->add_button(std::make_unique<text_button>("Submit", [this]() { finish_settings_input(true); }));
	visuals->add_button(std::make_unique<text_button>("Clear", [this]() { input::buffer_clear(); }));
	visuals->add_button(std::make_unique<text_button>("Copy", [this]() { input::buffer_copy(); }));
	visuals->add_button(std::make_unique<text_button>("Paste", [this]() { input::buffer_paste(); }));
	visuals->add_button(std::make_unique<text_button>("Cancel", [this]() { finish_settings_input(false); } ));

	input::begin_input_recording(&settings_input_.buffer, INPUT_MAX_LENGTH, [this]() {
		finish_settings_input(true);
	});

	state_ = logic_state::AWAITING_INPUT_SETTINGS;
}
void character_logic::finish_settings_input(bool success) {
	if (!settings_input_.active) {
		return;
	}

	input::end_input_recording();

	std::string result = settings_input_.buffer;
	logic_state restore_state = settings_input_.restore_state;
	auto callback = std::move(settings_input_.callback);

	settings_input_ = settings_input_session{};
	state_ = restore_state;

	if (callback) {
		callback(success, result);
	}
}

float character_logic::get_auto_delay_sec() const {
	const std::unordered_map<int, float> delay_map = {
		{ 1, 8.0f },
		{ 2, 6.0f },
		{ 3, 3.0f },
		{ 4, 1.5f },
		{ 5, 0.75f },
	};

	return delay_map.at(std::clamp(config_->auto_mode_speed, 1, 5));
}

int character_logic::get_choice_input(int num_choices) {
	int choice = -1;

	if (input::is_key_pressed('1') && num_choices > 0) {
		choice = 0;
	}
	else if (input::is_key_pressed('2') && num_choices > 1) {
		choice = 1;
	}
	else if (input::is_key_pressed('3') && num_choices > 2) {
		choice = 2;
	}
	else if (input::is_key_pressed('4') && num_choices > 3) {
		choice = 3;
	}

	return choice;
}

void character_logic::reset() {
	ai->cancel();
	current_state.interactions.clear();
	interaction_index_ = 0;
	error_state_ = error_state::NONE;

	visuals->reset(character_);
	show_main_menu();

	first_tick_ = true;

	// start new interaction
	character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
	begin_think(interaction);
}
void character_logic::reset_fully(bool all_characters) {
	if (all_characters) {
		repo->reset_all();
	}
	else {
		repo->reset_channel((int)character_);
	}

	reset();
}

void character_logic::begin_think(const character_interaction& interaction) {
	input_mode_btn_disabled_ = true;
	display_think();
	ai->handle_interaction_async(interaction);
	state_ = logic_state::THINKING;
}

void character_logic::display_think() {
	// slow effect while thinking
	visuals->set_chars_per_second(2.0f);
	visuals->set_saying("... ");
}

void character_logic::display_current_interaction() {
	auto_timer_ = 0.0f;
	if (interaction_index_ < current_state.interactions.size()) {
		const auto& inter = current_state.interactions[interaction_index_];
		visuals->set_saying(inter.saying);

		visuals->set_expression(inter.expression);
		visuals->set_pose(inter.pose_left, inter.pose_right);

		// move self (if allowed)
		if (config_ && config_->enable_window_controls) {
			const int screen_width = sys::display_width();
			const int screen_height = sys::display_height();

			int min_height = static_cast<int>(screen_height * 0.5f);
			int max_height = static_cast<int>(screen_height * 0.95f);

			int new_x_px;
			if (inter.new_x == -1) {
				new_x_px = visuals->get_x();
			}
			else {
				int new_x = inter.new_x;
				if (new_x < 0)
					new_x = 0;
				else if (new_x > 100)
					new_x = 100;

				new_x_px = static_cast<int>((new_x / 100.0f) * screen_width);
			}

			// pre-apply scale
			if (inter.new_scale != -1) {
				int new_scale = inter.new_scale;
				if (new_scale > 10)
					new_scale = 10;
				else if (new_scale < 1)
					new_scale = 1;

				// scale from 1-10 to min_height-max_height
				float scale_factor = (new_scale - 1) / 9.0f; // normalize to 0.0-1.0
				new_scale = min_height +
					static_cast<int>(scale_factor * (max_height - min_height));

				visuals->set_scale(new_scale);
			}

			int scale = visuals->get_scale();

			// clamp position to screen bounds
			if (new_x_px < 0)
				new_x_px = 0;
			else if (new_x_px + scale > screen_width) {
				new_x_px = screen_width - scale;
			}

			// stick to the bottom of the screen
			int new_y = screen_height - scale;

			visuals->set_position(new_x_px, new_y);
		}
	}
}
void character_logic::advance_interaction() {
	if (++interaction_index_ < current_state.interactions.size()) {
		display_current_interaction();
	}
	else {
		// end of conversation
		interaction_index_ = 0;
		if (error_state_ == error_state::NON_FATAL) {
			reset();
		}
		else if (error_state_ == error_state::FATAL) {
			window_->close();
		}
		else if (in_setup_) {
			setup_step_++;
			show_setup(setup_step_);
		}
		else if (in_update_) {
			show_confirm_update();
		}
		else {
			if (!current_state.actions.empty()) {
				if (custom_mode_) {
					await_input();
				}
				else {
					await_choice();
				}
				input_mode_btn_disabled_ = false;
			}
			else {
				state_ = logic_state::IDLE;
				refresh_display();
			}
		}
	}
}

void character_logic::create_default_manifest() {
	json manifest_json;

	manifest_json["version"] = DDLC_DESKTOP_VERSION_STR;

	std::ofstream manifest_file("manifest.json");
	manifest_file << manifest_json.dump(4);
	manifest_file.close();
}

void character_logic::set_character(ddlc_character new_character, bool warn_preset) {
	character_ = new_character;

	if (!supports_behaviour_preset(character_, config_->behaviour_preset)) {
		if (warn_preset) visuals->show_message("Current behaviour preset '" + config_->behaviour_preset +
			"' is not supported by the new character. Reverting to default preset.");

		config_->behaviour_preset = "default";
	}

	// update RPC
	update_rpc();
}

void character_logic::update_rpc() {
	if (!config_->rpc_enable) {
		return;
	}

	// set status
	std::string status = "Hanging out with ";
	if (config_->rpc_display_current_character) {
		status += ddlc_character_to_string(character_) + "!";
	}
	else status += "the club!";

	discord_rpc& dc = widget_->get_discord();

	dc.set_status(status);
	dc.reset_start_time();
	dc.update_activity();
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

	return -1;
}

void character_logic::close() {
	if (config_->run_in_background) {
		window_->hide();
	}
	else {
		if (!in_setup_) {
			config::save();
			ai->handle_close_interaction();
		}
		window_->close();
	}
}
void character_logic::refresh_display() {
	switch (state_) {
	case logic_state::IDLE:
		visuals->set_chars_per_second(50.0f);
		visuals->set_saying("");
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::THINKING:
		display_think();
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::TALKING:
		visuals->set_chars_per_second(50.0f);
		display_current_interaction();
		input_mode_btn_disabled_ = true;
		break;

	case logic_state::AWAITING_CHOICE:
		await_choice(true);
		input_mode_btn_disabled_ = false;
		break;

	case logic_state::AWAITING_INPUT:
		// restore input mode
		custom_mode_ = true;
		input_mode_btn_disabled_ = false;

		// restart input recording
		input::begin_input_recording(&live_input_buffer_, INPUT_MAX_LENGTH, [this]() {
			character_interaction input_interaction(character_interaction::kind::CUSTOM_MESSAGE);
			input_interaction.str_data = live_input_buffer_;
			input::end_input_recording();
			begin_think(input_interaction);
		});

		visuals->set_saying_immediate("You: " + live_input_buffer_ + "_");
		break;
	}
}