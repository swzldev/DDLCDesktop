<div align="center">
  <img src=".\assets\logo-circle.png" alt="Logo" height="230px">
  <h1>DDLC Desktop</h1>
  <p>Ever wondered what it would be like to have your own DDLC character on your desktop? Well now you can!</p>
  <p>
    <img src="https://img.shields.io/badge/status-stable-green" alt="Status: Stable">
    <img src="https://img.shields.io/badge/latest_version-1.1.0-red" alt="Latest: 1.1.0">
  </p>

  [Discord] | [Features] | [Installation] | [Controls] | [Issues] | [Changelog] | [Contributing] | [License]
</div>

[Discord]: https://discord.gg/x6a9AF2aSx
[Features]: #features
[Installation]: #installation
[Controls]: #controls
[Issues]: #issues
[Changelog]: #changelog
[Contributing]: #contributing
[License]: #license

> [!IMPORTANT]
> This application does not work out of the box - make sure you read the installation instructions carefully before use.

> [!IMPORTANT]
> Disclaimer: This is an unofficial, fan-made application and is not affiliated with, endorsed by, or approved by Team Salvato.
> *Doki Doki Literature Club* and all related characters, assets, and trademarks are © Team Salvato.
> This application does not include or distribute any copyrighted assets. Users must own a legitimate copy of Doki Doki Literature Club and provide their own game files.

<h2>Features</h2>
<ul>
  <li>An interactive DDLC character that sits on your desktop and talks to you.</li>
  <li>Real AI interactions using the OpenAI api.</li>
  <li>Respond similar to how you would in the game (with a set of 1-4 options).</li>
  <li>They react to things you do - currently its fairly limited though.</li>
</ul>

<h2>Installation</h2>
<p>Installation is fairly simple however there are a few nuances before you can be up and running.</p>

<p>Since the installation can be quite confusing, for non-tech savvy users, there is a <a href="https://www.youtube.com/watch?v=a_A4Sm2gPh0">YouTube guide</a> that explains everything in detail.</p>

<p>If you're fine with a written guide you can simply follow the instructions here. First install the application binaries from the <a href="https://github.com/swzldev/DDLCDesktop/releases">releases</a> page. It should be called something like: <code>DDLCDesktop-*.*.*.zip</code></p>
<p>Once downloaded, extract the contents somewhere (make sure it's not somewhere with limited permissions such as Program Files or System32).</p>

<p>Once extracted simply follow the instructions below:</p>

<h3>Assets</h3>
<p>For copyright reasons, the DDLC assets are not provided as part of the application, but you can extract them from the game very easily in just a few steps.</p>
<ol>
  <li>Download <a href="https://iwanplays.itch.io/rpaex">RPA Extract</a>.</li>
  <li>Open the DDLC installation directory and go into the "game" folder. Once inside drag the "images.rpa" file onto the rpaExtract.exe executable.</li>
  <li>Back in the "DDLC/game" folder, 2 new directories should appear, "images" and "gui", drag both of them into the "assets" folder of JustMonika.</li>
</ol>
<p>If you're confused you can use the visual diagram below:</p>
<img width="1668" height="801" alt="image" src="https://github.com/user-attachments/assets/82ba9a7e-7535-418e-9642-593cc6c8c1c2" />

<h3>Font (OPTIONAL)</h3>
<p>By default Just Monika attempts to use 3 fonts, "Aller" -> "Segoe UI" -> "Arial". If you want it to use the original DDLC font (Aller), you will need to install it onto your system. Again, it is not provided with the binaries for copyright reasons, but you can download it for free <a href="https://www.1001fonts.com/aller-font.html">here</a>.<p>

> [!IMPORTANT]
> On versions past 1.1.0, the rest of the installation process is now built into the app. As long as you've completed the two steps above, you can open JustMonika.exe and follow the instructions that should appear. I will keep the legacy guide below for reference, as some of it may still be useful (for example, inside the app's settings menu it is useful to see the different character preset names, etc.)
> You should also note that all the config.json options are now built into the app under the "settings" button. You should no longer ever need to modify config.json manually.

