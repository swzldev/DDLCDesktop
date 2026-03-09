#pragma once

#include <memory>
#include <cstdint>

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
		update_activity();
	}
	inline void set_thumbnail(const std::string& thumbnail) {
		thumbnail_ = thumbnail;
		update_activity();
	}

private:
	std::unique_ptr<discordpp::Client> client_;
	std::string thumbnail_;
	std::string status_;

	void init();

	void update_activity();
};
