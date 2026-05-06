# remete2
## Eremite (Remete2) ham radio remote site management and remote control

> This is a repository under construction!

The name and theme stem from a Hungarian play on words, based on the similarity between the words "remote" and "eremite" (hermit), which is "remete" in Hungarian. *These are just—hopefully—playful and amusing puns; they have nothing to do with any religious beliefs we may hold.* Earlier, we used the word "remete" to refer to both the microcontroller-based, remotely controlled server box equipped with relays and sensors, and the client software developed for Windows. Now, they have different names.

The first remote control that made it possible to use radios and antennas of our radio amateur (ham) club station from a remote location was developed by our oldest member, Karcsi (HA5BVK) and has been in excellent working order for a long time. The name "Remete" is his invention, and we still use it as a tribute to him. This is the reason why I also have taken the names of further elements of this system from that area.

As with the original "Remete" host and client, and my "Gatekeeper" ("Házmester") power management and data acquisition firmware, the C/C++, Visual Basic, HTML, and CSS codes in these older projects were created manually. They were neither generated nor Vibe-coded. However, I used LLMs heavily in the newer ones to shorten the development and documentation time.

***
Components:
| Name                    | Description |
| ----------------------- | --------------------------------------------------------------------------------------------- |
| Gatekeeper (Házmester)  | Remote site management hardware and software. The outermost circle that includes power management, a mechanical safeguard against lightning damage, data acquisition from environmental sensors, etc. |
| Eremite (Remete2)       | "The" remote control hardware and software (radios, antennas, rotators, etc.), the namegiver. |
| Friar (Szőrzetes)       | Client-side software and optionally some hardware as well, perhaps for physical controls attached to their computers. This is the new system, which is in the planning phase in case the old one becomes obsolete or is no longer maintainable for any reason. |
| <ul><li>Intermediary</li></ul> | <ul><li>Eremite (Remete2) client service. Part of the Friar.</li></ul> |
| <ul><li>Visionary</li></ul> | <ul><li>Eremite (Remete2) client UI applications. Part of the Friar.</li></ul> |
| Developer Documentation | Public documentation can be found at: https://github.com/ha3flt/remete2/wiki/ |

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