<h3>Config.json & API Key</h3>
<p>Next to JustMonika.exe there is a file called config.json. Open this file in a text editor and you will see the following:</p>
<pre><code>{
  "api": "openrouter",
  "api_key": "&lt;your api key&gt;",
  "model": "",
  "message_history_size": 6,
  "user_name": "&lt;your name&gt;",
  "behaviour_preset": "default",
  "character": "monika"
}
</code></pre>
<ul>
  <li><code>"api"</code>: The API to use: "openai", "openrouter", or "custom". OpenRouter is recommended as it has a free tier. Use "custom" for local LLMs or other OpenAI-compatible endpoints.</li>
  <li><code>"api_key"</code>: Your API key for the chosen API.</li>
  <li><code>"model"</code>: The model to use for the character. If you don't know what to choose you can find info in the models section of this guide.</li>
  <li><code>"message_history_size"</code>: The number of previous messages to keep in context for the character. Higher values will improve context but may increase costs. A good number is usually 6-10</li>
  <li><code>"user_name"</code>: The name the character will call you.</li>
  <li><code>"behaviour_preset"</code>: The behaviour preset for the character. More info in the behaviour presets section  of  this guide.</li>
  <li><code>"character"</code>: The character to use. More info in the characters section of this guide.</li>
</ul>

<h4>Models:</h4>
<p>The model you choose will have a big impact on how the character behaves. Additionally, not all models work effectively and some will cause crashes if not used for their intended purpose (e.g. using a model designed for coding, or reasoning, image generation, etc.). Below are some models that have been tested and are effective along with any additional info. Note the model names may differ depending on your chosen API.</p>
<h5>OpenRouter:</h5>
<ul>
  <li><code>"meta-llama/llama-3.3-70b-instruct:free"</code> - Probably the best free model, works very well.</li>
  <li><code>"openai/gpt-4o-mini"</code> - Good general purpose model, cheap and effective.</li>
  <li><code>"openai/gpt-4.1"</code> - My personal favourite, realistic responses but slightly more expensive.</li>
</ul>
<h5>OpenAI:</h5>
<ul>
  <li><code>"gpt-4o-mini"</code> - Good general purpose model, cheap and effective.</li>
  <li><code>"gpt-4.1"</code> - My personal favourite, realistic responses but slightly more expensive.</li>
</ul>

> [!TIP]
> If you've found a model that works well, please let me know on the discord so I can update the documentation!

<h5>Custom Endpoints:</h5>
<p>If you want to use a local LLM server or other OpenAI-compatible API, set <code>"api": "custom"</code> and configure your endpoint URL via the in-app Settings menu (Settings → API → Endpoint). The following software has been tested:</p>
<ul>
  <li><strong>LM Studio</strong> - Set endpoint to <code>http://localhost:1234/v1/responses</code></li>
  <li><strong>Ollama</strong> - With OpenAI compatibility layer</li>
  <li><strong>Text Generation WebUI</strong> - With OpenAI API extension</li>
  <li><strong>LocalAI</strong> - Any OpenAI-compatible local server</li>
</ul>

> [!IMPORTANT]
> Custom endpoints must be OpenAI API compatible and support the <code>/v1/responses</code> endpoint format. Make sure to include the full URL path in your endpoint configuration.

> [!TIP]
> Some local servers don't require an API key. In that case, you can leave the API Key field empty or enter any placeholder value.

<h4>Characters:</h4>
<ul>
  <li><code>"monika"</code> - Monika (default)</li>
  <li><code>"yuri"</code> - Yuri</li>
  <li><code>"natsuki"</code> - Natsuki</li>
  <li><code>"sayori"</code> - Sayori</li>
</ul>

<h4>Behaviour Presets:</h4>
<h5>Monika:</h5>
<ul>
  <li><code>"postgame"</code> - Post-game Monika (default)</li>
  <li><code>"romantic"</code> - Monika acts more flirty and romantic</li>
  <li><code>"confident"</code> - Monika acts more confident</li>
  <li><code>"obsessive"</code> - Monika acts more crazy, confident and obsessive</li>
  <li><code>"shy"</code> - Monika acts more shy and playful</li>
</ul>
<h5>Yuri:</h5>
<ul>
  <li><code>"postgame"</code> - Post-game Yuri (default)</li>
  <li><code>"romantic"</code> - Yuri acts more romantic (similar to act 1)</li>
  <li><code>"obsessive"</code> - Yuri acts more crazy and obsessive (similar to act 2)</li>
  <li><code>"shy"</code> - Yuri acts more shy and hesitant</li>
