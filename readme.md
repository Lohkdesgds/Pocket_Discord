# PocketDiscord C++ Discord Bot Library

*This is a library I've been working on for a while. I wanted a cool great Discord bot running on a simple and cheap ESP32, and so I searched for it for a while... There was one guy, but most of the API was limited or non existant, and it was just C, and it was so memory hungry, oh my god.*

*So I created this. Looking through Discord's API reference, some examples and the ESP-IDF documentation, I made it! This is a fairly stable V9-kinda-ready C++ library!*

<hr>

## ⇒ Features

* *Mainly C++ (C is used with ESP-IDF's C stuff)*
* *SD card support*
* *Easy installation (if your VS Code is good with ESP-IDF, tested using WSL2)*
* *Memory-like data in SD card (slow, but "unlimited memory", so no worries about big JSON crashing stuff ;P)*
* *Multithreaded lambda-based stuff (gateway events)*
* *Uses std::future for HTTP requests (async)*
* *Low memory usage! (with a bot running stand-by as logger, memory usage was in ~38 kB! ESP32 has ~300 kB, so ~260 kB is free for your stuff ;P)*
* *Smart custom JSON interpreter (not exactly fast, but very low memory and fast enough)*
* *Exception handling (basic, reports on console)*
* *... and many more!*

<hr>

## ⇒ Need help?

*Please, just join [the Discord Server](https://discord.gg/JkzJjCG).*<br>
*Sorry, this is not **the** library server, it's the one I own and I'm online most of the time.*<br>
*Later in the future I **might** create a dedicated server, but I don't know. Don't worry though, just talk in chat.*<br>
*Most of it is in Portuguese (Brazil), but I'm fine with English.*

<hr>

## ⇒ IMPORTANT NOTES

1. *I did **NOT** create a way to download attachments. If you want to help me with that, it would be nice (you can use MemoryFile to just dump all bytes there!).*
2. *Most of the RESUME/CLOSE-RESTART stuff is there. If the library breaks itself, it should restart the entire ESP32 by itself. (It will try RESUME 99% of the time, but if the error is bad it just restarts)*
3. *I am still adding things to this library. Some stuff is already there, and if you want to help, you probably can add more features yourself (HTTP stuff is simple, all stuff is in Discord's Development Portal)*
4. *I may change some stuff entirely from an update to another. I'll try to keep compatibility most of the time, but it's hard to guarantee that.*
5. *YES, I DID A RATE-LIMIT ALGORITHM. It's not optimized for many servers though. The rate limit is global. Better than no rate limit, right?*

<hr>

