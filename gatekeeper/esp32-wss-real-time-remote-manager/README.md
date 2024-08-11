## Gatekeeper (Házmester) - "esp32-wss-real-time-remote-manager"
### Secure Remote Real-time Digital and Analog I/O on ESP32-S3 using HTTPS+WSS (SSL/TLS and Secure WebScoket).

PlatformIO was tried, but the development is in the original espressif esp-idf framework.

How to build (//TODO):
1. Copy ./sdkconfig.sX.template --> ./sdkconfig (X=2 or 3 for the ESP32-S2 or ESP32-S3 board versions, respectively)
2. Patch files in the framework that you can find in the ./patches folder.
3. Build the project

DO NOT FORGET TO GENERATE A NEW KEY PAIR BEFORE RELEASING, AND DO NOT COMMIT AND UPLOAD IT TO GITHUB!

Key generation:
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 18250 -out servercert.pem -subj "/CN=Safe Remote Switch, HG5C"
