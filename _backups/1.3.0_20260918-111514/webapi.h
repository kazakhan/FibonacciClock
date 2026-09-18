#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>
#include <sys/time.h>

#ifdef ESP32
  #include <WebServer.h>
  #include <HTTPUpdateServer.h>
  static WebServer server(80);
  static HTTPUpdateServer httpUpdater;
#else
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPUpdateServer.h>
  static ESP8266WebServer server(80);
  static ESP8266HTTPUpdateServer httpUpdater;
#endif

#include "config.h"
#include "themes.h"
#include "settings.h"
#include "clock.h"
#include "wifisetup.h"
#include "webui.h"

static String colorHex(const ThemeColor& c) {
  char b[8];
  snprintf(b, sizeof(b), "#%02x%02x%02x", c.r, c.g, c.b);
  return String(b);
}

static void applyCurrentTargets() {
  time_t now = time(nullptr);
  if (now > (time_t)TIME_VALID_EPOCH) {
    struct tm t;
    localtime_r(&now, &t);
    clockUpdateTargets(t.tm_hour, t.tm_min);
  } else {
    clockUpdateTargets(0, 0);
  }
}

static void sendState() {
  time_t now = time(nullptr);
  bool synced = now > (time_t)TIME_VALID_EPOCH;
  struct tm t = {};
  if (synced) localtime_r(&now, &t);

  uint8_t hourMask = 0, minMask = 0;
  if (synced) {
    uint8_t h12 = t.tm_hour % 12;
    if (h12 == 0) h12 = 12;
    hourMask = valueToMask(h12);
    minMask = valueToMask(t.tm_min / 5);
  }

  const Theme& th = THEMES[settings.theme % THEME_COUNT];

  JsonDocument doc;
  doc["synced"] = synced;
  doc["epoch"] = (uint32_t)now;
  doc["year"] = synced ? (t.tm_year + 1900) : 0;
  doc["mon"] = synced ? (t.tm_mon + 1) : 0;
  doc["day"] = synced ? t.tm_mday : 0;
  doc["wday"] = synced ? t.tm_wday : 0;
  doc["hour"] = synced ? t.tm_hour : 0;
  doc["minute"] = synced ? t.tm_min : 0;
  doc["second"] = synced ? t.tm_sec : 0;
  doc["hourMask"] = hourMask;
  doc["minMask"] = minMask;
  doc["theme"] = settings.theme;
  doc["themeName"] = th.name;
  doc["brightness"] = settings.brightness;
  doc["on"] = settings.on;
  doc["bootTest"] = settings.bootTest;
  doc["test"] = clockTestActive();
  doc["total"] = totalLeds;
  doc["maxleds"] = MAX_LEDS;
  JsonArray seg = doc["seg"].to<JsonArray>();
  for (uint8_t i = 0; i < SEG_COUNT; i++) seg.add(settings.segCount[i]);
  doc["tz"] = settings.tz;
  doc["name"] = settings.hostname;
  doc["ip"] = WiFi.localIP().toString();
  doc["rssi"] = WiFi.RSSI();
  doc["uptime"] = millis() / 1000;
  doc["heap"] = ESP.getFreeHeap();
  doc["fw"] = FW_VERSION;

  JsonObject c = doc["colors"].to<JsonObject>();
  c["off"] = colorHex(th.off);
  c["hour"] = colorHex(th.hour);
  c["minute"] = colorHex(th.minute);
  c["both"] = colorHex(th.both);

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

static void handleThemes() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (uint8_t i = 0; i < THEME_COUNT; i++) {
    JsonObject o = arr.add<JsonObject>();
    o["name"] = THEMES[i].name;
    o["hour"] = colorHex(THEMES[i].hour);
    o["minute"] = colorHex(THEMES[i].minute);
    o["both"] = colorHex(THEMES[i].both);
  }
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleConfig() {
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }

  if (!doc["theme"].isNull()) {
    int v = doc["theme"];
    settings.theme = (v < 0 || v >= THEME_COUNT) ? 0 : (uint8_t)v;
  }
  if (!doc["brightness"].isNull()) {
    settings.brightness = (uint8_t)constrain((int)doc["brightness"], 0, 255);
  }
  if (!doc["on"].isNull()) {
    settings.on = (bool)doc["on"];
  }
  if (!doc["bootTest"].isNull()) {
    settings.bootTest = (bool)doc["bootTest"];
  }

  bool segChanged = false;
  JsonArray seg = doc["seg"].as<JsonArray>();
  if (!seg.isNull()) {
    if (seg.size() != SEG_COUNT) {
      server.send(400, "application/json", "{\"error\":\"seg must have 5 values\"}");
      return;
    }
    uint8_t tmp[SEG_COUNT];
    uint16_t total = 0;
    bool ok = true;
    for (uint8_t i = 0; i < SEG_COUNT; i++) {
      int v = seg[i] | -1;
      if (v < 1 || v > MAX_LEDS) { ok = false; break; }
      tmp[i] = (uint8_t)v;
      total += v;
    }
    if (!ok || total < 1 || total > MAX_LEDS) {
      server.send(400, "application/json", "{\"error\":\"invalid seg counts\"}");
      return;
    }
    for (uint8_t i = 0; i < SEG_COUNT; i++) {
      if (settings.segCount[i] != tmp[i]) segChanged = true;
      settings.segCount[i] = tmp[i];
    }
  }

  bool tzChanged = false;
  if (!doc["tz"].isNull()) {
    const char* tz = doc["tz"];
    if (tz[0]) {
      strncpy(settings.tz, tz, sizeof(settings.tz) - 1);
      settings.tz[sizeof(settings.tz) - 1] = 0;
      tzChanged = true;
    }
  }

  settingsSave();
  if (segChanged) {
    clockRebuild();
    clockForceRefresh = true;
  }
  if (tzChanged) {
    configTzTime(settings.tz, NTP_SERVER1, NTP_SERVER2);
    clockForceRefresh = true;
  }
  clockSetBrightness(settings.brightness);   // applies immediately (setBrightness + push)
  applyCurrentTargets();

  sendState();
}

