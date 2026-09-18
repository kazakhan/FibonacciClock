/*
 * Fibonacci Clock
 * ----------------
 * ESP8266 (Wemos D1 mini) / ESP32
 * Addressable LEDs on D4, no buttons.
 *
 *   Fibonacci : 1  2  3  5   8     (fixed values)
 *   LEDs      : configurable per segment from the web UI
 *
 * WiFiManager config portal, hourly NTP sync, colour themes,
 * modern responsive web UI, OTA updates.
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
#include "clock.h"
#include "wifisetup.h"
#include "webui.h"
#include "webapi.h"

static unsigned long lastNtp = 0;
static uint8_t lastHour = 255;
static uint8_t lastMin5 = 255;

static void startNtp() {
  configTzTime(settings.tz, NTP_SERVER1, NTP_SERVER2);
  lastNtp = millis();
  Serial.printf("[ntp] sync requested (tz=%s)\n", settings.tz);
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
  clockBegin();

  if (settings.bootTest) {
    clockTestRunBlocking();   // light each segment in turn
  }
  clockFill(8, 0, 16);        // dim purple while connecting

  setupWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    MDNS.begin(settings.hostname);
    MDNS.addService("http", "tcp", 80);
    startNtp();
  }

  setupWeb();
  setupOta();

  applyCurrentTargets();
  Serial.printf("[sys] ready  http://%s.local  (%s)\n",
                settings.hostname, WiFi.localIP().toString().c_str());
}

void loop() {
  server.handleClient();
  ArduinoOTA.handle();

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

  if (WiFi.status() == WL_CONNECTED && millis() - lastNtp > NTP_RESYNC_MS) {
    startNtp();
  }

  delay(2);
}
