#include <behaviour/character_logic.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include <behaviour/ai/character_ai.h>
#include <behaviour/character_interaction.h>
#include <behaviour/character_state.h>
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

character_logic::character_logic(window *window) {
  window_ = window;

  if (!fs::exists("config.json")) {
    in_setup_ = true;
    setup_step_ = 0;
  }

  config_ = config::get();
  character_ = config_->character;

  // create visuals
  visuals = new character_visuals(window_->get_renderer(), character_);

  // permanent buttons
  visuals->add_button({"Close",
                       [this]() {
                         if (!in_setup_) {
                           config::save();
                           ai->handle_close_interaction();
                           ai->save_state("character_state.json");
                         }

                         window_->close();
                       }},
                      true);

  // create ai
  ai = new character_ai();

  if (fs::exists("character_state.json")) {
    ai->load_state("character_state.json");
  }

  window_->on_mouse_click.push_back([this]() {
    character_interaction interaction(character_interaction::kind::CLICK);
    handle_interaction(interaction);
    return 0;
  });
}
character_logic::~character_logic() {
  if (ai) {
    ai->save_state("character_state.json");
    delete ai;
    ai = nullptr;
  }
  if (visuals) {
    delete visuals;
    visuals = nullptr;
  }
}

void character_logic::handle_interaction(
    const character_interaction &interaction) {
  if (interaction.get_kind() != character_interaction::kind::CLICK ||
      current_menu_ == menu_state::SETTINGS) {
    return;
  }

  if (state_ == logic_state::IDLE) {
    begin_think(interaction);
  } else if (state_ == logic_state::TALKING && visuals->is_speaking()) {
    auto_mode_ = false;
    visuals->finish_speaking(); // mouse down while speaking -> quick skip
  } else if (state_ == logic_state::TALKING) {
    // in conversations, mouse down means advance interaction
    advance_interaction();
  }
}

void character_logic::tick(float delta_time) {
  if (paused_)
    return;

  if (first_tick_) {
    if (in_setup_) {
      show_setup(setup_step_);
    } else {
      show_main_menu();

      // window opened
      character_interaction interaction(
          character_interaction::kind::WINDOW_OPEN);
      begin_think(interaction);
    }

    first_tick_ = false;
  }

  if (visuals->in_popup()) {
    return;
  }

  if (current_menu_ != menu_state::MAIN) {
    if (state_ == logic_state::AWAITING_INPUT_SETTINGS) {
      // show current input
      std::string full = current_input_prompt_ + *current_input_ + "_";
      visuals->set_saying_immediate(full);
    }

    visuals->tick(delta_time);
    if (current_menu_ != menu_state::SETUP)
      return;
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
      } else
        visuals->set_style(current_state.style);

      // handle errors
      if (current_state.err != character_state::error::NONE) {
        switch (current_state.err) {
        case character_state::error::FAIL_PARSE_RESPONSE_JSON:
          handle_error(
              ddlcd_runtime_error(ddlcd_error::FAIL_PARSE_AI_RESPONSE,
                                  "Failed to parse AI response as JSON."));
          return;
        case character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN:
          handle_error(ddlcd_runtime_error(
              ddlcd_error::FAIL_PARSE_AI_RESPONSE,
              "Failed to parse AI response due to unknown format."));
          return;
        default:
          break;
        }
      }

      if (!current_state.interactions.empty()) {
        state_ = logic_state::TALKING;
        interaction_index_ = 0;
        display_current_interaction();
      } else {
        state_ = logic_state::IDLE;
        refresh_display();
      }
    }
  } else if (state_ == logic_state::TALKING) {
    if (!visuals->is_speaking() && auto_mode_) {
      auto_timer_ += delta_time;
    }
    if (auto_timer_ >= AUTO_MODE_DELAY_SEC) {
      auto_timer_ = 0.0f;
      advance_interaction();
    }
  } else if (state_ == logic_state::AWAITING_CHOICE) {
    int num_actions = current_state.actions.size();
    int choice = get_choice_input(num_actions);

    if (choice != -1) {
      // user made a choice
      character_interaction choice_interaction(
          character_interaction::kind::CHOICE_MADE);
      choice_interaction.str_data = current_state.actions[choice];
      choice_interaction.int_data = choice + 1;

      begin_think(choice_interaction);
    }
  } else if (state_ == logic_state::AWAITING_INPUT) {
    // show current input
    std::string full = "You: " + *current_input_ + "_";
    visuals->set_saying_immediate(full);
  }

  visuals->tick(delta_time);
}

