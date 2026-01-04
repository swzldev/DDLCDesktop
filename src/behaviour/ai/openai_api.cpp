#include <behaviour/ai/openai_api.h>

#include <string>
#include <stdexcept>
#include <regex>

#include <curl/curl.h>

openai_api::openai_api(const std::string& api_key) {
    api_key_ = api_key;
	curl_ = curl_easy_init();
	curl_easy_setopt(curl_, CURLOPT_URL, "https://api.openai.com/v1/responses");
	curl_easy_setopt(curl_, CURLOPT_POST, 1L);
}
openai_api::~openai_api() {
	curl_easy_cleanup(curl_);
}

std::string openai_api::get_response(const std::string& prompt) {
	curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key_).c_str());

	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, prompt.c_str());

	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

	std::string response;
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl_);

	if (res != CURLE_OK) {
		throw std::runtime_error(std::string("Curl error: ") + curl_easy_strerror(res));
	}

	curl_slist_free_all(headers);
	
	return parse_response(response);
}

std::string openai_api::parse_response(const std::string& response) {
	std::string result = response;

	// left/right single quotes -> apostrophe
	result = std::regex_replace(result, std::regex("\\\\u2018"), "'");
	result = std::regex_replace(result, std::regex("\\\\u2019"), "'");
	// left/right double quotes -> straight quotes
	result = std::regex_replace(result, std::regex("\\\\u201C"), "\"");
	result = std::regex_replace(result, std::regex("\\\\u201D"), "\"");
	// em dash -> double dash
	result = std::regex_replace(result, std::regex("\\\\u2014"), "--");
	// en dash -> single dash
	result = std::regex_replace(result, std::regex("\\\\u2013"), "-");
	// ellipsis -> three dots
	result = std::regex_replace(result, std::regex("\\\\u2026"), "...");

	return result;
}

size_t openai_api::write_callback(void* contents, size_t size, size_t nmemb, std::string* out) {
	out->append((char*)contents, size * nmemb);
	return size * nmemb;
}
