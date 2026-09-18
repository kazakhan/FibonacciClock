#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "config.h"
#include "themes.h"
#include "settings.h"

// Runtime, editable, persisted theme store.
static Theme gThemes[MAX_THEMES];
static uint8_t gThemeCount = 0;

static String colorToHex(const ThemeColor& c) {
  char b[8];
  snprintf(b, sizeof(b), "#%02x%02x%02x", c.r, c.g, c.b);
  return String(b);
}

static bool parseHexColor(const char* s, ThemeColor& c) {
  if (!s) return false;
  if (*s == '#') s++;
  if (strlen(s) < 6) return false;
  auto hx = [](char ch) -> int {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    return -1;
  };
  int v[6];
  for (int i = 0; i < 6; i++) {
    v[i] = hx(s[i]);
    if (v[i] < 0) return false;
  }
  c.r = (v[0] << 4) | v[1];
  c.g = (v[2] << 4) | v[3];
  c.b = (v[4] << 4) | v[5];
  return true;
}

static void themesSetName(Theme& t, const char* name) {
  if (!name || !name[0]) name = "Theme";
  strncpy(t.name, name, THEME_NAME_LEN - 1);
  t.name[THEME_NAME_LEN - 1] = 0;
}

static void themesSeedDefaults() {
  gThemeCount = 0;
  for (uint8_t i = 0; i < DEFAULT_THEME_COUNT && i < MAX_THEMES; i++) {
    Theme& t = gThemes[gThemeCount];
    themesSetName(t, DEFAULT_THEME_DEFS[i].name);
    t.off = DEFAULT_THEME_DEFS[i].off;
    t.hour = DEFAULT_THEME_DEFS[i].hour;
    t.minute = DEFAULT_THEME_DEFS[i].minute;
    t.both = DEFAULT_THEME_DEFS[i].both;
    gThemeCount++;
  }
}

static void themesSave() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (uint8_t i = 0; i < gThemeCount; i++) {
    JsonObject o = arr.add<JsonObject>();
    o["name"] = gThemes[i].name;
    o["off"] = colorToHex(gThemes[i].off);
    o["hour"] = colorToHex(gThemes[i].hour);
    o["minute"] = colorToHex(gThemes[i].minute);
    o["both"] = colorToHex(gThemes[i].both);
  }
  File f = LittleFS.open("/themes.json", "w");
  if (!f) { Serial.println("[thm] save failed"); return; }
  serializeJson(doc, f);
  f.close();
}

static void themesLoad() {
  File f = LittleFS.open("/themes.json", "r");
  if (!f) {
    themesSeedDefaults();
    themesSave();
    Serial.printf("[thm] seeded %u default themes\n", gThemeCount);
  } else {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, f);
    f.close();
    JsonArray arr = doc.as<JsonArray>();
    if (err || arr.isNull()) {
      themesSeedDefaults();
      themesSave();
      Serial.println("[thm] parse failed, seeded defaults");
    } else {
      gThemeCount = 0;
      for (JsonObject o : arr) {
        if (gThemeCount >= MAX_THEMES) break;
        Theme& t = gThemes[gThemeCount];
        themesSetName(t, o["name"] | "Theme");
        if (!parseHexColor(o["off"] | "#ffffff", t.off)) t.off = {255, 255, 255};
        if (!parseHexColor(o["hour"] | "#ff0000", t.hour)) t.hour = {255, 0, 0};
        if (!parseHexColor(o["minute"] | "#00ff00", t.minute)) t.minute = {0, 255, 0};
        if (!parseHexColor(o["both"] | "#0000ff", t.both)) t.both = {0, 0, 255};
        gThemeCount++;
      }
      if (gThemeCount == 0) { themesSeedDefaults(); themesSave(); }
      Serial.printf("[thm] loaded %u themes\n", gThemeCount);
    }
  }
  if (gThemeCount == 0) themesSeedDefaults();
  if (settings.theme >= gThemeCount) settings.theme = 0;
}

static const Theme& activeTheme() {
  return gThemes[settings.theme % gThemeCount];
}

// Returns the new index, or -1 if the store is full.
static int themesAdd(const char* name, const ThemeColor& off, const ThemeColor& hour,
                     const ThemeColor& minute, const ThemeColor& both) {
  if (gThemeCount >= MAX_THEMES) return -1;
  Theme& t = gThemes[gThemeCount];
  themesSetName(t, name);
  t.off = off; t.hour = hour; t.minute = minute; t.both = both;
  gThemeCount++;
  themesSave();
  return gThemeCount - 1;
}

static bool themesUpdate(uint8_t id, const char* name, const ThemeColor& off,
                         const ThemeColor& hour, const ThemeColor& minute, const ThemeColor& both) {
  if (id >= gThemeCount) return false;
  Theme& t = gThemes[id];
  themesSetName(t, name);
  t.off = off; t.hour = hour; t.minute = minute; t.both = both;
  themesSave();
  return true;
}

static bool themesDelete(uint8_t id) {
  if (id >= gThemeCount || gThemeCount <= 1) return false;
  for (uint8_t i = id; i < gThemeCount - 1; i++) gThemes[i] = gThemes[i + 1];
  gThemeCount--;
  if (settings.theme == id) settings.theme = 0;
  else if (settings.theme > id) settings.theme--;
  if (settings.theme >= gThemeCount) settings.theme = 0;
  settingsSave();
  themesSave();
  return true;
}

static void themesReset() {
  themesSeedDefaults();
  settings.theme = 0;
  settingsSave();
  themesSave();
}
