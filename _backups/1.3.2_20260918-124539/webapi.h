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
#include "themestore.h"
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

  const Theme& th = activeTheme();
  bool sta = (WiFi.status() == WL_CONNECTED);

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
  doc["ip"] = sta ? WiFi.localIP().toString() : WiFi.softAPIP().toString();
  doc["ssid"] = sta ? WiFi.SSID() : String(AP_NAME);
  doc["rssi"] = sta ? WiFi.RSSI() : 0;
  doc["wifiMode"] = sta ? "sta" : (apActive ? "ap" : "off");
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

static void sendThemes() {
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (uint8_t i = 0; i < gThemeCount; i++) {
    JsonObject o = arr.add<JsonObject>();
    o["id"] = i;
    o["name"] = gThemes[i].name;
    o["off"] = colorHex(gThemes[i].off);
    o["hour"] = colorHex(gThemes[i].hour);
    o["minute"] = colorHex(gThemes[i].minute);
    o["both"] = colorHex(gThemes[i].both);
  }
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static bool parseThemeColors(JsonDocument& doc, ThemeColor& off, ThemeColor& hour,
                             ThemeColor& minute, ThemeColor& both) {
  if (!parseHexColor(doc["off"] | "#ffffff", off)) return false;
  if (!parseHexColor(doc["hour"] | "#ff0000", hour)) return false;
  if (!parseHexColor(doc["minute"] | "#00ff00", minute)) return false;
  if (!parseHexColor(doc["both"] | "#0000ff", both)) return false;
  return true;
}

static void handleThemeCreate() {
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }
  ThemeColor off, hour, minute, both;
  if (!parseThemeColors(doc, off, hour, minute, both)) {
    server.send(400, "application/json", "{\"error\":\"bad colour\"}");
    return;
  }
  int id = themesAdd(doc["name"] | "Theme", off, hour, minute, both);
  if (id < 0) { server.send(400, "application/json", "{\"error\":\"theme limit reached\"}"); return; }
  sendThemes();
}

static void handleThemeUpdate() {
  int id = server.arg("id").toInt();
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }
  ThemeColor off, hour, minute, both;
  if (!parseThemeColors(doc, off, hour, minute, both)) {
    server.send(400, "application/json", "{\"error\":\"bad colour\"}");
    return;
  }
  if (!themesUpdate((uint8_t)id, doc["name"] | "Theme", off, hour, minute, both)) {
    server.send(400, "application/json", "{\"error\":\"bad theme id\"}");
    return;
  }
  clockForceRefresh = true;
  sendThemes();
}

static void handleThemeDelete() {
  int id = server.arg("id").toInt();
  if (!themesDelete((uint8_t)id)) {
    server.send(400, "application/json", "{\"error\":\"cannot delete (last theme?)\"}");
    return;
  }
  clockForceRefresh = true;
  sendThemes();
}

static void handleThemeReset() {
  themesReset();
  clockForceRefresh = true;
  sendThemes();
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
    settings.theme = (v < 0 || v >= gThemeCount) ? 0 : (uint8_t)v;
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

  settings.savedEpoch = (uint32_t)epoch;   // survive a power cycle
  settingsSave();
  applyCurrentTargets();

  sendState();
}

static void handleWifiScan() {
  bool wasAp = apActive;
  if (wasAp) WiFi.mode(WIFI_AP_STA);   // scanning needs the station enabled
  int n = WiFi.scanNetworks();
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();
  for (int i = 0; i < n; i++) {
    JsonObject o = arr.add<JsonObject>();
    o["ssid"] = WiFi.SSID(i);
    o["rssi"] = WiFi.RSSI(i);
#ifdef ESP32
    o["open"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN);
#else
    o["open"] = (WiFi.encryptionType(i) == ENC_TYPE_NONE);
#endif
  }
  WiFi.scanDelete();
  if (wasAp) WiFi.mode(WIFI_AP);
  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

static void handleWifiConnect() {
  if (!server.hasArg("plain")) { server.send(400, "application/json", "{\"error\":\"no body\"}"); return; }
  JsonDocument doc;
  if (deserializeJson(doc, server.arg("plain"))) {
    server.send(400, "application/json", "{\"error\":\"bad json\"}");
    return;
  }
  const char* ssid = doc["ssid"] | "";
  const char* pass = doc["pass"] | "";
  if (!ssid[0]) { server.send(400, "application/json", "{\"error\":\"ssid required\"}"); return; }
  strncpy(settings.wifiSsid, ssid, sizeof(settings.wifiSsid) - 1);
  settings.wifiSsid[sizeof(settings.wifiSsid) - 1] = 0;
  strncpy(settings.wifiPass, pass, sizeof(settings.wifiPass) - 1);
  settings.wifiPass[sizeof(settings.wifiPass) - 1] = 0;
  settingsSave();
  server.send(200, "application/json", "{\"ok\":true,\"rebooting\":true}");
  delay(300);
  ESP.restart();   // reboot into a clean pure-STA connect
}

static void handleReboot() {
  server.send(200, "application/json", "{\"ok\":true}");
  delay(200);
  ESP.restart();
}

static void handleWifiReset() {
  server.send(200, "application/json", "{\"ok\":true}");
  delay(200);
  wifiForget();
  delay(200);
  ESP.restart();
}

static void setupWeb() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, sendState);
  server.on("/api/themes", HTTP_GET, sendThemes);
  server.on("/api/themes", HTTP_POST, handleThemeCreate);
  server.on("/api/themes/update", HTTP_POST, handleThemeUpdate);
  server.on("/api/themes/delete", HTTP_POST, handleThemeDelete);
  server.on("/api/themes/reset", HTTP_POST, handleThemeReset);
  server.on("/api/config", HTTP_POST, handleConfig);
  server.on("/api/test", HTTP_GET, handleTest);
  server.on("/api/test", HTTP_POST, handleTest);
  server.on("/api/time", HTTP_POST, handleTime);
  server.on("/api/wifi/scan", HTTP_GET, handleWifiScan);
  server.on("/api/wifi", HTTP_POST, handleWifiConnect);
  server.on("/api/reboot", HTTP_POST, handleReboot);
  server.on("/api/wifi/reset", HTTP_POST, handleWifiReset);
  httpUpdater.setup(&server);
  server.onNotFound([]() { server.send(404, "text/plain", "Not found"); });
  server.begin();
  Serial.println("[web] server started");
}