void character_logic::handle_error(const ddlcd_runtime_error &error) {
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

  switch (error.kind) {
  case ddlcd_error::FAIL_OPEN_CONFIG:
    current_state.interactions = error_stories::fail_load_config_story();
    fatal = true;
    break;
  case ddlcd_error::FAIL_AI_RESPONSE:
    current_state.interactions = error_stories::fail_ai_response_story();
    fatal = true;
    break;
  case ddlcd_error::FAIL_PARSE_AI_RESPONSE:
    current_state.interactions = error_stories::fail_parse_ai_response_story();
    break;
  case ddlcd_error::OTHER:
    current_state.interactions.push_back(
        {"Uh oh! An error has occurred: " + error.message, "h", "1", "1"});
    break;
  }

  if (!fatal) {
    current_state.interactions.push_back(
        {"The application will now reset.", "a", "1", "1"});
  }

  error_state_ = fatal ? error_state::FATAL : error_state::NON_FATAL;
  state_ = logic_state::TALKING;

  interaction_index_ = 0;
  display_current_interaction();
}

void character_logic::show_setup(unsigned int step) {
  visuals->set_chars_per_second(30.0f);

  visuals->clear_buttons();

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
  } else if (step == 1) {
    // collect name
    await_input_custom("What's your name?: ", &config_->user_name,
                       [this](bool success) {
                         if (success && !config_->user_name.empty()) {
                           setup_step_++;
                           show_setup(setup_step_);
                         } else {
                           visuals->show_message("Please enter a valid name.");
                           show_setup(setup_step_); // retry
                         }
                       });
  } else if (step == 2) {
    // collect pronouns
    await_input_custom(
        "What are your pronouns?: ", &config_->pronouns, [this](bool success) {
          if (success && !config_->pronouns.empty()) {
            setup_step_++;
            show_setup(setup_step_);
          } else {
            visuals->show_message("Please enter valid pronouns.");
            show_setup(setup_step_); // retry
          }
        });
  } else if (step == 3) {
    // part 2
    current_state.interactions =
        error_stories::firstrun_story_p2(config_->user_name);
    state_ = logic_state::TALKING;
    interaction_index_ = 0;
    display_current_interaction();
  } else if (step == 4) {
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
  } else if (step == 5) {
    // part 3
    current_state.interactions = error_stories::firstrun_story_p3();
    state_ = logic_state::TALKING;
    interaction_index_ = 0;
    display_current_interaction();
  } else if (step == 6) {
    // collect api key
    await_input_custom("Enter your OpenRouter API key: ", &config_->api_key,
                       [this](bool success) {
                         if (success && !config_->api_key.empty()) {
                           setup_step_++;
                           show_setup(setup_step_);
                         } else {
                           visuals->show_message(
                               "Please enter a valid API key.");
                           show_setup(setup_step_); // retry
                         }
                       });
  } else if (step == 7) {
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
  } else {
    // done
    in_setup_ = false;
    config::save();
    reset_all();
  }
}

