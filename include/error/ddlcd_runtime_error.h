#pragma once

#include <exception>
#include <string>

enum class ddlcd_error {
	FAIL_OPEN_CONFIG,
	OTHER,
};

class ddlcd_runtime_error : public std::exception {
public:
	ddlcd_runtime_error(ddlcd_error kind, const std::string& message = "")
		: kind(kind), message(message) {
	}

	const char* what() const noexcept override {
		return message.c_str();
	}

	ddlcd_error kind;
	std::string message;
};