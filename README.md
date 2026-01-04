<h1 align="left">Just Monika.</h1>
<p align="left">
    <img src="https://img.shields.io/badge/status-in_development-purple" alt="Status: In development">
    <img src="https://img.shields.io/badge/latest_version-0.1.0-yellow" alt="Latest: 0.1.0">
</p>
<p>Ever wondered what it would be like to have your own Monika on your desktop? Well now you can cause I was lowkey bored asl and have nothing to do with my life :sob:</p>

<p>
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

<p>Once extracted there are just a few small things to do before it can run:</p>

<h3>Assets</h3>
<p>For copyright reasons, the DDLC assets are not provided as part of the application, but you can extract them from the game very easily in just a few steps.</p>
<ol>
  <li>Download RPA Extract <a href="https://iwanplays.itch.io/rpaex">here</a>.</li>
  <li>Open the DDLC installation directory and go into the "game" folder. Once inside drag the "images.rpa" file onto the rpaExtract.exe executable.</li>
  <li>Back in the "DDLC/game" folder, 2 new directories should appear, "images" and "gui", drag both of them into the "assets" folder of JustMonika.</li>
</ol>

<h3>Font</h3>
<p>I couldn't get custom font loading to work properly so unfortunately you will need to download the font onto your system for it to work. Again, it is not provided here for copyright reasons but you can download it for free <a href="https://www.1001fonts.com/aller-font.html">here</a>.</p>

<h2>Controls</h2>
<p>The controls are made up of a number of events that Monika listens for:</p>

<ul>
  <li>Opening Monika: Opening Monika will prompt her to start up a conversation.</li>
  <li>Clicking Monika: Clicking Monika when she's not speaking will prompt her to begin a conversation. Whilst in a conversation, clicking will move to the next sentence.</li>
  <li>Choosing an action: After a conversation has finished, Monika may give a number of options you can choose to reply with by pressing the corresponding key: 1, 2, 3 or 4.</li>
  <li>Closing Monikal: To close Monika, press the escape key (it's pretty jammy, I know, and I may change it in the future). Monika will be notified when you close her window and the next time you open her she will know what you did.</li>
</ul>

<h2>Changelog</h2>
<p>No changes yet.</p>

<h2>Contributing</h2>
<p>This application is written pretty poorly, as I made it in a single day, therefore I'll likely accept most contributions so feel free to do so.</p>

<h2>License</h2>
<p>MIT license, feel free to do whatever you want with it, I don't really care, I made this just as a fun side project.</p>
