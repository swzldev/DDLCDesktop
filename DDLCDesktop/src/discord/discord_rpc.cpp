#include <discord/discord_rpc.h>

#include <string>
#include <cstdint>
#include <ctime>

#define DISCORDPP_IMPLEMENTATION
#include <discordpp.h>

#include <output/log.h>

void discord_rpc::update_activity() {
	if (!client_) {
		return;
	}

	discordpp::Activity activity;

	// basic details
	activity.SetType(discordpp::ActivityTypes::Playing);
	activity.SetName("DDLC Desktop");
	if (status_.length() < 2) {
		activity.SetState("Idle");
	}
	else activity.SetState(status_);

	// assets
	discordpp::ActivityAssets assets;
	if (thumbnail_.empty()) {
		assets.SetLargeImage("generic");
	}
	else assets.SetLargeImage(thumbnail_);

	activity.SetAssets(assets);

	// timestamps
	discordpp::ActivityTimestamps timestamps;
	timestamps.SetStart(start_time_);

	activity.SetTimestamps(timestamps);

	// client update
	log::print("[Discord RPC] Updating activity with status: {}", status_);
	client_->UpdateRichPresence(activity, [](discordpp::ClientResult result) {
		if (result.Successful()) {
			log::print("[Discord RPC] Activity updated!");
		}
		else {
			log::print("[Discord RPC] Failed to update activity: {}", result.Error());
		}
	});
}

void discord_rpc::init() {
	// create client
	client_ = std::make_unique<discordpp::Client>();
	client_->SetApplicationId(APPLICATION_ID);

	// setup log callback
	client_->AddLogCallback([](auto message, auto severity) {
		log::print("[Discord RPC] {}: {}", discordpp::EnumToString(severity), message);
		}, discordpp::LoggingSeverity::Verbose);

	start_time_ = std::time(nullptr);
	update_activity();
}