</ul>
<h5>Natsuki:</h5>
<ul>
  <li><code>"postgame"</code> - Post-game Natsuki (default)</li>
  <li><code>"romantic"</code> - Natsuki acts more romantic but tries to hide it</li>
  <li><code>"hurt"</code> - Natsuki acts more hurt/abandoned (kind of similar to act 2)</li>
  <li><code>"playful"</code> - Natsuki acts more playful and relaxed (similar to her baking hangout before the festival)</li>
</ul>
<h5>Sayori:</h5>
<ul>
  <li><code>"postgame"</code> - Postgame Sayori (default)</li>
  <li><code>"depressed"</code> - Sayori acts more melancholy and sad (similar to the end of act 1)</li>
  <li><code>"happy"</code> - Sayori acts more happy and bubbly (similar to the start of act 1)</li>
  <li><code>"romantic"</code> - Sayori acts romantic</li>
  <li><code>"obsessive"</code> - Sayori acts more obsessive and condident (like the end of act 4)</li>
</ul>

> [!TIP]
> Make sure you type everything into the config in lowercase exactly how it's written above - it's case sensitive!

> [!TIP]
> Stuck? Join the [discord](https://discord.gg/x6a9AF2aSx) (In there you can also find a YouTube tutorial that goes through everything in detail).

<h2>Controls</h2>
<p>The controls are made up of a number of events that Monika listens for:</p>

<h3>Application interactions:</h3>
<ul>
  <li>Opening the app: Opening the app will prompt the chosen character to start up a conversation.</li>
  <li>Clicking them: Clicking them when they're not speaking will prompt them to begin a conversation. Whilst in a conversation, clicking will move to the next sentence.</li>
  <li>Closing the app: To close the app you can press the 'close' button at the bottom of the dialogue box, the character will be notified when you close their window and the next time you open them they won't have forgotten.</li>
</ul>

<h3>User interactions:</h3>
<ul>
  <li>Responding to them: After a conversation has finished, they may give a number of options you can choose to reply with by pressing the corresponding key: 1, 2, 3 or 4. You can also press the "Custom" button to type your own reply. Whilst in custom mode, you can press the "Actions" button to toggle back to the available actions.</li>
</ul

<h3>Resetting conversations</h3>
<p>By default, the app will cache previous conversations and interactions to provide context and a more immersive experience. However, if you want to reset the conversation history, you can click the "Reset" button in the textbox. Note that clicking it while in the "thinking" state (displaying the 3 dots) won't work. You need to click it mid conversation to reset correctly.</p>

> [!TIP]
> You can also manually delete the file <code>character_state.json</code> generated after the app closes.

> [!TIP]
> You can disable conversation history by setting the <code>"openai_message_history_size"</code> field in config.json to 0 (Not reccomended). Increasing this value will also improve context, but may increase costs per interaction for long conversations.

<h2>Issues</h2>
<p>If you encounter any issues, please do one of the following:</p>

<ul>
  <li>Read through the known issues below.</li>
  <li>Join the discord and report it in the #bug-report channel - This is the preferred method as I am most active on there and can often help with troubleshooting.</li>
  <li>Open an issue on the <a href="https://github.com/swzldev/DDLCDesktop/issues">GitHub issues page</a> - Please make sure to include as much information as possible including your config.json settings, API used, model used and any error messages you may have received. As a minimum you should upload the log.txt, a screenshot of the error (if applicable) and the mini_dump.mdmp dump file (if one was created), plus whatever you we're doing before the crash occurred.</li>
</ul>

<h3>Known Issues</h3>
<ul>
  <li>Some models may produce invalid outputs causing crashes. If this happens, try resetting or switching to a different model. This should prompt an in-app error where Monika will tell you if something has gone wrong but just incase you can try to reset it and see if that solves your issue.</li>
  <li>Stuck on a cycle of Monika telling you the API response failed. If it happens very quickly (i.e. the character only thinks for a second or two before telling you the API response failed) then it's likely your API key is invalid or has run out of credits. If it happens after a longer period of time (i.e. 10+ seconds) then it's likely the model is producing invalid outputs causing crashes. Try switching to a different model or resetting using the "Reset" button.</li>
</ul>

<h2>Changelog</h2>

### [1.1.0] [20/01/2026]
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

<h2>Contributing</h2>
<p>This application is written pretty poorly, as I made it in a single day, therefore I'll likely accept most contributions so feel free to do so.</p>

<h2>License</h2>
<p>MIT license, feel free to do whatever you want with it, I don't really care, I made this just as a fun side project.</p>
