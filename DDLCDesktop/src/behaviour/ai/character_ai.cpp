#include <behaviour/ai/character_ai.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <format>
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include <behaviour/ai/ai_api.h>
#include <behaviour/ai/system_prompts.h>
#include <behaviour/character_interaction.h>
#include <config/config.h>
#include <error/ddlcd_runtime_error.h>
#include <ddlc/characters.h>
#include <ddlc/poses.h>
#include <ddlc/expressions.h>
#include <output/log.h>

using json = nlohmann::json;

character_ai::character_ai() {
    config_ = config::get();

    api_ = new ai_api();

    // start worker thread
    stop_worker_.store(false, std::memory_order_relaxed);
    worker_ = std::thread(&character_ai::worker_loop, this);
}
character_ai::~character_ai() {
    request_cancel();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_worker_.store(true, std::memory_order_relaxed);
    }
    cv_.notify_all();

    if (worker_.joinable()) {
        worker_.join();
    }

    delete api_;
}

void character_ai::handle_close_interaction() {
    memory_->get_history().add_message("user", "[" + now_str() + "] " + get_user_name() + " closed " +
                            ddlc_character_to_string(config_->character) +
                            "'s window.");
}

void character_ai::handle_interaction_async(const character_interaction &interaction) {
    if (has_task_.load(std::memory_order_relaxed) || is_processing_.load(std::memory_order_relaxed)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);
        pending_interaction_ = interaction;
        has_task_.store(true, std::memory_order_relaxed);
        has_result_.store(false, std::memory_order_relaxed);
        cancel_requested_.store(false, std::memory_order_relaxed);
    }
    cv_.notify_one();
    }
    bool character_ai::is_response_ready() const {
    return has_result_.load(std::memory_order_relaxed);
    }
    character_state character_ai::get_response() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!has_result_) {
    return character_state{};
    }

    has_result_.store(false, std::memory_order_relaxed);
    is_processing_.store(false, std::memory_order_relaxed);
    return pending_result_;
    }

void character_ai::save_state() {
    memory_->save();
}
void character_ai::load_state() {
    memory_->load();
}
void character_ai::reset_state() {
    memory_->reset();
}
void character_ai::cancel_and_reset() {
    request_cancel();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        has_task_.store(false, std::memory_order_relaxed);
        has_result_.store(false, std::memory_order_relaxed);
        is_processing_.store(false, std::memory_order_relaxed);
    }
    reset_state();
}

std::string character_ai::get_user_name() const {
    if (config_->user_name.empty()) {
        return "The user";
    }
    return config_->user_name;
}

std::string character_ai::now_str() const {
    auto now = std::chrono::system_clock::now();
    return std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::floor<std::chrono::seconds>(now));
}

void character_ai::request_cancel() {
    cancel_requested_.store(true, std::memory_order_relaxed);

    if (api_) {
        api_->cancel();
    }
}

void character_ai::worker_loop() {
    while (true) {
        character_interaction interaction;

        // wait for work or stop
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() {
                return stop_worker_.load(std::memory_order_relaxed) ||
                    has_task_.load(std::memory_order_relaxed);
            });
            if (stop_worker_) {
                is_processing_.store(false, std::memory_order_relaxed);
                return;
            }

            interaction = pending_interaction_;
            has_task_.store(false, std::memory_order_relaxed);
            is_processing_.store(true, std::memory_order_relaxed);
        }

        if (cancel_requested_.load(std::memory_order_relaxed)) {
            is_processing_.store(false, std::memory_order_relaxed);
            continue;
        }

        if (!api_) {
            // no API configured; treat as empty response
            character_state empty_state{};
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pending_result_ = empty_state;
                has_result_.store(true, std::memory_order_relaxed);
                is_processing_.store(false, std::memory_order_relaxed);
            }
            continue;
        }

        character_state result{};
        try {
            result = handle_interaction_internal(interaction);
        } catch (...) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                pending_result_ = result;
                has_result_.store(true, std::memory_order_relaxed);
                is_processing_.store(false, std::memory_order_relaxed);
            }
            continue;
        }

        if (cancel_requested_.load(std::memory_order_relaxed)) {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                // clear processing and any pending result on late cancellation
                pending_result_ = character_state{};
                has_result_.store(false, std::memory_order_relaxed);
                is_processing_.store(false, std::memory_order_relaxed);
            }
            continue;
        }

        // store result
        {
            std::lock_guard<std::mutex> lock(mutex_);
            pending_result_ = result;
            has_result_.store(true, std::memory_order_relaxed);
            is_processing_.store(false, std::memory_order_relaxed);
        }
    }
}

std::string character_ai::get_endpoint() {
    switch (config_->api) {
    case api::OPENAI:
        return "https://api.openai.com/v1/responses";
    case api::OPENROUTER:
        return "https://openrouter.ai/api/v1/responses";
    case api::CUSTOM:
        return config_->custom_endpoint;
    default:
        return "https://api.openai.com/v1/responses";
    }
}

