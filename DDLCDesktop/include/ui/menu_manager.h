#pragma once

#include <memory>
#include <stack>

#include <ui/imenu_page.h>

class menu_manager {
public:
	menu_manager() = default;

	void push_page(std::unique_ptr<imenu_page> page);
	void pop_page();

	inline void handle_back() { pop_page(); }

private:
	std::stack<std::unique_ptr<imenu_page>> pages;
};