#pragma once
#include <Arduino.h>
#include <DNSServer.h>
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include "config.h"
#include "settings.h"
#include "clock.h"

// Own WiFi management. We deliberately never run AP+STA at the same time: the
// ESP8266 lwIP stack is unstable in mixed mode. On boot we try the saved
// network (pure STA); if that fails we fall back to a setup AP with a captive
// portal, so the clock's web UI is always reachable.

static DNSServer dnsServer;
static bool apActive = false;
static unsigned long lastReconnectAt = 0;

#define WIFI_CONNECT_TIMEOUT_MS 25000UL   // normally ~3s; generous for a weak signal
#define WIFI_RETRY_MS 60000UL             // start the setup AP if still not connected

static bool wifiOnline() {
  return WiFi.status() == WL_CONNECTED && WiFi.localIP() != IPAddress(0, 0, 0, 0);
}

static void wifiStartAp() {
  if (apActive) return;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_NAME);
  delay(100);
  dnsServer.start(53, "*", WiFi.softAPIP());
  apActive = true;
  Serial.printf("[wifi] AP '%s' at %s\n", AP_NAME, WiFi.softAPIP().toString().c_str());
  clockFill(0, 0, 90);   // blue while in setup AP mode
}

static void wifiStopAp() {
  if (!apActive) return;
  dnsServer.stop();
  WiFi.softAPdisconnect(true);
  apActive = false;
  Serial.println("[wifi] AP stopped");
}

static bool wifiConnectBlocking(uint32_t timeoutMs) {
  if (settings.wifiSsid[0] == 0) return false;
  WiFi.persistent(false);
#ifdef ESP8266
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.setOutputPower(20.5);
#endif
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(settings.wifiSsid, settings.wifiPass);
  WiFi.setAutoReconnect(true);
  unsigned long start = millis();
  while (!wifiOnline() && (millis() - start) < timeoutMs) {
    delay(200);
    yield();
  }
  Serial.printf("[wifi] connect took %lus\n", (millis() - start) / 1000);
  return wifiOnline();
}

static void wifiBegin() {
  WiFi.persistent(false);
  if (settings.wifiSsid[0]) {
    Serial.printf("[wifi] connecting to '%s'...\n", settings.wifiSsid);
    if (wifiConnectBlocking(WIFI_CONNECT_TIMEOUT_MS)) {
      Serial.printf("[wifi] connected: %s\n", WiFi.localIP().toString().c_str());
      return;
    }
    Serial.println("[wifi] connect failed");
  } else {
    Serial.println("[wifi] no saved network");
  }
  wifiStartAp();
}

static void wifiForget() {
  settings.wifiSsid[0] = 0;
  settings.wifiPass[0] = 0;
  settingsSave();
  WiFi.disconnect(true);
}

static void wifiLoop() {
  if (apActive) dnsServer.processNextRequest();

  if (wifiOnline()) {
    if (apActive) wifiStopAp();
    return;
  }
  if (!apActive && (millis() - lastReconnectAt) > WIFI_RETRY_MS) {
    lastReconnectAt = millis();
    wifiStartAp();
  }
}