character_state character_ai::handle_interaction_internal(
    const character_interaction &interaction) {
    api_->set_api_key(config_->api_key);
    api_->set_endpoint(get_endpoint());

    std::string prompt = build_prompt(interaction);
    std::string response = api_->get_response(prompt);

#if defined(DEBUG) || defined(_DEBUG)
    log::print("Generated prompt: {}\nAI Response: {}\n\n", prompt, response);
#endif

    character_state fail_parse_state{};
    fail_parse_state.err = character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN;

    if (response_is_error(response)) {
        return fail_parse_state;
    }

    std::string content = extract_content_from_response(response);
    std::string json = extract_json(content);

    // add to conversation history
    if (!json.empty()) {
        memory_->get_history().add_message("assistant", json);
    }
    else return fail_parse_state;

    return parse_response(json);
}

std::string character_ai::build_prompt(const character_interaction &interaction) {
    json messages = json::array();

    // add system prompt
	messages.push_back({ { "role", "system" }, { "content", get_system_prompt() } });

    // add conversation history
    for (const auto &msg : memory_->get_history().get_history(config_->message_history_size)) {
        messages.push_back({ { "role", msg.role }, { "content", msg.content } });
    }

    std::string interaction_message = interaction_to_message(interaction);
        messages.push_back({ { "role", "user" }, { "content", interaction_message } });

    // add to history
    memory_->get_history().add_message("user", interaction_message);

    json payload;
    payload["model"] = config_->model;
    payload["input"] = messages;

    return payload.dump();
}
std::string character_ai::interaction_to_message(const character_interaction &interaction) {
    std::string user_name = get_user_name();
    std::string character_name = ddlc_character_to_string(config_->character);
    switch (interaction.get_kind()) {
    case character_interaction::kind::CLICK:
        return "[" + now_str() + "] " + user_name + " clicked " + character_name +
                ".";
    case character_interaction::kind::CHOICE_MADE:
        return "[" + now_str() + "] " + user_name + ": \"" + interaction.str_data +
                "\" (Choice " + std::to_string(interaction.int_data) + ")";
    case character_interaction::kind::CUSTOM_MESSAGE:
        return "[" + now_str() + "] " + user_name + ": \"" + interaction.str_data +
                "\"";
    case character_interaction::kind::WINDOW_OPEN:
        return "[" + now_str() + "] " + user_name + " opened " + character_name +
                "'s window.";
    default:
        return "[" + now_str() + "] " + user_name + " interacted with " +
                character_name +
                ", but an internal error occurred and we don't know what the "
                "interaction was.";
    }
}
std::string character_ai::extract_json(const std::string &str) {
    size_t beg = str.find('{');
    size_t end = str.rfind('}');
    if (beg != std::string::npos && end != std::string::npos && end > beg) {
        return str.substr(beg, end - beg + 1);
    }
    return "";
}
bool character_ai::response_is_error(const std::string &response) {
    try {
        auto j = json::parse(response);

        if (j.contains("error") && !j["error"].is_null()) {
            log::print("Response was an error: {}\n", j["error"].dump());
            return true;
        }
    }
    catch (...) {
        return true;
    }
    return false;
}
std::string
character_ai::extract_content_from_response(const std::string &response) {
    try {
        auto j = json::parse(response);

        if (j.contains("error") && !j["error"].is_null()) {
            return "";
        }

        // format: output[type == message] -> content[type == output_text] -> text
        json output_arr = j["output"];
        for (const auto &item : output_arr) {
            if (item["type"] == "message") {
                json content_arr = item["content"];
                for (const auto &content_item : content_arr) {
                    if (content_item["type"] == "output_text") {
                        return content_item["text"].get<std::string>();
                    }
                }
            }
        }
    }
    catch (...) {
    }

    return "";
}
character_state character_ai::parse_response(const std::string &raw_response) {
    character_state state{};

    try {
        auto j = json::parse(raw_response);

        // parse interactions
        for (auto &inter : j["interactions"]) {
			ddlc_character chr = config_->character;
            character_state::interaction i;
            i.saying = inter.value("saying", "");
            i.expression = ddlc::get_expression_code(chr, inter.value("expression", ""));
            i.pose_left = ddlc::get_pose_code_left(chr, inter.value("pose_left", ""));
            i.pose_right = ddlc::get_pose_code_right(chr, inter.value("pose_right", ""));
            i.new_x = inter.value("new_x", -1);         // default no change
            i.new_scale = inter.value("new_scale", -1); // default no change
            state.interactions.push_back(i);
        }

        // parse style + actions
        state.style = j.value("style", "normal");
        if (state.style != "normal" && state.style != "casual") {
            state.style = "normal"; // fallback
        }

        state.actions = j.value("actions", std::vector<std::string>{});
    } 
    catch (nlohmann::json::exception &e) {
        log::print("JSON parsing error in AI response: {}\n", e.what());

        state.err = character_state::error::FAIL_PARSE_RESPONSE_JSON;
    } catch (...) {
        state.err = character_state::error::FAIL_PARSE_RESPONSE_UNKNOWN;
    }

    return state;
}

std::string character_ai::get_system_prompt() const {
return system_prompts::get_prompt(config_->character, config_->behaviour_preset) +
        "\n" + "All dialogue MUST be in the following language: \"" + config_->language;
}
