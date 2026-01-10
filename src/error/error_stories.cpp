#include <error/error_stories.h>

#include <vector>
#include <string>

std::vector<std::string> error_stories::fail_load_config_story() {
    return {
        "Well, that's weird...",
        "There's meant to be a config.json file here...",
        "Did you install the application correctly?",
        "Did you?",
        "Well... I guess there's nothing that can be done now...",
        "You may need to reinstall...",
        "Make sure you follow the instructions closely this time..",
        "If you're stuck you can even join the discord server..",
        "..."
	};
}
