<p><img src=".\assets\logo-circle.png" alt="Logo" align="right" height="230px"></p>
<h1 align="middle">Just Monika.</h1>
<p align="middle">
    <img src="https://img.shields.io/badge/status-in_development-purple" alt="Status: In development">
    <img src="https://img.shields.io/badge/latest_version-0.2.0-yellow" alt="Latest: 0.2.0">
</p>
<p align="middle">Ever wondered what it would be like to have your own Monika on your desktop? No? Well now you can!</p>

<p align="middle">
  <a href="#features">Features</a> &bull;
  <a href="#installation">Installation</a> &bull;
  <a href="#controls">Controls</a> &bull;
  <a href="#changelog">Changelog</a> &bull;
  <a href="#contributing">Contributing</a> &bull;
  <a href="#license">License</a>
</p>

<blockquote>
<h3>Important note:</h3>
<p>This application does not work out of the box - make sure you read the installation instructions carefully before use.</p>
</blockquote>

<h2>Features</h2>
<ul>
  <li>An interactive Monika that sits on your desktop and talks to you.</li>
  <li>Real AI interactions using the OpenAI api.</li>
  <li>Respond to Monika similar to how you would in the game (with a set of 1-4 options).</li>
  <li>Monika reacts to things you do - currently its fairly limited though.</li>
</ul>

<h2>Installation</h2>
<p>Installation is fairly simple however there are a few nuances before you can be up and running.</p>

<p>First install the application binaries from the <a href="https://github.com/swzldev/JustMonika/releases">releases</a> page. It should be called <code>JustMonika-*.*.*-Windows.zip</code></p>
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

<h3>Config.json & API Key</h3>
<p>Next to JustMonika.exe there is a file called config.json. Open this file in a text editor and you will see the following:</p>
<pre><code>{
  "openai_api_key": "&lt;your api key&gt;",
  "openai_model": "gpt-4o-mini",
  "openai_message_history_size": 6,
  "user_name": "&lt;your name&gt;",
  "behaviour_preset": "default"
}
</code></pre>
<p>You will need to fill in your OpenAI API key in order for Monika to be able to talk to you. You can get an API key by signing up for an account at <a href="https://platform.openai.com/signup">OpenAI</a> and generating a key in the API section. Note: you may have to add credit for it to work, but don't worry, it's pretty afforable, just 5$ of credit will last you thousands of conversations (assuming you stick to the gpt-4o-mini model).</p>

<p>Once you have your API key, replace <code>&lt;your api key&gt;</code> with your actual key. You can also change the <code>user_name</code> field to whatever you want Monika to call you, and lastly, if you want a custom behaviour you can change the <code>"behaviour_preset"</code> to any one of the following:</p>

<ul>
  <li><code>"postgame"</code> - Postgame Monika (default)</li>
  <li><code>"romantic"</code> - Monika acts more flirty and romantic</li>
  <li><code>"confident"</code> - Monika acts more confident</li>
  <li><code>"obsessive"</code> - Monika acts more crazy, confident and obsessive</li>
  <li><code>"shy"</code> - Monika acts more shy and playful</li>
</ul>

<h2>Controls</h2>
<p>The controls are made up of a number of events that Monika listens for:</p>

<ul>
  <li>Opening Monika: Opening Monika (aka starting the application) will prompt her to start up a conversation.</li>
  <li>Clicking Monika: Clicking Monika when she's not speaking will prompt her to begin a conversation. Whilst in a conversation, clicking will move to the next sentence.</li>
  <li>Choosing an action: After a conversation has finished, Monika may give a number of options you can choose to reply with by pressing the corresponding key: 1, 2, 3 or 4.</li>
  <li>Closing Monikal: To close Monika, press the escape key (it's pretty jammy, I know, and I may change it in the future). Monika will be notified when you close her window and the next time you open her she will know what you did.</li>
</ul>

<h2>Changelog</h2>

<h3>Unreleased</h3>
<ul>
  <li>Monika is now able to move freely around the desktop and resize herself.</li>
  <li>Fixed a bug where the system prompt was erased causing crashes.</li>
  <li>Fixed a crash that would happen with models that return multiple outputs.</li>
  <li>Many improvements to the system prompts for a more realistic experience.</li>
</ul>

<h3>v0.2.0 [05/01/2026]</h3>
<ul>
  <li>Added multiple behaviour presets for Monika.</li>
  <li>Added fallbacks for when the AI generates bad character poses/expressions to prevent crashes.</li>
  <li>Fixed various bugs and improved stability.</li>
</ul>

<h2>Contributing</h2>
<p>This application is written pretty poorly, as I made it in a single day, therefore I'll likely accept most contributions so feel free to do so.</p>

<h2>License</h2>
<p>MIT license, feel free to do whatever you want with it, I don't really care, I made this just as a fun side project.</p>
