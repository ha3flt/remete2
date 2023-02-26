## Gatekeeper (Házmester) - "esp32-wss-real-time-remote-manager"
### Secure Remote Real-time Digital and Analog I/O on ESP32-S3 using HTTPS+WSS (SSL/TLS and Secure WebScoket).

How to build (//TODO):
1. Copy ./sdkconfig.sX.template --> ./sdkconfig (X=2 or 3 for the ESP32-S2 or ESP32-S3 board versions, respectively)
2. Patch files in the framework that you can find in the ./patches folder.
3. Build the project

DO NOT FORGET TO GENERATE A NEW KEY PAIR BEFORE RELEASE, AND DO NOT COMMIT THEM AND UPLOAD TO GITHUB!

Key generation:
openssl req -newkey rsa:2048 -nodes -keyout prvtkey.pem -x509 -days 18250 -out servercert.pem -subj "/CN=Safe Remote Switch, HG5C"
