# remete2
## Eremite (Remete2) is a ham radio remote site management and remote control system that employs standalone hosts and computer-based clients.

> This is a repository under construction!

The name and theme stem from a Hungarian play on words, based on the similarity between the words "remote" and "eremite" (hermit), which is "remete" in Hungarian. *These are just—hopefully—playful and amusing puns; they have nothing to do with any religious beliefs we may hold.* Earlier, we used the word "remete" to refer to both the microcontroller-based, remotely controlled server box equipped with relays and sensors, and the client software developed for Windows. Now, they have different names.

The first remote control that made it possible to use radios and antennas of our radio amateur (ham) club station from a remote location was developed by our oldest member, Karcsi (HA5BVK) and has been in excellent working order for a long time. The name "Remete" is his invention, and we still use it as a tribute to him. This is the reason why I also have taken the names of further elements of this system from that area.

As with the original "Remete-III" host and client, and my "Gatekeeper" ("Házmester") real-time power management and data acquisition firmware, the C/C++, Visual Basic, HTML, and CSS codes in these older projects were created manually. They were neither generated nor Vibe-coded. However, I used LLMs heavily in the newer ones to shorten the development and documentation time.

***
Components:
| Name                    | Description |
| ----------------------- | --------------------------------------------------------------------------------------------- |
| Gatekeeper (Házmester)  | A web-based real-time, multi-user and secure remote site management firmware and hardware using modern encryption and living communication channels (secure WebSockets). This is the outermost circle nearest to the router that includes power management using opto-isolated relays, a mechanical safeguard against lightning damage, analog and digital data acquisition from environmental sensors, etc. (Its earlier version is called: The Bridge. It could only turn relays on and off, and its web page had to be manually refreshed.) The Gatepeeker is an ESP32-S3 board that is programmed using the ESP-IDF CLI environment, but Platformio.ini is available if needed. The first version of this solution was developed remotely and worked successfully at the home of a fellow ham radio operator in Slovakia. Sadly, he passed away in February 2026. Rest in peace, Johhny. |
| <blockquote>"The Bridge"</blockquote> | <blockquote>The old web-based relay remote switching application. Essentially, it is an example from the development tools of an old ESP32 board. Although it is neither real-time nor secure, it was a project that could be completed in a few days many years ago and has worked flawlessly ever since. However, its static nature poses a risk in a multi-user environment because it does not update the data on the client-side data based on events, such as when someone else turning a channel on or off. I mitigated most of the risk a few years ago by designing a special workflow within the client application. This workflow assumes that everyone uses our extended "Remete" application instead of a web browser. It is only partially documented here.</blockquote> |
| <blockquote>"Remete-III"</blockquote> | <blockquote>Despite its name, this is the old system developed by our oldest member, Karcsi (HA5BVK). This system consists of a server box that contains a TCP/IP-capable PIC, numerous of opto-isolated SPI I/O chips for the relay matrix for our antenna system, tunneled RS-232 connections, and analog inputs from the antenna rotators. The client side is a Visual Basic 4 application. Since we don't have time to document a system that is already up and running, that we won't further develop, but rather replace, only partial information about these units and their communication is provided here.</blockquote> |
| Eremite (Remete2)       | The host side of the remote control system. A standalone host, a box and its hardware and firmware for the above mentioned radios, antennas, rotators, etc.), the namegiver. |
| Friar (Szőrzetes)       | Complete client-side solution for the host above, multi-platform software and optionally some hardware as well, perhaps for physical controls attached to the clients' computers. This will be the new system, which is in the planning phase in case the old one becomes obsolete or is no longer maintainable for any reason. |
| <ul><li>Intermediary</li></ul> | <ul><li>Eremite (Remete2) client service. Part of the Friar.</li></ul> |
| <ul><li>Visionary</li></ul> | <ul><li>Eremite (Remete2) client UI applications. Part of the Friar.</li></ul> |

Public documentation can be found at: https://github.com/ha3flt/remete2/wiki/

***

Pictures of the current system that we want to redesign:
<div align-text="center" width="95%">
  <img src="https://ha3flt.org/photos/github/remete-present-001.jpg" alt="Remete, present, 1." width="480px"/>
  <img src="https://ha3flt.org/photos/github/remete-present-002.jpg" alt="Remete, present, 2." width="480px"/>
</div>
<div align-text="center" width="95%">
  <img src="https://ha3flt.org/photos/github/remete-present-011.jpg" alt="Remete, present, 3." width="480px"/>
  <img src="https://ha3flt.org/photos/github/remete-present-012.jpg" alt="Remete, present, 4." width="480px"/>
</div>

***

Created by **HA3FLT**
