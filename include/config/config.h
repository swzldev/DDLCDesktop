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

  // api
  api api;
  std::string api_key;
  std::string model;
  std::string
      custom_endpoint; // custom endpoint. this theoretically works with any api
                       // that is compatible with openai's endpoint bullsh
  int message_history_size;

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