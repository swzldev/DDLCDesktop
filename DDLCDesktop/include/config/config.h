#pragma once

#include <memory>
#include <string>

#include <ddlc/characters.h>

enum class api {
  OPENAI,
  OPENROUTER,
  CUSTOM,
};

class config {
public:
  static void load();
  static bool save();
  static config *get();

  // application
  bool start_on_boot;
  bool run_in_background;
  int auto_mode_speed;
  bool enable_discord_rpc;

  // api
  api api;
  std::string api_key;
  std::string model;
  std::string custom_endpoint;
  int message_history_size;
  int max_tokens;

  // behaviour
  std::string pronouns;
  std::string user_name;
  std::string language;
  std::string behaviour_preset;
  ddlc_character character;

  // visuals
  bool enable_window_controls = true;

private:
  static std::unique_ptr<config> loaded_;

  static void load_default();
};