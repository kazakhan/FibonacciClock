/*
 * KazaKhan's Fibonacci Clock
 * --------------------------
 * ESP8266 (Wemos D1 mini) / ESP32
 * Addressable LEDs on D4, no buttons.
 *
 *   Fibonacci : 1  2  3  5   8     (fixed values)
 *   LEDs      : configurable per segment from the web UI
 *
 * The web UI is always reachable: if no WiFi is configured the clock starts an
 * access point (192.168.4.1) and serves the same page. Hourly NTP sync,
 * editable themes, OTA updates.
 */

#include <ArduinoOTA.h>
#ifdef ESP32
  #include <ESPmDNS.h>
#else
  #include <ESP8266mDNS.h>
#endif

#include "config.h"
#include "themes.h"
#include "settings.h"
#include "themestore.h"
#include "clock.h"
#include "wifisetup.h"
#include "webui.h"
#include "webapi.h"

static unsigned long lastNtp = 0;
static unsigned long lastEpochSave = 0;
static bool epochSaved = false;
static uint8_t lastHour = 255;
static uint8_t lastMin5 = 255;
static bool netStarted = false;

#define EPOCH_SAVE_MS (15UL * 60UL * 1000UL)

static void startNtp() {
  configTzTime(settings.tz, NTP_SERVER1, NTP_SERVER2);
  lastNtp = millis();
  Serial.printf("[ntp] sync requested (tz=%s)\n", settings.tz);
}

static void saveEpoch() {
  time_t now = time(nullptr);
  if (now > (time_t)TIME_VALID_EPOCH) {
    settings.savedEpoch = (uint32_t)now;
    settingsSave();
    Serial.printf("[time] saved epoch %u\n", settings.savedEpoch);
  }
}

static void setupOta() {
  ArduinoOTA.setHostname(settings.hostname);
  ArduinoOTA.onStart([]() {
    Serial.println("[ota] start");
    clockFill(0, 0, 60);
  });
  ArduinoOTA.onEnd([]() { Serial.println("[ota] done"); });
  ArduinoOTA.onError([](ota_error_t e) { Serial.printf("[ota] error %u\n", e); });
  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println();

#ifdef ESP8266
  uint32_t bootCount = 0;
  ESP.rtcUserMemoryRead(0, &bootCount, sizeof(bootCount));
  bootCount++;
  ESP.rtcUserMemoryWrite(0, &bootCount, sizeof(bootCount));
  Serial.printf("=== Fibonacci Clock %s  boot #%u  reset=%s  heap=%u ===\n",
                FW_VERSION, bootCount, ESP.getResetReason().c_str(), ESP.getFreeHeap());
#else
  Serial.printf("=== Fibonacci Clock %s  reset=%d  heap=%u ===\n",
                FW_VERSION, (int)esp_reset_reason(), ESP.getFreeHeap());
#endif

  settingsLoad();
  themesLoad();

  // Restore the last known time so the clock runs without WiFi/NTP.
  if (settings.savedEpoch > TIME_VALID_EPOCH) {
    struct timeval tv;
    tv.tv_sec = (time_t)settings.savedEpoch;
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
    Serial.printf("[time] restored epoch %u\n", settings.savedEpoch);
  }
  configTzTime(settings.tz, NTP_SERVER1, NTP_SERVER2);   // set TZ even offline

  clockBegin();

  if (settings.bootTest) {
    clockTestRunBlocking();   // light each segment in turn
  }
  clockFill(8, 0, 16);        // dim purple while connecting

  wifiBegin();

  setupWeb();
  setupOta();

  applyCurrentTargets();
  Serial.printf("[sys] ready  http://%s  (%s)\n", settings.hostname,
                WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString().c_str()
                                              : WiFi.softAPIP().toString().c_str());
}

void loop() {
  server.handleClient();
  wifiLoop();
  ArduinoOTA.handle();

  if (wifiOnline()) {
    if (!netStarted) {
      netStarted = true;
      MDNS.begin(settings.hostname);
      MDNS.addService("http", "tcp", 80);
      startNtp();
      Serial.printf("[net] online: %s\n", WiFi.localIP().toString().c_str());
    }
    if (millis() - lastNtp > NTP_RESYNC_MS) startNtp();
    time_t now = time(nullptr);
    if (now > (time_t)TIME_VALID_EPOCH && (!epochSaved || millis() - lastEpochSave > EPOCH_SAVE_MS)) {
      epochSaved = true;
      lastEpochSave = millis();
      saveEpoch();
    }
  } else {
    netStarted = false;
  }

  if (clockTestActive()) {
    clockTestTick();
  } else {
    clockTick();

    if (clockForceRefresh) {
      clockForceRefresh = false;
      lastHour = 255;
      lastMin5 = 255;
    }

    time_t now = time(nullptr);
    if (now > (time_t)TIME_VALID_EPOCH) {
      struct tm t;
      localtime_r(&now, &t);
      if (t.tm_hour != lastHour || (t.tm_min / 5) != lastMin5) {
        lastHour = t.tm_hour;
        lastMin5 = t.tm_min / 5;
        clockUpdateTargets(t.tm_hour, t.tm_min);
      }
    } else if (lastHour != 255) {
      lastHour = 255;
      lastMin5 = 255;
      clockUpdateTargets(0, 0);
    }
  }

  delay(2);
}
