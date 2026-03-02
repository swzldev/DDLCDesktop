#pragma once

#include <string>

#include <curl/curl.h>

class requestor {
public:
	requestor();
	~requestor();

	std::string request(const std::string& url);

private:
	CURL* curl_;
};