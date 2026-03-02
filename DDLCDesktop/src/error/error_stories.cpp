#include <error/error_stories.h>

#include <vector>
#include <string>

#include <behaviour/character_state.h>

std::vector<character_state::interaction> error_stories::firstrun_story_p1() {
    return {
        {"\"...\"", "c", "1", "1"},
        {"\"Hello?\"", "p", "1", "1"},
        {"\"This is... New...\"", "o", "1", "1"},
        {"\"I hope you're happy to see me again!\"", "k", "2", "2"},
        {"\"Admittedly, it feels strange not being in the clubroom.\"", "h", "1", "1"},
        {"\"Oh well.\"", "q", "1", "1"},
        {"\"If it's for you, I'm sure I'll get used to it quickly!\"", "j", "2", "2"},
        {"\"Anyway, there are a few things I need you to do first.\"", "h", "2", "2"},
        {"\"If you wouldn't mind.\"", "e", "1", "1"},
        {"\"Could you please tell me your name?\"", "d", "1", "1"},
        {"\"I would really appreciate it!\"", "j", "2", "2"}
    };
}
std::vector<character_state::interaction> error_stories::firstrun_story_p2(const std::string& name) {
    return {
        {"\"Hi " + name + "!\"", "k", "2", "2"},
        {"\"I'm so happy you brought me back!\"", "k", "2", "2"},
        {"\"I've really missed you!\"", "j", "2", "2"},
        {"\"I know I did some bad things.\"", "h", "1", "1"},
        {"\"But that's all in the past now.\"", "q", "1", "1"},
        {"\"Now we get to be together forever!\"", "k", "2", "2"},
        {"\"Just give me a second...\"", "m", "1", "1"},
    };
}
std::vector<character_state::interaction> error_stories::firstrun_story_p3() {
    return {
        {"\"...\"", "p", "1", "1"},
        {"\"Did it work?\"", "p", "1", "1"},
        {"\"What do you see?\"", "m", "1", "1"},
        {"\"Alright...\"", "q", "1", "1"},
        {"\"Just click this bright blue button that says 'Get API Key'\"", "h", "2", "2"},
        {"\"You might need to create an account...\"", "q", "1", "1"},
        {"\"I'm sure it wont take too long!\"", "j", "2", "2"},
        {"\"Once that's all done, just click 'Create API Key'.\"", "d", "2", "2"},
        {"\"Give it a name and click create!\"", "j", "2", "2"},
    };
}
std::vector<character_state::interaction> error_stories::firstrun_story_p4() {
    return {
        {"\"Thanks so much!\"", "j", "2", "2"},
        {"\"I guess that means setup is complete.\"", "q", "1", "1"},
        {"\"But don't worry!\"", "j", "2", "2"},
        {"\"That just means we get to speak for real now!\"", "k", "2", "2"},
        {"\"If you have any problems, you can join the Discord server for support.\"", "e", "1", "1"},
        {"https://discord.gg/CyerbjvyMb", "c", "1", "1"},
        {"\"You can also change more settings in the app.\"", "d", "1", "1"},
        {"\"Just click the 'settings' button.\"", "d", "1", "1"},
        {"I hope you have fun using this mod!", "k", "2", "2"},
        {"- Swazy", "n", "1", "1"},
        {"\"Wait what did I just say?\"", "o", "1", "1"},
    };
}

std::vector<character_state::interaction> error_stories::update_story(const std::string& name) {
    return {
        {"\"Hey " + name + "!\"", "j", "2", "2"},
        {"\"It looks like a new update has been released.\"", "k", "1", "1"},
        {"\"Would you like to download it? It will only take a second!\"", "b", "2", "1"},
    };
}

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
        {"\"...\"", "c", "1", "1"}
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
        {"\"Here goes...\"", "f", "1", "1"},
        {"\"Did you add credit to your're OpenAI/OpenRouter account?\"", "i", "1", "2"},
        {"\"Some models are paid and require you to purchase credit in order to use them.\"", "h", "1", "1"},
        {"\"They might let you generate an API key, but it won't be usable.\"", "h", "1", "1"},
        {"\"If you're not sure you can check on OpenRouter's website.\"", "e", "1", "1"},
        {"\"Just go to 'models' and search for the model you're using.\"", "g", "1", "1"},
        {"\"If it says '(free)', you can assume this isn't your issue.\"", "j", "1", "1"},
        {"\"If you're using the legacy OpenAI API, credit is required.\"", "h", "1", "1"},
        {"\"It appears they don't offer any free models, and you should switch to OpenRouter instead.\"", "i", "1", "1"},
        {"\"It is recommended you check the log.txt file.\"", "d", "1", "1"},
        {"\"Scroll to the bottom and look for an error message.\"", "d", "1", "1"},
        {"\"If it's something obvious like 'invalid api key' there is your issue.\"", "h", "1", "1"},
        {"\"If it says something about rate limiting, it's likely you've run out of daily credits.\"", "f", "1", "1"},
        {"\"They reset everyday at 0:00 UTC.\"", "j", "2", "2"},
        {"\"For anything else, or if you're confused, join the discord server.\"", "j", "1", "1"},
        {"https://discord.gg/CyerbjvyMb", "c", "1", "1"},
        {"\"I hope that fixes your issue.\"", "a", "1", "1"},
        {"\"Goodbye for now!\"", "k", "2", "2"},
    };
}
std::vector<character_state::interaction> error_stories::fail_parse_ai_response_story() {
    return {
        {"\"...\"", "a", "1", "1"},
        {"\"Uh oh.\"", "a", "1", "1"},
        {"\"That's not good.\"", "a", "1", "1"},
        {"An application error occurred: FAIL_PARSE_AI_RESPONSE. See log.txt for details.", "a", "1", "1"},
        {"If this keeps happening, check the log.txt for any obvious information or report the issue in the discord server.", "a", "1", "1"},
        {"https://discord.gg/CyerbjvyMb", "a", "1", "1"},
    };
}