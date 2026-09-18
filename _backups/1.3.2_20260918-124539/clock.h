#pragma once
#include <Adafruit_NeoPixel.h>
#include "config.h"
#include "themes.h"
#include "settings.h"
#include "themestore.h"

static Adafruit_NeoPixel strip(MAX_LEDS, LED_PIN, LED_TYPE);

static uint8_t curR[MAX_LEDS], curG[MAX_LEDS], curB[MAX_LEDS];
static uint8_t tgtR[MAX_LEDS], tgtG[MAX_LEDS], tgtB[MAX_LEDS];

// Runtime segment layout, derived from settings.segCount[].
static uint16_t segStarts[SEG_COUNT];
static uint8_t segCounts[SEG_COUNT];
static uint16_t totalLeds = 0;

static bool clockForceRefresh = false;   // ask the main loop to recompute targets

// value (0..12) -> bitmask over the 5 fixed Fibonacci blocks (1,1,2,3,5).
// bit0 = 1st one, bit1 = 2nd one, bit2 = 2, bit3 = 3, bit4 = 5.
static uint8_t valueToMask(uint8_t v) {
  switch (v) {
    case 0:  return 0b00000;
    case 1:  return 0b00001;   // 1
    case 2:  return 0b00100;   // 2
    case 3:  return 0b01000;   // 3
    case 4:  return 0b01001;   // 3+1
    case 5:  return 0b10000;   // 5
    case 6:  return 0b10001;   // 5+1
    case 7:  return 0b10100;   // 5+2
    case 8:  return 0b11000;   // 5+3
    case 9:  return 0b11001;   // 5+3+1
    case 10: return 0b11100;   // 5+3+2
    case 11: return 0b11101;   // 5+3+2+1
    case 12: return 0b11111;   // 5+3+2+1+1
  }
  return 0;
}

// Recompute segment starts/total from settings and reset the strip.
static void clockRebuild() {
  uint16_t start = 0;
  for (uint8_t i = 0; i < SEG_COUNT; i++) {
    segCounts[i] = settings.segCount[i];
    segStarts[i] = start;
    start += segCounts[i];
  }
  totalLeds = (start > MAX_LEDS) ? MAX_LEDS : start;

  strip.updateLength(totalLeds);

  for (uint16_t i = 0; i < MAX_LEDS; i++) {
    curR[i] = curG[i] = curB[i] = 0;
    tgtR[i] = tgtG[i] = tgtB[i] = 0;
  }
  strip.clear();
  strip.show();

  Serial.printf("[led] total=%u seg=%u,%u,%u,%u,%u\n", totalLeds,
                segCounts[0], segCounts[1], segCounts[2], segCounts[3], segCounts[4]);
}

static void clockBegin() {
  strip.begin();
  strip.setBrightness(settings.brightness);
  clockRebuild();
}

static void clockPush() {
  for (uint16_t i = 0; i < totalLeds; i++) {
    strip.setPixelColor(i, curR[i], curG[i], curB[i]);
  }
  strip.show();
}

// Compute the target colors for the given local time.
static void clockUpdateTargets(uint8_t hour24, uint8_t minute) {
  uint8_t h12 = hour24 % 12;
  if (h12 == 0) h12 = 12;
  uint8_t hourMask = valueToMask(h12);
  uint8_t minMask = valueToMask(minute / 5);

  const Theme& t = activeTheme();

  for (uint8_t b = 0; b < SEG_COUNT; b++) {
    bool hA = hourMask & (1 << b);
    bool mA = minMask & (1 << b);
    ThemeColor c;
    if (!settings.on)        c = {0, 0, 0};
    else if (hA && mA)       c = t.both;
    else if (hA)             c = t.hour;
    else if (mA)             c = t.minute;
    else                     c = t.off;

    for (uint8_t i = 0; i < segCounts[b]; i++) {
      uint16_t idx = segStarts[b] + i;
      if (idx >= totalLeds) break;
      tgtR[idx] = c.r;
      tgtG[idx] = c.g;
      tgtB[idx] = c.b;
    }
  }
}

