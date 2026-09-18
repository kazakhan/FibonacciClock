# KazaKhan's Fibonacci Clock

A WiFi-connected **Fibonacci clock** built on an ESP8266 (Wemos D1 mini) or
ESP32, driving a WS2812/NeoPixel strip. Hourly NTP sync, editable colour themes,
OTA updates, and a fast, responsive web UI that explains itself. **WiFi is
optional** — with no network configured it starts its own access point and serves
the same page, so you can use and configure the clock from any phone.

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
- **WiFi is optional**: connect to your network, or use the built-in **setup AP**
  (`FibonacciClock` → `http://192.168.4.1`) with the full web UI and a WiFi card
  to scan/join networks later. No buttons.
- **Hourly NTP** sync with a selectable timezone (~32 zones).
- **Editable themes**: create, edit and delete themes (with colour pickers),
  plus a one-click **Restore defaults**. 22 built in (RGB, Mondrian, 80s,
  Pastel, Neon, Cyberpunk, …), off blocks lit white.
- **Offline time**: the last known time is persisted and restored, so the clock
  keeps running without WiFi.
- **Runtime-configurable LEDs per segment** with a live 5-square preview.
- **Segment chase test** on boot (toggleable) and on demand.
- **OTA** updates (ArduinoOTA and a browser `/update` page).
- Settings and themes persisted to LittleFS.
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

## First-time WiFi setup (optional)

WiFi is not required. On first boot (or if the saved network can't be reached)
the clock starts an access point called **FibonacciClock**:

1. Join the `FibonacciClock` WiFi network from a phone/laptop.
2. Open **http://192.168.4.1** (a captive-portal popup usually appears).
3. The **whole clock UI works here** — set themes, LED counts, brightness, time.
4. To join a network, open the **WiFi** card, tap **Scan**, pick your network,
   enter the password and **Connect**. Once it connects, the setup AP stops.
5. The clock is then at **http://fibonacci-clock.local** or its DHCP IP.

If no WiFi/NTP is available, set the time manually (Time card) — the last known
time is saved and restored on the next boot so the clock keeps running offline.

> **Note:** on boot the clock tries the saved network once (pure STA); if it
> can't associate within ~25 s it automatically starts the setup AP, so the web
> UI is always reachable. A weak 2.4 GHz signal can make joining slow or
> unreliable — the AP fallback means the clock still works. Joining uses no
> WiFi sleep and maximum output power for the best chance of a quick connect.

---

## Web UI

- **Clock** — the live 5-square rendering, the time, the current hour/minute
  breakdown, and **How to read this?** (also a hover tooltip).
- **Display** — power, brightness, and the theme grid. Each theme has **✎ edit**
  and **× delete** actions, plus a **+ New** tile that opens the theme editor
  (name + colour pickers). **Restore defaults** brings back the 22 built-ins.
- **LED layout** — LED count for each Fibonacci segment, live total, "test
  segments on boot" toggle, **Save layout**, **Test segments**.
- **Time** — timezone dropdown (saves instantly) and a time-only manual setter.
- **WiFi** — status, **Scan**, network name + password, **Connect**.
- **System** — IP, signal, uptime, heap, firmware version; **Reboot**, **Forget
  WiFi**, and **Firmware update** (upload a `.bin`).

### HTTP API

| Method | Path | Body / query | Purpose |
|---|---|---|---|
| GET | `/api/state` | — | Time, masks, colours, WiFi status, settings |
| GET | `/api/themes` | — | Theme list (`id`, `name`, colours) |
| POST | `/api/themes` | `{name,off,hour,minute,both}` | Create a theme |
| POST | `/api/themes/update?id=N` | same | Edit theme `N` |
| POST | `/api/themes/delete?id=N` | — | Delete theme `N` |
| POST | `/api/themes/reset` | — | Restore the default themes |
| POST | `/api/config` | `{theme, brightness, on, bootTest, tz, seg:[5]}` | Update settings |
| POST | `/api/time` | `{hour, min}` | Set the time (keeps the date) |
| POST | `/api/test?mode=segments` | — | Run the segment chase |
| GET | `/api/wifi/scan` | — | Scan for networks |
| POST | `/api/wifi` | `{ssid, pass}` | Join a network |
| POST | `/api/reboot` | — | Reboot |
| POST | `/api/wifi/reset` | — | Forget WiFi and reboot into setup AP |
| POST | `/update` | multipart `firmware` | OTA firmware upload |

Settings live in `/config.json` and themes in `/themes.json` on LittleFS; both
survive reboots and reflashes.

---

## Repository layout

```
FibonacciClock.ino   main: setup/loop, NTP, orchestration
config.h             pins, LED limits, block values, defaults, version
themes.h             theme structs + default themes
themestore.h         runtime theme store (load/save/CRUD) -> /themes.json
settings.h           LittleFS load/save/validate -> /config.json
clock.h              NeoPixel output, Fibonacci mapping, fades, segment test
wifisetup.h          own WiFi manager + setup AP + captive portal
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

## License

This project is released under the **GNU General Public License v2.0**
(GPL-2.0). See [LICENSE](LICENSE) for the full text.

```
Copyright (C) 2026 Jamie Saunders (KazaKhan)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
```
