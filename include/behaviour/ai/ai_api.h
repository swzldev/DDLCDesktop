#pragma once

#include <string>

#include <curl/curl.h>

class ai_api {
public:
	ai_api(const std::string& endpoint, const std::string& api_key);
	~ai_api();

	std::string get_response(const std::string& prompt);

private:
	std::string endpoint_;
	std::string api_key_;
	CURL* curl_ = nullptr;

	std::string parse_response(const std::string& response);

	size_t static write_callback(void* contents, size_t size, size_t nmemb, std::string* out);
};