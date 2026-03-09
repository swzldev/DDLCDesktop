#pragma once

#include <memory>
#include <string>
#include <cstdint>
#include <ctime>

#include <discordpp.h>

#include <ddlc/characters.h>

class discord_rpc {
public:
	static constexpr uint64_t APPLICATION_ID = 1480639216869441758;

public:
	inline discord_rpc() { init(); }
	~discord_rpc() = default;

	inline static void run_discord_callbacks() {
		discordpp::RunCallbacks();
	}

	inline void set_status(const std::string& status) {
		status_ = status;
	}
	inline void set_thumbnail(const std::string& thumbnail) {
		thumbnail_ = thumbnail;
	}
	inline void reset_start_time() {
		start_time_ = std::time(nullptr);
	}

	void update_activity();

private:
	std::unique_ptr<discordpp::Client> client_;
	std::string thumbnail_;
	std::string status_;
	time_t start_time_ = 0;

	void init();
};