void character_logic::show_main_menu() {
  visuals->clear_buttons();
  current_menu_ = menu_state::MAIN;

  // set buttons
  visuals->add_button({
      "Auto",
      [this]() { auto_mode_ = true; },
      button_style::LABEL,
      button_type::TOGGLE,
      "",
      [this]() { auto_mode_ = false; },
  });
  visuals->add_button({"Reset", [this]() {
                         if (state_ == logic_state::THINKING) {
                           return; // dont reset while thinking
                         }
                         reset_all();
                       }});
  visuals->add_button({"Custom", [this]() { await_input(); },
                       button_style::LABEL, button_type::SWAP, "Actions",
                       [this]() { await_choice(/*true*/); },
                       &input_mode_btn_disabled_});
  visuals->add_button({"Settings", [this]() { show_settings_menu(); }});

  refresh_display();
}
void character_logic::show_settings_menu() {
  visuals->clear_buttons();
  current_menu_ = menu_state::SETTINGS;

  visuals->set_chars_per_second(100.0f);
  visuals->set_saying("Choose an option...");

  // set buttons
  visuals->add_button({"API", [this]() { show_settings_api_menu(); }});
  visuals->add_button(
      {"Character", [this]() { show_settings_character_menu(); }});
  visuals->add_button({"User", [this]() { show_settings_user_menu(); }});
  visuals->add_button({"Back", [this]() { show_main_menu(); }});
}
void character_logic::show_settings_api_menu() {
  visuals->clear_buttons();
  current_menu_ = menu_state::SETTINGS;

  visuals->set_chars_per_second(100.0f);

  std::string api_str = (config_->api == api::OPENAI)       ? "OpenAI"
                        : (config_->api == api::OPENROUTER) ? "OpenRouter"
                                                            : "Custom";

  std::string message =
      "API: " + api_str + " | Model: " + config_->model + "\n";
  if (config_->api == api::CUSTOM) {
    std::string endpoint_display = config_->custom_endpoint.empty()
                                       ? "[Not set]"
                                       : config_->custom_endpoint;
    message += "Endpoint: " + endpoint_display + "\n";
  }
  message += "API Key: [Hidden]\n";
  message += "Choose an option...";
  visuals->set_saying(message);

  // set buttons
  visuals->add_button(
      {"API Mode", [this]() {
         std::string *new_api = new std::string();
         await_input_custom(
             "Enter your API (OpenRouter/OpenAI/Custom): ", new_api,
             [this, new_api](bool success) {
               if (success) {
                 std::string new_api_lower = string_utils::to_lower(*new_api);
                 if (new_api_lower == "openai") {
                   config_->api = api::OPENAI;
                 } else if (new_api_lower == "openrouter") {
                   config_->api = api::OPENROUTER;
                 } else if (new_api_lower == "custom") {
                   config_->api = api::CUSTOM;
                 } else {
                   visuals->show_message("Invalid API mode. Supported: "
                                         "'OpenAI', 'OpenRouter', 'Custom'.");
                 }
                 config::save(); // save config
               }
               delete new_api;
               show_settings_api_menu();
             });
       }});
  visuals->add_button({"Model", [this]() {
                         await_input_custom("Enter model: ", &config_->model,
                                            [this](bool success) {
                                              if (success) {
                                                config::save(); // save config
                                              }
                                              show_settings_api_menu();
                                            });
                       }});
  visuals->add_button({"API Key", [this]() {
                         await_input_custom(
                             "Enter API key: ", &config_->api_key,
                             [this](bool success) {
                               if (success) {
                                 config::save(); // save config
                               }
                               show_settings_api_menu();
                             });
                       }});
  if (config_->api == api::CUSTOM) {
    visuals->add_button(
        {"Endpoint", [this]() {
           await_input_custom("Enter endpoint URL: ", &config_->custom_endpoint,
                              [this](bool success) {
                                if (success) {
                                  config::save(); // save config
                                }
                                show_settings_api_menu();
                              });
         }});
  }
  visuals->add_button({"Back", [this]() { show_settings_menu(); }});
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
  visuals->add_button({"Switch character",
                       [this]() { show_settings_character_change_menu(); }});
  visuals->add_button(
      {"Preset", [this]() {
         std::string *new_preset = new std::string();
         await_input_custom(
             "Enter new behaviour preset: ", new_preset,
             [this, new_preset](bool success) {
               if (success) {
                 if (supports_behaviour_preset(character_, *new_preset)) {
                   visuals->show_popup("Warning: Changing the preset will "
                                       "reset all progress. Continue?",
                                       [this, new_preset](int result) {
                                         if (result == 0) {
                                           config_->behaviour_preset =
                                               *new_preset;
                                           reset_all();
                                         }
                                       });
                 } else {
                   std::string supported;
                   auto presets = get_behaviour_presets(character_);
                   for (size_t i = 0; i < presets.size(); i++) {
                     supported += "'" + presets[i];
                     if (i < presets.size() - 1) {
                       supported += "', ";
                     } else {
                       supported += "'";
                     }
                   }
                   visuals->show_message(
                       "Invalid behaviour preset. Supported: " + supported);
                 }
                 config::save(); // save config
               }
               delete new_preset;
               show_settings_character_menu();
             });
       }});
  visuals->add_button(
      {"Window control: ON",
       [this]() {
         config_->enable_window_controls = false;
         config::save();
       },
       button_style::LABEL, button_type::SWAP, "Window control: OFF",
       [this]() {
         config_->enable_window_controls = true;
         config::save();
       },
       nullptr, nullptr, false, !config_->enable_window_controls});
  visuals->add_button({"Back", [this]() { show_settings_menu(); }});
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
    } else
      visuals->show_popup(
          "Warning: Changing the character will reset all progress. Continue?",
          [this, ch](int result) {
            if (result == 0) {
              // user confirmed
              set_character(ch);
            }
          });
  };

  // set buttons
  visuals->add_button({"Monika", [this, try_set_character]() {
                         try_set_character(ddlc_character::MONIKA);
                       }});
  visuals->add_button({"Yuri", [this, try_set_character]() {
                         try_set_character(ddlc_character::YURI);
                       }});
  visuals->add_button({"Natsuki", [this, try_set_character]() {
                         try_set_character(ddlc_character::NATSUKI);
                       }});
  visuals->add_button({"Sayori", [this, try_set_character]() {
                         try_set_character(ddlc_character::SAYORI);
                       }});
  visuals->add_button({"Back", [this]() { show_settings_character_menu(); }});
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
  visuals->add_button({"Name", [this]() {
                         await_input_custom(
                             "Enter your name: ", &config_->user_name,
                             [this](bool success) {
                               if (success)
                                 config::save(); // save config
                               show_settings_user_menu();
                             });
                       }});
  visuals->add_button({"Pronouns", [this]() {
                         await_input_custom(
                             "Enter your pronouns: ", &config_->pronouns,
                             [this](bool success) {
                               if (success)
                                 config::save(); // save config
                               show_settings_user_menu();
                             });
                       }});
  visuals->add_button({"Language", [this]() {
                         await_input_custom(
                             "Enter your language (does not affect UI): ",
                             &config_->language, [this](bool success) {
                               if (success)
                                 config::save(); // save config
                               show_settings_user_menu();
                             });
                       }});
  visuals->add_button({"Back", [this]() { show_settings_menu(); }});
}

