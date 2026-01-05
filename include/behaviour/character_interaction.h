#pragma once

class character_interaction {
public:
	enum class kind {
		CLICK,
		CHOICE_MADE,
		WINDOW_OPEN,
	};

public:
	character_interaction(kind k) : kind_(k) {}
	~character_interaction() = default;

	kind get_kind() const { return kind_; }

	std::string str_data;
	int int_data = 0;

private:
	kind kind_;
};