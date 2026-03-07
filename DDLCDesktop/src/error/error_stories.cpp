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
std::vector<character_state::interaction> error_stories::error_story(const std::string& user_name, const std::string& message) {
    return {
        {"\"...\"", "c", "1", "1"},
        {"\"" + user_name + "?\"", "p", "1", "1"},
        {"\"Well, this is a little embarrassing...\"", "m", "1", "1"},
        {"\"It's saying an error occurred, let me get you the details...\"", "f", "2", "1"},
        {"\"It says: '" + message + "'\"", "h", "1", "1"},
        {"\"If you need any help don't forget to join the discord server!\"", "e", "2", "2"},
        {"https://discord.gg/CyerbjvyMb", "j", "1", "1"},
    };
}