static void handleTest() {
  String mode = server.hasArg("mode") ? server.arg("mode") : "";
  if (mode == "segments" || mode == "segment" || mode == "on") {
    clockTestStart();
  } else if (mode == "off" || mode == "stop") {
    clockTestStop();
  } else {
    server.send(400, "application/json", "{\"error\":\"mode must be segments or off\"}");
    return;
  }
  sendState();
}

static void handleTime() {
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }

  time_t epoch;
  if (!doc["epoch"].isNull()) {
    epoch = (time_t)(uint32_t)doc["epoch"];
  } else {
    struct tm t;
    if (!doc["year"].isNull()) {
      t = {};
      t.tm_year = (int)doc["year"] - 1900;
      t.tm_mon = (int)(doc["mon"] | 1) - 1;
      t.tm_mday = (int)(doc["day"] | 1);
    } else {
      time_t now = time(nullptr);
      localtime_r(&now, &t);   // keep the current local date
    }
    t.tm_hour = (int)(doc["hour"] | 0);
    t.tm_min = (int)(doc["min"] | 0);
    t.tm_sec = 0;
    t.tm_isdst = -1;
    epoch = mktime(&t);
  }

  struct timeval tv;
  tv.tv_sec = epoch;
  tv.tv_usec = 0;
  settimeofday(&tv, nullptr);
  applyCurrentTargets();

  sendState();
}

static void handleReboot() {
  server.send(200, "application/json", "{\"ok\":true}");
  delay(200);
  ESP.restart();
}

static void handleWifiReset() {
  server.send(200, "application/json", "{\"ok\":true}");
  delay(200);
  wm.resetSettings();
  delay(200);
  ESP.restart();
}

static void setupWeb() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, sendState);
  server.on("/api/themes", HTTP_GET, handleThemes);
  server.on("/api/config", HTTP_POST, handleConfig);
  server.on("/api/test", HTTP_GET, handleTest);
  server.on("/api/test", HTTP_POST, handleTest);
  server.on("/api/time", HTTP_POST, handleTime);
  server.on("/api/reboot", HTTP_POST, handleReboot);
  server.on("/api/wifi/reset", HTTP_POST, handleWifiReset);
  httpUpdater.setup(&server);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.println("[web] server started");
}
