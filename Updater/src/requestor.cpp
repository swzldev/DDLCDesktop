#include <requestor.h>

#include <string>

#include <curl/curl.h>

static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t total_size = size * nmemb;
	std::string* response = static_cast<std::string*>(userp);
	response->append(static_cast<char*>(contents), total_size);
	return total_size;
}

requestor::requestor() {
	curl_ = curl_easy_init();
}
requestor::~requestor() {
	if (curl_) {
		curl_easy_cleanup(curl_);
	}
}

std::string requestor::request(const std::string& url) {
	if (!curl_) {
		return "";
	}

	std::string response;

	curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());

	// required for github api requests
	curl_easy_setopt(curl_, CURLOPT_USERAGENT, "DDLCDesktop-Updater");

	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

	curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);

	curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 30L);

	CURLcode res = curl_easy_perform(curl_);

	if (res != CURLE_OK) {
		return "";
	}
	return response;
}