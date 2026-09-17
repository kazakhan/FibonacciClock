# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/), and this
project uses [Semantic Versioning](https://semver.org/).

Entries are newest-first. Previous entries are never modified.

## [1.2.0] - 2026-09-17

### Added
- "How to read this clock" popup that explains the clock using the **current
  theme colours and the current time**: Fibonacci block values, a colour legend
  with live swatches, and the live hours/minutes sums.
- Hover tooltip on the clock graphic summarising the current reading.
- `#howto` deep link that opens the explainer directly.
- Header logo (`NoSmokingJim.png`) and a PNG data-URI favicon.
- `README.md`, `CHANGELOG.md`, `_backups/` and `.gitignore`.

### Changed
- Page title / header renamed to **"KazaKhan's Fibonacci Clock"**.
- Timezone selection is now a **dropdown of ~32 zones** (auto-saves on change).
- Manual time entry is **time-only** (the current date is kept); no date setting.
- Brightness changes now apply to the physical LEDs immediately.
- Segment editor no longer loses edits to the 1-second status poll, and save
  errors are surfaced instead of a false "Saved".

## [1.1.0] - 2026-09-17

### Added
- Runtime-configurable **LEDs per segment** (web UI) with a live 5-square preview.
- **Segment chase test** on boot (toggleable) plus a "Test segments" button.
- OTA firmware upload, WiFi reset, and a live clock preview on the web page.
- Theme picker with 22 themes.
- Timezone setting, persisted to LittleFS.

### Changed
- Replaced the ring preview with the classic **8×5 five-square layout**
  (2 top-left, two 1s top-middle, 3 bottom-left, 5 right).
- Corrected the block values to **1, 1, 2, 3, 5**.
- Inactive blocks are lit **white**, matching the original clock.
- Settings are persisted to LittleFS (`/config.json`).

## [1.0.0] - 2026-09-17

### Added
- Initial rewrite for **ESP8266 (Wemos D1 mini)** and **ESP32**.
- WiFiManager configuration portal (customised) and hourly NTP synchronisation.
- Adafruit NeoPixel output on D4, Fibonacci hour/minute display with fades.
- Embedded responsive web UI, themes, and OTA updates.
