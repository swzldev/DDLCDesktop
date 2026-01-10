#include <behaviour/ai/ai_api.h>

#include <string>
#include <stdexcept>
#include <vector>

#include <curl/curl.h>

ai_api::ai_api(const std::string& endpoint, const std::string& api_key) {
	endpoint_ = endpoint;
    api_key_ = api_key;
	curl_ = curl_easy_init();

	curl_easy_setopt(curl_, CURLOPT_URL, endpoint_.c_str());
	curl_easy_setopt(curl_, CURLOPT_POST, 1L);

	curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 10L);

	curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);
}
ai_api::~ai_api() {
	curl_easy_cleanup(curl_);
}

std::string ai_api::get_response(const std::string& prompt) {
	curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key_).c_str());

	headers = curl_slist_append(headers, "X-Title: DDLC Desktop");

	curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, prompt.c_str());

	curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);

	std::string response;
	curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response);

	CURLcode res = curl_easy_perform(curl_);

	// free BEFORE checking result
	curl_slist_free_all(headers);
	headers = nullptr;

	if (res != CURLE_OK) {
		throw std::runtime_error(std::string("Curl error: ") + curl_easy_strerror(res));
	}
	
	return parse_response(response);
}

std::string ai_api::parse_response(const std::string& response) {
	static const std::vector<std::pair<std::string, std::string>> unicode_replacements = {
		// quotes
		{"\\u2018", "'"},   // left single quote
		{"\\u2019", "'"},   // right single quote
		{"\\u201C", "\""},  // left double quote
		{"\\u201D", "\""},  // right double quote
		{"\\u00AB", "\""},  // left angle quote
		{"\\u00BB", "\""},  // right angle quote

		// dashes & hyphens
		{"\\u2014", "--"},  // em dash
		{"\\u2013", "-"},   // en dash
		{"\\u2011", "-"},   // non-breaking hyphen
		{"\\u2212", "-"},   // minus sign

		// ellipsis
		{"\\u2026", "..."}, // ellipsis

		// spaces & invisibles
		{"\\u00A0", " "},   // non-breaking space
		{"\\u202F", " "},   // narrow no-break space
		{"\\u200B", ""},    // zero-width space
		{"\\u200C", ""},    // zero-width non-joiner
		{"\\u200D", ""},    // zero-width joiner

		// bullets & dots
		{"\\u2022", "*"},   // bullet
		{"\\u00B7", "*"},   // middle dot

		// primes
		{"\\u2032", "'"},   // single prime
		{"\\u2033", "\""},  // double prime

		// symbols
		{"\\u2122", "(TM)"},// trademark
		{"\\u00A9", "(C)"}, // copyright
		{"\\u00AE", "(R)"}, // registered

		// math
		{"\\u00D7", "x"},   // multiplication
		{"\\u00F7", "/"}    // division
	};

	std::string result = response;

	auto replace_all = [](std::string& s, const std::string& from, const std::string& to) {
		if (from.empty()) return;

		size_t pos = 0;
		while ((pos = s.find(from, pos)) != std::string::npos)
		{
			s.replace(pos, from.length(), to);
			pos += to.length();
		}
	};

	for (const auto& [from, to] : unicode_replacements)
	{
		// some models return either the escaped or unescaped version
		replace_all(result, from, to);
		replace_all(result, "\\" + from, to);
	}

	return result;
}

size_t ai_api::write_callback(void* contents, size_t size, size_t nmemb, std::string* out) {
	out->append((char*)contents, size * nmemb);
	return size * nmemb;
}