static uint8_t stepToward(uint8_t c, uint8_t t) {
  if (c == t) return c;
  if (c < t) return (t - c <= FADE_STEP) ? t : c + FADE_STEP;
  return (c - t <= FADE_STEP) ? t : c - FADE_STEP;
}

// Advance the fade animation. Returns true if a redraw happened.
static bool clockTick() {
  bool changed = false;
  for (uint16_t i = 0; i < totalLeds; i++) {
    uint8_t r = stepToward(curR[i], tgtR[i]);
    uint8_t g = stepToward(curG[i], tgtG[i]);
    uint8_t b = stepToward(curB[i], tgtB[i]);
    if (r != curR[i] || g != curG[i] || b != curB[i]) {
      curR[i] = r; curG[i] = g; curB[i] = b;
      changed = true;
    }
  }
  if (changed) clockPush();
  return changed;
}

// Jump straight to the target (no fade).
static void clockSnap() {
  for (uint16_t i = 0; i < totalLeds; i++) {
    curR[i] = tgtR[i]; curG[i] = tgtG[i]; curB[i] = tgtB[i];
  }
  clockPush();
}

static void clockSetBrightness(uint8_t b) {
  strip.setBrightness(b);
  clockPush();
}

// Fill every LED (used at boot / config-portal entry).
static void clockFill(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < totalLeds; i++) {
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

// ---------------------------------------------------------------------------
// Segment test: light one whole segment at a time, in turn.
// ---------------------------------------------------------------------------
static const uint8_t TEST_COLORS[SEG_COUNT][3] = {
  {255, 0, 0},     // fib 1 - red
  {0, 255, 0},     // fib 2 - green
  {0, 0, 255},     // fib 3 - blue
  {255, 180, 0},   // fib 5 - amber
  {200, 0, 255},   // fib 8 - magenta
};

static bool testActive = false;
static int8_t testIndex = -1;
static unsigned long testNextAt = 0;

static void clockTestStart() {
  testActive = true;
  testIndex = -1;
  testNextAt = millis() + 400;   // brief all-off pause first
  for (uint16_t i = 0; i < MAX_LEDS; i++) curR[i] = curG[i] = curB[i] = 0;
  clockPush();
  Serial.println("[test] segment test start");
}

static void clockTestStop() {
  if (!testActive) return;
  testActive = false;
  clockForceRefresh = true;
  Serial.println("[test] segment test stop");
}

static bool clockTestActive() { return testActive; }

// Non-blocking step; call from loop().
static void clockTestTick() {
  if (!testActive) return;
  if ((long)(millis() - testNextAt) < 0) return;

  testIndex++;
  if (testIndex >= SEG_COUNT) {
    testActive = false;
    clockForceRefresh = true;
    Serial.println("[test] segment test done");
    return;
  }

  for (uint16_t i = 0; i < MAX_LEDS; i++) curR[i] = curG[i] = curB[i] = 0;
  uint8_t seg = (uint8_t)testIndex;
  uint8_t r = TEST_COLORS[seg][0], g = TEST_COLORS[seg][1], b = TEST_COLORS[seg][2];
  for (uint8_t i = 0; i < segCounts[seg]; i++) {
    uint16_t idx = segStarts[seg] + i;
    if (idx >= totalLeds) break;
    curR[idx] = r; curG[idx] = g; curB[idx] = b;
  }
  clockPush();
  Serial.printf("[test] seg %u (fib %u) LEDs %u..%u\n", seg, FIB_VALUES[seg],
                segStarts[seg], segStarts[seg] + segCounts[seg] - 1);
  testNextAt = millis() + 500;
}

// Blocking run, used once at boot before WiFi.
static void clockTestRunBlocking(unsigned long timeoutMs = 6000) {
  clockTestStart();
  unsigned long t0 = millis();
  while (clockTestActive() && (millis() - t0) < timeoutMs) {
    clockTestTick();
    delay(5);
  }
  clockTestStop();
}
