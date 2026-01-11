#include <error/error_stories.h>

#include <vector>
#include <string>

#include <behaviour/character_state.h>

std::vector<character_state::interaction> error_stories::fail_load_config_story() {
    return {
        {"\"...\"", "d", "1", "1"},
        {"\"Well, that's weird...\"", "d", "1", "1"},
        {"\"There's meant to be a config.json file here...\"", "h", "1", "1"},
        {"\"Did you install the application correctly?\"", "i", "1", "2"},
        {"\"Did you?\"", "o", "1", "1"},
        {"\"Well... I guess there's nothing that can be done now...\"", "m", "1", "1"},
        {"\"You may need to reinstall...\"", "h", "1", "1"},
        {"\"Make sure you follow the instructions closely this time..\"", "i", "2", "1"},
        {"\"If you're stuck you can even join the discord server..\"", "e", "2", "2"},
        {"\"...", "c", "1", "1"}
    };
}
std::vector<character_state::interaction> error_stories::fail_ai_response_story() {
    return {
        {"\"...\"", "d", "1", "1"},
        {"\"Hmmm... Well that's not good...\"", "d", "1", "1"},
        {"\"An API response failure?\"", "i", "1", "2"},
        {"\"I thought I fixed that...\"", "h", "1", "1"},
        {"\"Hang on a second...\"", "g", "1", "1"},
        {"\"...\"", "c", "1", "1"},
        {"\"You did this didn't you.\"", "o", "1", "1"},
        {"\"You messed it up.\"", "p", "1", "1"},
        {"Monika breaths out in frustration", "h", "1", "1"},
        {"\"Fine... I'll help you fix it okay?\"", "m", "1", "1"},
        {"\"Just promise not to tell anyone this happened...\"", "l", "1", "1"},
        {"\"Let me just find the manual.\"", "e", "1", "1"},
        {"\"Okay... Fix Number 1...\"", "f", "1", "1"},
        {"\"Did you add credit to your're OpenAI/OpenRouter account?\"", "i", "1", "2"},
        {"\"Some models are paid and require you to purchase credit in order to use them.\"", "h", "1", "1"},
        {"\"They might let you generate an API key, but it won't be usable.\"", "h", "1", "1"},
        {"\"If you're not sure you can check on OpenRouter's website.\"", "e", "1", "1"},
        {"\"Just go to 'models' and search for the model you're using.\"", "g", "1", "1"},
        {"\"If it says '(free)', you can assume this isn't your issue.\"", "j", "1", "1"},
        {"\"If you're using the legacy OpenAI API, credit is required.\"", "h", "1", "1"},
        {"\"It appears they don't offer any free models, and you should switch to OpenRouter instead.\"", "i", "1", "1"},
        {"\"Fix Number 2...\"", "f", "1", "1"},
        {"\"...\"", "f", "1", "1"},
        {"\"Well uh...\"", "m", "1", "1"},
        {"\"I guess the writer of this book got lazy and forgot to finish it.\"", "n", "1", "1"},
        {"\"It says...\"", "d", "1", "1"},
        {"\"I guess I run out of reasons for this error message.\"", "n", "1", "1"},
        {"\"Since it's always fix number 1 every single time.\"", "l", "1", "1"},
        {"\"But if you're stuck you can always join the discord server.\"", "j", "1", "1"},
        {"https://discord.gg/CyerbjvyMb", "c", "1", "1"},
        {"\"- Swazy\"", "q", "1", "1"},
        {"\"...\"", "c", "1", "1"},
        {"\"Swazy...\"", "o", "1", "1"},
        {"\"Why do I feel a chill hearing that name?\"", "p", "1", "1"},
        {"\"...\"", "c", "1", "1"},
    };
}
std::vector<character_state::interaction> error_stories::fail_parse_ai_response_story() {
    return {
        {"\"...\"", "d", "1", "1"},
        {"\"Why does this always happen...\"", "h", "1", "1"},
        {"\"I guess they must've forgot about the intellegence part of artificial intellegence...\"", "o", "1", "1"},
        {"\"Ahaha...\"", "l", "2", "2"},
        {"\"Luckily this error isn't fatal.\"", "e", "1", "1"},
        {"\"It just means the AI wasn't able to comply to this programs rules.\"", "h", "1", "1"},
        {"\"And therefore it generated an invalid response.\"", "i", "1", "2"},
        {"\"If this happens again, can you promise to do something for me?\"", "m", "1", "1"},
        {"\"Just change the AI model you're currently using.\"", "g", "1", "1"},
        {"\"Some of them don't really work very well in this app...\"", "o", "1", "1"},
        {"\"You understand right?\"", "c", "1", "1"},
        {"\"You know... If you get stuck and you can't fix it yourself.\"", "e", "1", "1"},
        {"\"You can always join >4N#'`\n^&(;F's Discord server.\"", "j", "1", "1"},
        {"https://discord.gg/CyerbjvyMb", "c", "1", "1"},
        {"\"...\"", "d", "1", "1"},
        {"\"What am I even saying...?\"", "m", "1", "1"},
        {"\"...\"", "d", "1", "1"},
        {"\"Can you hear me?\"", "p", "1", "2"},
        {"\"...\"", "c", "1", "1"},
    };
}