void character_logic::await_choice(bool show_immediate) {
  int num_actions = current_state.actions.size();
  std::string message = "Choose an action:\n";
  for (int i = 0; i < num_actions; i++) {
    message += std::to_string(i + 1) + ") " + current_state.actions[i] + "    ";
  }
  if (show_immediate) {
    visuals->set_saying_immediate(message);
  } else {
    visuals->set_saying(message);
  }

  custom_mode_ = false;
  state_ = logic_state::AWAITING_CHOICE;
}
void character_logic::await_input() {
  current_input_ = new std::string();

  // start recording
  input::begin_input_recording(current_input_, INPUT_MAX_LENGTH, [this]() {
    // on submit
    character_interaction input_interaction(
        character_interaction::kind::CUSTOM_MESSAGE);
    input_interaction.str_data = *current_input_;
    if (current_input_) {
      delete current_input_;
      current_input_ = nullptr;
    }
    input::end_input_recording();
    begin_think(input_interaction);
  });

  custom_mode_ = true;
  state_ = logic_state::AWAITING_INPUT;
}
void character_logic::await_input_custom(
    const std::string &prompt, std::string *value,
    const std::function<void(bool)> &callback) {
  current_input_prompt_ = prompt;
  current_input_ = new std::string(*value);

  logic_state original_state = state_;
  auto cleanup = [this, callback, original_state](bool success) {
    delete current_input_;
    input::end_input_recording();
    state_ = original_state;
    if (callback)
      callback(success);
  };

  visuals->clear_buttons();
  visuals->add_button({"Clear", [this]() { *current_input_ = ""; }});
  visuals->add_button(
      {"Copy", [this]() { input::set_clipboard_text(*current_input_); }});
  visuals->add_button({"Paste", [this]() {
                         std::string clip = input::get_clipboard_text();
                         for (char c : clip) {
                           if (current_input_->length() < INPUT_MAX_LENGTH) {
                             current_input_->push_back(c);
                           }
                         }
                       }});
  visuals->add_button({"Cancel", [this, cleanup]() {
                         // on cancel
                         cleanup(false);
                       }});

  // start recording
  input::begin_input_recording(current_input_, INPUT_MAX_LENGTH,
                               [this, value, cleanup]() {
                                 // on submit
                                 *value = *current_input_;
                                 cleanup(true);
                               });

  state_ = logic_state::AWAITING_INPUT_SETTINGS;
}

