#pragma once

class imenu_page {
public:
	virtual ~imenu_page() = default;

	virtual void on_page_enter() = 0;
	virtual void on_page_exit() = 0;
};