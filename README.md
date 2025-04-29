# Hardware Development Kit
[![Docs](https://img.shields.io/badge/docs-tidbyt.dev-blue?style=flat-square)](https://tidbyt.dev)
[![Build Status](https://img.shields.io/github/actions/workflow/status/tidbyt/hdk/main.yaml?style=flat-square)](https://github.com/tidbyt/hdk/actions/workflows/main.yaml)
[![Discourse](https://img.shields.io/discourse/status?server=https%3A%2F%2Fdiscuss.tidbyt.com&style=flat-square)](https://discuss.tidbyt.com/)
[![Discord Server](https://img.shields.io/discord/928484660785336380?style=flat-square)](https://discord.gg/r45MXG4kZc)

This repository contains a community supported firmware for the Tidbyt hardware 🤓. 

![social banner](./docs/assets/social.png)

## Warning
⚠️ Warning! Flashing your Tidbyt with this firmware or derivatives could fatally 
damage your device. As such, flashing your Tidbyt with this firmware or
derivatives voids your warranty and comes without support.

## Setup
This project uses PlatformIO to build, flash, and monitor firmware on the Tidbyt.
To get started, you will need to download [PlatformIO Core][2] on your computer.

Additionally, this firmware is designed to work with [Pixlet][1]. Using
`pixlet serve`, you can serve a WebP on your local network. Take note of your
computers IP address and replace it in the `TIDBYT_REMOTE_URL` example above.
While we had pixlet in mind, you can point this firmware at any URL that hosts
a WebP image that is optimized for the Tidbyt display.

## Configuration (secrets.h)
Before running the application, you need to configure the `src/secrets.h` file with the following:

```c
// WiFi credentials
#define TIDBYT_WIFI_SSID "Your WiFi Network Name"
#define TIDBYT_WIFI_PASSWORD "Your WiFi Password"

// Base URL for your pixlet server
const char* BASE_URL = "http://your-server-ip-or-hostname:3000";

// Applet paths to display (these are paths that will be appended to BASE_URL)
const char* APPLET_PARAMS[] = {
  "/?fileName=applet1",
  "/?fileName=applet2&param1=value1&param2=value2",
  // Add more applet configurations as needed
};
```

The `APPLET_PARAMS` array defines which pixlet applets will be displayed in rotation, with each URL being constructed by combining the `BASE_URL` with the path and parameters for that specific applet.

## Getting Started
To flash the custom firmware on your device, run the following after replacing
the variables with your desired information:
```
TIDBYT_WIFI_SSID='Your WiFi' \
TIDBYT_WIFI_PASSWORD='super-secret' \
TIDBYT_REMOTE_URL='http://192.168.10.10:8080/api/v1/preview.webp' \
 pio run --environment adafruit_matrixportal_esp32s3 --target upload
```

## Monitoring Logs
To check the output of your running firmware, run the following:
```
pio device monitor
```

## Back to Normal
To get your Tidbyt back to normal, you can run the following to flash the
production firmware onto your Tidbyt:
```
pio run --target reset
```

[1]: https://github.com/tidbyt/pixlet
[2]: https://docs.platformio.org/en/latest/core/installation/index.html


Add applets to the APPLETS char array in main.c to upload.
When uploading, use PlatformIO's VS Code extension. Try different cables if the upload does not find the USB port.