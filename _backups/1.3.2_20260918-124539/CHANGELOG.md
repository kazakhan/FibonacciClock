# Changelog

All notable changes to this project are documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/), and this
project uses [Semantic Versioning](https://semver.org/).

Entries are newest-first. Previous entries are never modified.

## [1.3.1] - 2026-09-18

### Fixed
- **Boot WiFi connection** on ESP8266: set `WIFI_NONE_SLEEP` and maximum output
  power before joining, which cuts association from ~30 s to a few seconds.
- **Removed AP+STA operation.** Running the setup AP and the STA together could
  crash the ESP8266 lwIP stack (`Exception (0)` in `lmacTxFrame`). The clock now
  uses pure STA on boot and falls back to a pure setup AP.
- The setup AP is only stopped once the STA has a **valid IP**, not merely a
  connected status.
- Offline time is now saved on the **first** successful NTP sync (and every
  15 min), not only after 15 minutes.
- WiFi card **Connect** now saves the credentials and reboots into a clean STA
  connect (avoids the unstable AP+STA path).

### Changed
- `FW_VERSION` → `1.3.1`.
- Verified on hardware: AP mode serves the full UI (`/`, `/api/state`,
  theme CRUD, WiFi scan); theme create/update/delete works over the AP.

## [1.3.0] - 2026-09-18

### Added
- **Editable themes**: create, edit and delete themes from the web UI (colour
  pickers for hours/minutes/both/off + name), with **Restore defaults**.
  Themes are stored at `/themes.json` on LittleFS.
- **Own WiFi management** — the clock's web UI is now reachable in AP mode.
  If no WiFi is configured (or it can't connect) the device starts the
  **FibonacciClock** access point with a captive portal at **192.168.4.1** and
  serves the full UI; a **WiFi card** lets you scan and join a network.
- **Offline time**: the last known epoch is persisted (on NTP sync, manual set,
  and every 15 min) and restored at boot so the clock runs without WiFi.
- New API: `POST /api/themes`, `POST /api/themes/update`, `POST /api/themes/delete`,
  `POST /api/themes/reset`, `GET /api/wifi/scan`, `POST /api/wifi`.

### Changed
- **Removed the WiFiManager dependency** (its portal owned port 80 and blocked
  our page). WiFi credentials now live in `/config.json`.
- `GET /api/themes` now includes `id` and `off`; `GET /api/state` adds
  `ssid` and `wifiMode`.
- `FW_VERSION` bumped to `1.3.0`.

## [1.2.1] - 2026-09-17

### Added
- **LICENSE**: the project is now released under the **GNU General Public
  License v2.0 (GPL-2.0)**.

### Changed
- README: added a License section and clarified the credits.

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
