# KazaKhan's Fibonacci Clock

A WiFi-connected **Fibonacci clock** built on an ESP8266 (Wemos D1 mini) or
ESP32, driving a WS2812/NeoPixel strip. It has a WiFiManager setup portal, hourly
NTP sync, 22 colour themes, OTA updates, and a fast, responsive web UI that
explains itself.

![KazaKhan's Fibonacci Clock](images/logo.png)

---

## How the clock works

The clock is five squares, each worth a Fibonacci number: **1, 1, 2, 3, 5**.

- **Red** = hours only
- **Green** = minutes only
- **Blue** = hours *and* minutes
- **White** = not used

To read it:

- **Hours** = add the values of the red + blue squares.
- **Minutes** = add the values of the green + blue squares, then **multiply by 5**.

For example, `6:45` is:

```
hours   = 1 + 5           = 6
minutes = (1 + 3 + 5) × 5 = 45
```

Minutes only move in 5-minute steps (the classic Fibonacci clock behaviour).
The web page has a **"How to read this?"** link (and a hover tooltip on the
clock) that shows this using the colours and time that are live right now.

---

## Features

- ESP8266 **and** ESP32 support.
- **WiFiManager** setup portal (customised, dark, branded) — no buttons needed.
- **Hourly NTP** sync with a selectable timezone (~32 zones).
- **22 themes** (RGB, Mondrian, 80s, Pastel, Neon, Cyberpunk, …), off blocks lit white.
- **Runtime-configurable LEDs per segment** with a live 5-square preview.
- **Segment chase test** on boot (toggleable) and on demand.
- **OTA** updates (ArduinoOTA and a browser `/update` page).
- Settings persisted to LittleFS.
- Fast, responsive, self-contained web page (no CDN, no filesystem upload).

---

## Hardware

| Item | Notes |
|---|---|
| MCU | Wemos D1 mini (ESP8266) **or** ESP32 |
| LEDs | WS2812B / NeoPixel strip (GRB, 800 kHz) |
| Data pin | **D4** — GPIO2 on ESP8266, GPIO4 on ESP32 |
| LED counts | configurable per segment (defaults `1, 1, 3, 7, 12` = 24) |
| Power | 5 V. For 24+ LEDs use a **separate 5 V supply** with a common ground, a 330–470 Ω resistor in series with the data line, and a 1000 µF capacitor across the strip. |

The five segments are daisy-chained in the order `1, 1, 2, 3, 5`. The number of
LEDs in each is whatever your build uses; set it in the web UI.

---

## Build & flash

### Arduino IDE
1. Install the **ESP8266** (or **ESP32**) board support in Boards Manager.
2. Install these libraries via Library Manager:
   - **WiFiManager** (tzapu)
   - **Adafruit NeoPixel**
   - **ArduinoJson** (v7)
3. Open `FibonacciClock.ino`.
4. Select the board:
   - ESP8266: **LOLIN(WEMOS) D1 R2 & mini**
   - ESP32: **ESP32 Dev Module** (or **WEMOS D1 MINI ESP32**)
5. Compile and upload. **No filesystem/SPIFFS upload is required** — the web UI
   is embedded in flash.

`config.h` contains the compile-time defaults (pin, AP name, hostname, NTP
servers, default timezone, `MAX_LEDS`, block values).

---

## First-time WiFi setup

On first boot (or after a WiFi reset) the clock starts an access point called
**FibonacciClock**:

1. Join the `FibonacciClock` WiFi network from a phone/laptop.
2. Open **http://192.168.4.1**.
3. Enter your WiFi SSID/password and save. The clock reboots and connects.
4. Find it at **http://fibonacci-clock.local** or its DHCP IP address.

The portal also lets you set the timezone, theme number and brightness.

---

## Web UI

- **Clock** — the live 5-square rendering, the time, the current hour/minute
  breakdown, and **How to read this?** (also a hover tooltip).
- **Display** — power, brightness, theme picker.
- **LED layout** — LED count for each Fibonacci segment, live total, "test
  segments on boot" toggle, **Save layout**, **Test segments**.
- **Time** — timezone dropdown (saves instantly) and a time-only manual setter.
- **System** — IP, signal, uptime, heap, firmware version; **Reboot**, **Reset
  WiFi**, and **Firmware update** (upload a `.bin`).

### HTTP API

| Method | Path | Body / query | Purpose |
|---|---|---|---|
| GET | `/api/state` | — | Current time, masks, colours, settings |
| GET | `/api/themes` | — | Theme list with colours |
| POST | `/api/config` | `{theme, brightness, on, bootTest, tz, seg:[5]}` | Update settings |
| POST | `/api/time` | `{hour, min}` | Set the time (keeps the date) |
| POST | `/api/test?mode=segments` | — | Run the segment chase |
| POST | `/api/reboot` | — | Reboot |
| POST | `/api/wifi/reset` | — | Erase WiFi settings and reboot to the portal |
| POST | `/update` | multipart `firmware` | OTA firmware upload |

Settings live in `/config.json` on LittleFS and survive reboots and reflashes.

---

## Repository layout

```
FibonacciClock.ino   main: setup/loop, NTP, orchestration
config.h             pins, LED limits, block values, defaults, version
themes.h             the 22 colour themes
settings.h           LittleFS load/save/validate
clock.h              NeoPixel output, Fibonacci mapping, fades, segment test
wifisetup.h          WiFiManager customisation
webapi.h             HTTP routes and handlers
webui.h              embedded HTML/CSS/JS (the web page)
CHANGELOG.md         version history
_backups/            timestamped snapshots taken before each change
images/              README assets
```

---

## Credits

Inspired by the original open-source **Fibonacci Clock** by Philippe Chrétien
([pchretien/fibo](https://github.com/pchretien/fibo)) and the geekoclock.com
manual. This firmware is an independent rewrite for ESP8266/ESP32 with WiFi,
NTP and a modern web UI.
