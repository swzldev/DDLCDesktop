<div align="center">
  <img src="./assets/logo-circle.png" alt="DDLC Desktop logo" height="230" />
  <h1>DDLC Desktop</h1>
  <p>Ever wondered what it would be like to have your own DDLC character on your desktop? Well now you can!</p>
  <p>
    <img src="https://img.shields.io/badge/status-stable-green" alt="Status: Stable" />
    <img src="https://img.shields.io/github/v/release/swzldev/DDLCDesktop" alt="Latest release" />
  </p>

  <p>
    <a href="https://discord.gg/CyerbjvyMb">Discord</a> |
    <a href="#features">Features</a> |
    <a href="#installation">Installation</a> |
    <a href="#controls">Controls</a> |
    <a href="#issues">Issues</a> |
    <a href="#changelog">Changelog</a> |
    <a href="#contributing">Contributing</a> |
    <a href="#license">License</a>
  </p>
</div>

> [!IMPORTANT]
> Disclaimer: This is an unofficial, fan-made application and is not affiliated with, endorsed by, or approved by Team Salvato.
> *Doki Doki Literature Club* and all related characters, assets, and trademarks are © Team Salvato.
> This application does not include or distribute any copyrighted assets. Users must own a legitimate copy of Doki Doki Literature Club and provide their own game files.

## Features
- An interactive DDLC character that sits on your desktop and talks to you.
- Real AI interactions using the `v1/responses` (OpenAI) API request format.
- Talk to them like in the game with a set of options or a custom message.

## Installation
As of v2.0.0, installation is super simple and only takes a few steps.

1) Download the latest release of the installer [here](https://github.com/swzldev/DDLCDesktop/releases). (You'll want the one called DDLCDesktop-OfflineInstaller-vX.X.X-x64.exe)
2) Run the installer -- you may need to disable your antivirus, don't worry, it's completely safe, it only appears because the application is not signed. Signing it would cost me [upwards of £500 a year](https://www.digicert.com/signing/code-signing-certificates#code_signing_key_locker).
3) Follow the instructions on screen, make sure you download it to an appropriate path, the default is usually fine, but you should be careful about putting it into any restricted folders such as Program Files or OneDrive -- many people have had issues due to this.

### Models
The model you choose has a big impact on how the character behaves. Not all models work effectively, and some can cause crashes if used outside their intended purpose (e.g., using a model designed for coding/reasoning/image generation). Below are some models that have been tested and work well. (Model names may differ depending on your chosen API.)

#### OpenRouter
- `"meta-llama/llama-3.3-70b-instruct:free"` - Probably the best free model, works very well.
- `"openai/gpt-4o-mini"` - Good general purpose model, cheap and effective.
- `"openai/gpt-4.1"` - My personal favourite, realistic responses but slightly more expensive.

#### OpenAI
- `"gpt-4o-mini"` - Good general purpose model, cheap and effective.
- `"gpt-4.1"` - My personal favourite, realistic responses but slightly more expensive.

> [!TIP]
> If you've found a model that works well, please let me know on the discord so I can update the documentation!

### Custom endpoints
If you want to use a local LLM server or other OpenAI-compatible API, set `"api": "custom"` and configure your endpoint URL via the in-app Settings menu (Settings -> API -> Endpoint). The following software has been tested:

- **LM Studio** - Set endpoint to `http://localhost:1234/v1/responses`
- **Ollama** - With OpenAI compatibility layer
- **Text Generation WebUI** - With OpenAI API extension

> [!IMPORTANT]
> Custom endpoints must be OpenAI API compatible and support the `/v1/responses` endpoint format. Make sure to include the full URL path in your endpoint configuration.

> [!TIP]
> Some local servers don't require an API key. In that case, you can leave the API Key field empty or enter any placeholder value.

### Characters
- `"monika"` - Monika (default)
- `"yuri"` - Yuri
- `"natsuki"` - Natsuki
- `"sayori"` - Sayori

### Behaviour presets
#### Monika
- `"postgame"` - Post-game Monika (default)
- `"romantic"` - Monika acts more flirty and romantic
- `"confident"` - Monika acts more confident
- `"obsessive"` - Monika acts more crazy, confident and obsessive
- `"shy"` - Monika acts more shy and playful