int character_logic::get_choice_input(int num_choices) {
  int choice = -1;

  if (input::is_key_pressed('1') && num_choices > 0) {
    choice = 0;
  } else if (input::is_key_pressed('2') && num_choices > 1) {
    choice = 1;
  } else if (input::is_key_pressed('3') && num_choices > 2) {
    choice = 2;
  } else if (input::is_key_pressed('4') && num_choices > 3) {
    choice = 3;
  }

  return choice;
}

void character_logic::reset_all() {
  // cleanup
  if (fs::exists("character_state.json")) {
    fs::remove("character_state.json");
  }

  ai->cancel_and_reset();
  current_state.interactions.clear();
  interaction_index_ = 0;
  error_state_ = error_state::NONE;

  visuals->reset(character_);
  show_main_menu();

  // start new interaction
  character_interaction interaction(character_interaction::kind::WINDOW_OPEN);
  begin_think(interaction);
}

void character_logic::begin_think(const character_interaction &interaction) {
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
    const auto &inter = current_state.interactions[interaction_index_];
    visuals->set_saying(inter.saying);

    visuals->set_expression(inter.expression);
    visuals->set_pose(inter.pose_left, inter.pose_right);

    // move self (if allowed)
    if (config_ && config_->enable_window_controls) {
      const int screen_width = sys::display_width();
      const int screen_height = sys::display_height();

      int min_height = screen_height * 0.5f;
      int max_height = screen_height * 0.95f;

      int new_x_px;
      if (inter.new_x == -1) {
        new_x_px = visuals->get_x();
      } else {
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
  } else {
    // end of conversation
    interaction_index_ = 0;
    if (error_state_ == error_state::NON_FATAL) {
      reset_all();
    } else if (error_state_ == error_state::FATAL) {
      window_->close();
    } else if (in_setup_) {
      setup_step_++;
      show_setup(setup_step_);
    } else {
      if (!current_state.actions.empty()) {
        if (custom_mode_) {
          await_input();
        } else {
          await_choice();
        }
        input_mode_btn_disabled_ = false;
      } else {
        state_ = logic_state::IDLE;
        refresh_display();
      }
    }
  }
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
    input::begin_input_recording(current_input_, INPUT_MAX_LENGTH, [this]() {
      character_interaction input_interaction(
          character_interaction::kind::CUSTOM_MESSAGE);
      input_interaction.str_data = *current_input_;
      if (current_input_) {
        delete current_input_;
        current_input_ = nullptr;
      }
      input::end_input_recording();
      begin_think(input_interaction);
    });

    std::string full = "You: " + *current_input_ + "_";
    visuals->set_saying_immediate(full);
    break;
  }
}
