#pragma once

#include <string>
#include <atomic>

#include <curl/curl.h>

class ai_api {
public:
	ai_api();
	~ai_api();

	void set_endpoint(const std::string& endpoint);
	void set_api_key(const std::string& api_key);

	std::string get_response(const std::string& prompt);
	void cancel();

private:
	std::string endpoint_;
	std::string api_key_;
	CURL* curl_ = nullptr;

	std::atomic<bool> cancel_requested_{ false };

	std::string parse_response(const std::string& response);

	static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* out);
	static int progress_callback(void* clientp,
		curl_off_t dltotal, curl_off_t dlnow,
		curl_off_t ultotal, curl_off_t ulnow
	);
};