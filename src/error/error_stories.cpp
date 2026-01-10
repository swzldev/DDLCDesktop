#include <error/error_stories.h>

#include <vector>
#include <string>

std::vector<character_state::interaction> error_stories::fail_load_config_story() {
    return {
        {"Well, that's weird...", "d", "1", "1"},
        {"There's meant to be a config.json file here...", "h", "1", "1"},
        {"Did you install the application correctly?", "i", "1", "2"},
        {"Did you?", "o", "1", "1"},
        {"Well... I guess there's nothing that can be done now...", "m", "1", "1"},
        {"You may need to reinstall...", "h", "1", "1"},
        {"Make sure you follow the instructions closely this time..", "i", "2", "1"},
        {"If you're stuck you can even join the discord server..", "e", "2", "2"},
        {"...", "c", "1", "1"}
    };
}