#### Yuri
- `"postgame"` - Post-game Yuri (default)
- `"romantic"` - Yuri acts more romantic (similar to act 1)
- `"obsessive"` - Yuri acts more crazy and obsessive (similar to act 2)
- `"shy"` - Yuri acts more shy and hesitant

#### Natsuki
- `"postgame"` - Post-game Natsuki (default)
- `"romantic"` - Natsuki acts more romantic but tries to hide it
- `"hurt"` - Natsuki acts more hurt/abandoned (kind of similar to act 2)
- `"playful"` - Natsuki acts more playful and relaxed (similar to her baking hangout before the festival)

#### Sayori
- `"postgame"` - Postgame Sayori (default)
- `"depressed"` - Sayori acts more melancholy and sad (similar to the end of act 1)
- `"happy"` - Sayori acts more happy and bubbly (similar to the start of act 1)
- `"romantic"` - Sayori acts romantic
- `"obsessive"` - Sayori acts more obsessive and condident (like the end of act 4)

> [!TIP]
> Make sure you type everything into the config in lowercase exactly how it's written above - it's case sensitive!

> [!TIP]
> Stuck? Join the [discord](https://discord.gg/x6a9AF2aSx) (In there you can also find a YouTube tutorial that goes through everything in detail).

## Controls
The controls are made up of a number of events that the character listens for:

### Application interactions
- **Opening the app:** Opening the app prompts the chosen character to start a conversation.
- **Clicking them:** Clicking them when they're not speaking prompts them to begin a conversation. Whilst in a conversation, clicking moves to the next sentence.
- **Closing the app:** Press the **Close** button at the bottom of the dialogue box. The character will be notified when you close their window and the next time you open them they won't have forgotten.

### User interactions
- **Responding to them:** After a conversation finishes, they may give up to 4 options you can reply with by pressing the corresponding key: 1, 2, 3 or 4. You can also press the **Custom** button to type your own reply. Whilst in custom mode, you can press the **Actions** button to toggle back to the available actions.

### Resetting conversations
By default, the app caches previous conversations and interactions to provide context and a more immersive experience. If you want to reset the conversation history, click the **Reset** button in the textbox. Note that clicking it while in the "thinking" state (displaying the 3 dots) won't work — you need to click it mid conversation to reset correctly.

> [!TIP]
> You can also manually delete the file `character_state.json` generated after the app closes.

> [!TIP]
> You can disable conversation history by setting the `"message_history_size"` field in `config.json` to 0 (not recommended). Increasing this value will also improve context, but may increase costs per interaction for long conversations.
## Issues
If you encounter any issues, please do one of the following:

- Read through the known issues below.
- Join the Discord and report it in the `#bug-report` channel (preferred — I'm most active there and can often help with troubleshooting).
- Open an issue on the [GitHub issues page](https://github.com/swzldev/DDLCDesktop/issues). Please include as much information as possible: your `config.json` settings, API used, model used, and any error messages. As a minimum, upload `log.txt`, a screenshot of the error (if applicable), and the `mini_dump.mdmp` dump file (if one was created), plus what you were doing before the crash occurred.

### Known issues
- Some models may produce invalid outputs causing crashes. If this happens, try resetting or switching to a different model. This should prompt an in-app error where the character will tell you something has gone wrong, but you can also try resetting and see if that solves your issue.
- Stuck in a cycle of the character telling you the API response failed:
  - If it happens very quickly (the character only thinks for a second or two), your API key is likely invalid or out of credits.
  - If it happens after a longer period (10+ seconds), the model may be producing invalid outputs causing crashes. Try switching to a different model or resetting using the **Reset** button.

## Changelog

### [2.1.0]
#### Added
- Added run on boot and run in background settings under the new "general" settings tab.
- Added a system tray icon to allow the app to be "minimised".
- Added an 'auto speed' setting under settings > general.
- Added an uninstaller that automatically deletes the app and carries out any additional cleanup.
- Added discord rich presence support.
#### Fixed
- Fixed a bug where non-ascii characters could be pasted since there was no character validation.
- Fixed the installer sometimes setting the install path to OneDrive if folder redirection is enabled.
- Fixed the installer having the wrong title + description on the install page.
#### Changed
- Redesigned the message history system to be character-specific, this means each character holds their own history and removes the need to reset context when switching characters.
- Improved the error output system - it now prints the actual error message (if present) within the story, the story is also much shorter and consistent across all error types.
- Redesigned the button system to be more robust and allow for additional button types such as "number buttons" in place of sliders etc.
- Installer default install path is now C:/Users/<user>/AppData/roaming/.
- Installer now attempts to locate DDLC game directory if installed on Steam.
- General Installer improvements.
- Changed the start menu and desktop shortcut names to "DDLC Desktop" from "DDLCDesktop", note this may cause duplicates if you've used the old installer before.
- Trimmed the entered API key which may help prevent pasting extra whitespace/newlines which can bug out.

### [2.0.0]
#### Added
- Added an updater project that is able to automatically download new versions as and when they release.
- Added an installer project that makes installation much easier & auto-extracts game assets.
- Added a max_tokens entry to the config along with it's in-app setting under API > Token Limit.
#### Fixed
- Fixed some memory leak issues with the input handling system.
- Fixed a bug where interactions would still occur whilst in a popup.
#### Changed
- Improved the input handling system which should now be more stable.
- Changed the 'Close' button to be 'Quit' and removed it from certain parts like in settings.
- Improved some error messages.

### 1.1.1 [25/01/2026]
- Added support for custom endpoints (local LLM servers, etc.)
- Improved the error message for bad API responses to be less ambiguous and confusing.

### 1.1.0 [20/01/2026]
- Added support for each characters "casual" outfit varients.
- Improved character sizing and positioning (it's now more consistent, and more restricted, also works on monitors other than 1080p)
- Allowed support for other languages.
- Fixed text rendering so it shouldn't have any more weird characters in sentences and stuff.
- Fixed a bug where clicking the buttons would also count as clicking the character.
- Fixed a bug where there was a small 1-2px gap between the characters heads and bodies.
- Added the ability to drag them around.
- Added custom pronoun support.
- Fixed a bug where some models would wrap the json data in code fences.
- Fixed a bug where it would crash when changing to a character that doesnt support the current pose/expression.
- The textbox no longer disappears if theyre not saying anything, which previously prevented access to any of the buttons such as "close".
- All config.json options are now built-in to the app under the "settings" button.
- Adding interactive setup if on a fresh installation.

### 1.0.0 [11/01/2026]
- Added support for custom inputs.
- Added OpenRouter API support.
- Added error message stories (essentially built-in stories that will play if an error occurs)
- Added a dedicated reset button.
- Removed broken Yuri expressions.
- Fixed a bug where the app wouldn't close fully if you closed it whilst it was thinking (displaying "...") - for some reason a lot of people thought this was suspicious so its fixed now.

### 1.0.0-beta [09/01/2026]
- Fixed Natsuki and Sayori not having a name.
- Fixed some Yuri head sprite names being incorrect.
- Fixed the window click through bug, now you are only able to click Monika and anywhere else will pass through.
- Improved stroke rendering - it's now much cleaner at bigger font sizes and replicates the game much more closely.
- Removed old escape to close bind in place of a new "close" button.

### 1.0.0-alpha [07/01/2026]
- The characters are now able to move freely around the desktop and resize themselves.
- Fixed a bug where the system prompt was erased causing crashes.
- Fixed a crash that would happen with models that return multiple outputs.
- Added partial support for other characters: Yuri, Natsuki and Sayori.
- Added a ton of error checking and fallbacks to prevent some crashes and improve stability.
- Many improvements to the system prompts for a more realistic experience.

### 0.2.0 [05/01/2026]
- Added multiple behaviour presets for Monika.
- Added fallbacks for when the AI generates bad character poses/expressions to prevent crashes.
- Fixed various bugs and improved stability.

## Contributing
This application is written pretty poorly (I made it in a single day), so I'll likely accept most contributions — feel free to jump in.

## License
MIT license — feel free to do whatever you want with it. I made this as a fun side project.
