#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "themes.h"

struct Settings {
  uint8_t theme;
  uint8_t brightness;
  bool on;
  bool bootTest;
  uint8_t segCount[SEG_COUNT];
  char tz[64];
  char hostname[32];
  char wifiSsid[33];
  char wifiPass[65];
  uint32_t savedEpoch;
};

static Settings settings;

static uint16_t settingsTotalLeds() {
  uint16_t t = 0;
  for (uint8_t i = 0; i < SEG_COUNT; i++) t += settings.segCount[i];
  return t;
}

static bool settingsValidSegments() {
  uint16_t total = 0;
  for (uint8_t i = 0; i < SEG_COUNT; i++) {
    if (settings.segCount[i] < 1) return false;
    total += settings.segCount[i];
  }
  return total >= 1 && total <= MAX_LEDS;
}

static void settingsDefaults() {
  settings.theme = DEFAULT_THEME;
  settings.brightness = DEFAULT_BRIGHTNESS;
  settings.on = true;
  settings.bootTest = DEFAULT_BOOT_TEST;
  for (uint8_t i = 0; i < SEG_COUNT; i++) settings.segCount[i] = DEFAULT_SEG_COUNTS[i];
  strncpy(settings.tz, DEFAULT_TZ, sizeof(settings.tz) - 1);
  settings.tz[sizeof(settings.tz) - 1] = 0;
  strncpy(settings.hostname, HOSTNAME, sizeof(settings.hostname) - 1);
  settings.hostname[sizeof(settings.hostname) - 1] = 0;
  settings.wifiSsid[0] = 0;
  settings.wifiPass[0] = 0;
  settings.savedEpoch = 0;
}

static void settingsSave();

static void fsBegin() {
#ifdef ESP32
  if (!LittleFS.begin(true)) {
    Serial.println("[fs] LittleFS mount failed");
  }
#else
  if (!LittleFS.begin()) {
    LittleFS.format();
    LittleFS.begin();
  }
#endif
}

static void settingsLoad() {
  settingsDefaults();
  fsBegin();

  File f = LittleFS.open("/config.json", "r");
  if (!f) {
    Serial.println("[cfg] no config, using defaults");
    return;
  }

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) {
    Serial.println("[cfg] parse failed, using defaults");
    return;
  }

  settings.theme = doc["theme"] | DEFAULT_THEME;
  settings.brightness = doc["brightness"] | DEFAULT_BRIGHTNESS;
  settings.on = doc["on"] | true;
  settings.bootTest = doc["bootTest"] | DEFAULT_BOOT_TEST;

  JsonArray seg = doc["seg"].as<JsonArray>();
  if (!seg.isNull() && seg.size() == SEG_COUNT) {
    for (uint8_t i = 0; i < SEG_COUNT; i++) {
      int v = seg[i] | 0;
      settings.segCount[i] = (uint8_t)constrain(v, 0, MAX_LEDS);
    }
  }

  const char* tz = doc["tz"] | DEFAULT_TZ;
  strncpy(settings.tz, tz, sizeof(settings.tz) - 1);
  settings.tz[sizeof(settings.tz) - 1] = 0;

  const char* hn = doc["hostname"] | HOSTNAME;
  strncpy(settings.hostname, hn, sizeof(settings.hostname) - 1);
  settings.hostname[sizeof(settings.hostname) - 1] = 0;

  const char* ws = doc["wifiSsid"] | "";
  strncpy(settings.wifiSsid, ws, sizeof(settings.wifiSsid) - 1);
  settings.wifiSsid[sizeof(settings.wifiSsid) - 1] = 0;
  const char* wp = doc["wifiPass"] | "";
  strncpy(settings.wifiPass, wp, sizeof(settings.wifiPass) - 1);
  settings.wifiPass[sizeof(settings.wifiPass) - 1] = 0;
  settings.savedEpoch = doc["savedEpoch"] | 0;

  // Guard against a corrupted/empty config (e.g. after a failed portal save).
  if (settings.tz[0] == 0 || settings.hostname[0] == 0 || !settingsValidSegments()) {
    Serial.println("[cfg] invalid config, resetting to defaults");
    settingsDefaults();
    settingsSave();
  }

  Serial.printf("[cfg] loaded theme=%u bright=%u on=%d bootTest=%d seg=%u,%u,%u,%u,%u tz=%s\n",
                settings.theme, settings.brightness, settings.on, settings.bootTest,
                settings.segCount[0], settings.segCount[1], settings.segCount[2],
                settings.segCount[3], settings.segCount[4], settings.tz);
}

static void settingsSave() {
  JsonDocument doc;
  doc["theme"] = settings.theme;
  doc["brightness"] = settings.brightness;
  doc["on"] = settings.on;
  doc["bootTest"] = settings.bootTest;
  JsonArray seg = doc["seg"].to<JsonArray>();
  for (uint8_t i = 0; i < SEG_COUNT; i++) seg.add(settings.segCount[i]);
  doc["tz"] = settings.tz;
  doc["hostname"] = settings.hostname;
  doc["wifiSsid"] = settings.wifiSsid;
  doc["wifiPass"] = settings.wifiPass;
  doc["savedEpoch"] = settings.savedEpoch;

  File f = LittleFS.open("/config.json", "w");
  if (!f) {
    Serial.println("[cfg] save failed");
    return;
  }
  serializeJson(doc, f);
  f.close();
}
