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

// Own WiFi management: connect with saved credentials, otherwise start an
// access point (with a captive-portal DNS) so the clock's web UI is always
// reachable. No WiFiManager portal — our own server owns port 80.

static DNSServer dnsServer;
static bool apActive = false;
static unsigned long lastReconnectAt = 0;

#define WIFI_CONNECT_TIMEOUT_MS 15000UL
#define WIFI_RETRY_MS 30000UL

static void wifiStartAp() {
  if (apActive) return;
  if (WiFi.getMode() != WIFI_AP_STA) WiFi.mode(WIFI_AP);
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
  WiFi.mode(WIFI_STA);
  Serial.println("[wifi] AP stopped");
}

static bool wifiConnectBlocking(uint32_t timeoutMs) {
  if (settings.wifiSsid[0] == 0) return false;
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.wifiSsid, settings.wifiPass);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < timeoutMs) {
    delay(200);
    yield();
  }
  return WiFi.status() == WL_CONNECTED;
}

static void wifiBegin() {
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

// Non-blocking: saves credentials and starts connecting; wifiLoop() stops the
// AP once the STA link is up. Keeps the AP alive during the attempt.
static void wifiConnect(const char* ssid, const char* pass) {
  if (!ssid || !ssid[0]) return;
  strncpy(settings.wifiSsid, ssid, sizeof(settings.wifiSsid) - 1);
  settings.wifiSsid[sizeof(settings.wifiSsid) - 1] = 0;
  strncpy(settings.wifiPass, pass ? pass : "", sizeof(settings.wifiPass) - 1);
  settings.wifiPass[sizeof(settings.wifiPass) - 1] = 0;
  settingsSave();
  if (apActive) WiFi.mode(WIFI_AP_STA); else WiFi.mode(WIFI_STA);
  WiFi.begin(settings.wifiSsid, settings.wifiPass);
  lastReconnectAt = millis();
  Serial.printf("[wifi] connecting to '%s'...\n", settings.wifiSsid);
}

static void wifiForget() {
  settings.wifiSsid[0] = 0;
  settings.wifiPass[0] = 0;
  settingsSave();
  WiFi.disconnect(true);
}

static void wifiLoop() {
  if (apActive) dnsServer.processNextRequest();

  if (WiFi.status() == WL_CONNECTED) {
    if (apActive) wifiStopAp();
  } else if (!apActive && (millis() - lastReconnectAt) > WIFI_RETRY_MS) {
    lastReconnectAt = millis();
    wifiStartAp();
  }
}
