#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Platform / hardware
// ---------------------------------------------------------------------------
#ifdef ESP32
  #define LED_PIN 4          // "D4" on ESP32 boards is typically GPIO4
#else
  #define LED_PIN 2          // Wemos D1 mini D4 = GPIO2
#endif

#define MAX_LEDS 100             // strip buffer allocation (runtime total <= this)
#define LED_TYPE (NEO_GRB + NEO_KHZ800)   // WLED / WS2812B default. Use NEO_RGB if wrong.

#define AP_NAME "FibonacciClock"
#define HOSTNAME "fibonacci-clock"
#define FW_VERSION "1.3.1"

#define NTP_SERVER1 "pool.ntp.org"
#define NTP_SERVER2 "time.nist.gov"
#define NTP_RESYNC_MS (60UL * 60UL * 1000UL)   // re-sync every hour
#define TIME_VALID_EPOCH 1600000000UL          // anything above this is a real time

#define DEFAULT_TZ "AEST-10AEDT,M10.1.0,M4.1.0/3"
#define DEFAULT_THEME 0
#define DEFAULT_BRIGHTNESS 64
#define DEFAULT_BOOT_TEST true

#define FADE_STEP 8            // per-channel fade increment

// ---------------------------------------------------------------------------
// Fibonacci clock layout
//   The five block VALUES are fixed (1,1,2,3,5) and define the time maths:
//     hours   = sum of the red + blue blocks
//     minutes = (sum of the green + blue blocks) x 5
//   The LED COUNT of each block is a physical property of the build and is
//   configurable at runtime (always >= 1, total <= MAX_LEDS).
// ---------------------------------------------------------------------------
#define SEG_COUNT 5

static const uint8_t FIB_VALUES[SEG_COUNT] = {1, 1, 2, 3, 5};
static const uint8_t DEFAULT_SEG_COUNTS[SEG_COUNT] = {1, 1, 3, 7, 12};
