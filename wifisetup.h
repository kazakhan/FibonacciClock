#pragma once
#include <WiFiManager.h>
#include "config.h"
#include "settings.h"
#include "clock.h"

// All strings handed to WiFiManager are kept in RAM (non-const static arrays).
// On ESP8266, flash-resident strings can land at unaligned addresses and the
// library reads them with word-wide loads, which faults with a LoadStoreError.
static char WM_HEAD_HTML[] =
  "<style>"
  "body{font-family:system-ui,-apple-system,'Segoe UI',Roboto,sans-serif;}"
  ".wrap{max-width:420px;margin:0 auto;}"
  "h1{letter-spacing:.3px;}"
  "button,input[type=submit]{border-radius:10px!important;}"
  "input,select{border-radius:10px!important;}"
  "</style>";

static char WM_MENU_HTML[] =
  "<div style='font-size:12px;opacity:.65;padding:6px 2px'>"
  "Fibonacci Clock " FW_VERSION "</div>";

static char WM_ID_TZ[] = "tz";
static char WM_LABEL_TZ[] = "Timezone (POSIX TZ)";
static char WM_ID_THEME[] = "theme";
static char WM_LABEL_THEME[] = "Theme number";
static char WM_ID_BRIGHT[] = "bright";
static char WM_LABEL_BRIGHT[] = "Brightness";
static char WM_CUSTOM_BRIGHT[] = "type=\"range\" min=\"0\" max=\"255\"";
static char WM_INFO_HTML[] =
  "<p style='opacity:.7;font-size:13px;margin-top:8px'>After connecting, open the "
  "clock page to pick themes visually.</p>";

static WiFiManager wm;
static WiFiManagerParameter* pTz = nullptr;
static WiFiManagerParameter* pTheme = nullptr;
static WiFiManagerParameter* pBright = nullptr;

static void wmApplySettings() {
  settingsSave();
  clockSetBrightness(settings.brightness);
}

// Called after the portal saves the custom parameters.
// Empty fields are ignored so that a plain WiFi-only save does not wipe them.
static void wmSaveParams() {
  if (pTz) {
    const char* v = pTz->getValue();
    if (v && v[0]) {
      strncpy(settings.tz, v, sizeof(settings.tz) - 1);
      settings.tz[sizeof(settings.tz) - 1] = 0;
    }
  }
  if (pTheme) {
    const char* v = pTheme->getValue();
    if (v && v[0]) {
      int x = atoi(v);
      settings.theme = (x < 0 || x >= THEME_COUNT) ? 0 : (uint8_t)x;
    }
  }
  if (pBright) {
    const char* v = pBright->getValue();
    if (v && v[0]) {
      int x = atoi(v);
      settings.brightness = (uint8_t)constrain(x, 0, 255);
    }
  }
  wmApplySettings();
  if (settings.tz[0]) configTzTime(settings.tz, NTP_SERVER1, NTP_SERVER2);
  Serial.println("[wm] params saved");
}

static void wmApCallback(WiFiManager* m) {
  (void)m;
  static bool reported = false;
  if (reported) return;
  reported = true;
  Serial.println("[wm] config portal started");
  clockFill(0, 0, 90);   // blue while the portal is open
}

static void setupWiFi() {
  wm.setDebugOutput(false);
  wm.setTitle("Fibonacci Clock");
  wm.setDarkMode(true);
  wm.setHostname(settings.hostname);
  wm.setConfigPortalTimeout(300);
  wm.setMinimumSignalQuality(10);
  wm.setAPCallback(wmApCallback);
  wm.setSaveParamsCallback(wmSaveParams);
  wm.setCustomHeadElement(WM_HEAD_HTML);
  wm.setCustomMenuHTML(WM_MENU_HTML);

  static char themeBuf[4];
  static char brightBuf[4];
  snprintf(themeBuf, sizeof(themeBuf), "%u", settings.theme);
  snprintf(brightBuf, sizeof(brightBuf), "%u", settings.brightness);

  pTz = new WiFiManagerParameter(WM_ID_TZ, WM_LABEL_TZ, settings.tz, sizeof(settings.tz) - 1);
  pTheme = new WiFiManagerParameter(WM_ID_THEME, WM_LABEL_THEME, themeBuf, 3);
  pBright = new WiFiManagerParameter(WM_ID_BRIGHT, WM_LABEL_BRIGHT, brightBuf, 3,
                                     WM_CUSTOM_BRIGHT);
  wm.addParameter(pTz);
  wm.addParameter(pTheme);
  wm.addParameter(pBright);
  wm.addParameter(new WiFiManagerParameter(WM_INFO_HTML));

  Serial.println("[wm] connecting...");
  if (!wm.autoConnect(AP_NAME)) {
    Serial.println("[wm] failed / timed out, continuing offline");
  } else {
    Serial.printf("[wm] connected: %s\n", WiFi.localIP().toString().c_str());
  }
}
