/*
**IMPORTANT: This program compiles, but HAVEN'T been tested on real hardware. It is subject to change according to test results.**
Project: Hackpad_numeric_keypad
Creator: YuanRetro (William Li)
Last Modified: 2026-MAR-21
Repo: https://github.com/yuanretro/Hackpad_Numeric_Keypad
Description: This firmware registers the keypad in the OS and sends out key press like a regular numpad.
NumLock key switches the keypad between sending out number keys and function keys.
One of the LED indicate the NumLock state, and lights up according to key presses.
The LED strip displays a breathing effect when no activity is present on the keyapd.
*/

#include <Arduino.h>
#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

// Keypad array GPIOs
const uint8_t ROW_PINS[5] = {26, 27, 28, 29, 6};
const uint8_t COL_PINS[4] = {7, 0, 1, 2};

// LED strip GPIOs
#define LED_PIN 4
#define LED_COUNT 4
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

// HID key codes
#define KP_NONE 0x00
#define KP_NUMLOCK 0x53
#define KP_SLASH 0x54
#define KP_ASTERISK 0x55
#define KP_MINUS 0x56
#define KP_PLUS 0x57
#define KP_ENTER 0x58
#define KP_1 0x59
#define KP_2 0x5A
#define KP_3 0x5B
#define KP_4 0x5C
#define KP_5 0x5D
#define KP_6 0x5E
#define KP_7 0x5F
#define KP_8 0x60
#define KP_9 0x61
#define KP_0 0x62
#define KP_DOT 0x63

// Keypad mapping for keys not affected by NumLock state
const uint8_t KEY_FIXED[5][4] = {
    {KP_NUMLOCK, KP_SLASH, KP_ASTERISK, KP_MINUS},
    {KP_NONE, KP_NONE, KP_NONE, KP_PLUS},
    {KP_NONE, KP_NONE, KP_NONE, KP_NONE},
    {KP_NONE, KP_NONE, KP_NONE, KP_ENTER},
    {KP_NONE, KP_NONE, KP_NONE, KP_NONE},
};

// Keypad mapping when NumLock is active
const uint8_t KEY_NUMON[5][4] = {
    {KP_NONE, KP_NONE, KP_NONE, KP_NONE},
    {KP_7, KP_8, KP_9, KP_NONE},
    {KP_4, KP_5, KP_6, KP_NONE},
    {KP_1, KP_2, KP_3, KP_NONE},
    {KP_0, KP_NONE, KP_DOT, KP_NONE},
};

// Keypad mapping when NumLock is inactive
const uint8_t KEY_NUMOFF[5][4] = {
    {KP_NONE, KP_NONE, KP_NONE, KP_NONE},
    {KEY_HOME, KEY_UP_ARROW, KEY_PAGE_UP, KP_NONE},
    {KEY_LEFT_ARROW, KP_NONE, KEY_RIGHT_ARROW, KP_NONE},
    {KEY_END, KEY_DOWN_ARROW, KEY_PAGE_DOWN, KP_NONE},
    {KEY_INSERT, KP_NONE, KEY_DELETE, KP_NONE},
};

// Key states
bool keyState[5][4] = {};
uint32_t lastChange[5][4] = {};
const uint32_t DEBOUNCE_MS = 12;

// NumLock state
bool numLock = true;
uint8_t colPressed[4] = {};

uint32_t COL_COLOR[4];

// Initial colors for the LED strip
void initColors()
{
  COL_COLOR[0] = strip.Color(180, 0, 255, 0);
  COL_COLOR[1] = strip.Color(255, 0, 80, 0);
  COL_COLOR[2] = strip.Color(60, 200, 255, 0);
  COL_COLOR[3] = strip.Color(140, 255, 0, 0);
}

// Update LED strip according to key press
void updateLEDs()
{
  for (int c = 0; c < 4; c++)
  {
    if (colPressed[c] > 0)
    {
      strip.setPixelColor(c, COL_COLOR[c]);
    }
    else if (c == 0 && numLock)
    {
      strip.setPixelColor(c, strip.Color(0, 0, 0, 50));
    }
    else
    {
      strip.setPixelColor(c, strip.Color(0, 0, 5, 8));
    }
  }
  strip.show();
}

// Breathe effect when no activity is present
void breathe()
{
  static uint32_t lastT = 0;
  static float phase = 0;
  if (millis() - lastT < 16)
    return;
  lastT = millis();
  phase += 0.02f;
  uint8_t w = (uint8_t)(25 + 25 * sinf(phase));
  strip.setPixelColor(0, strip.Color(0, 0, 0, numLock ? 50 : w));
  for (int i = 1; i < LED_COUNT; i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, w / 4, w));
  }
  strip.show();
}

// Obtain respective key code when a key is pressed
uint8_t getKeycode(int r, int c)
{
  if (KEY_FIXED[r][c] != KP_NONE)
  {
    return KEY_FIXED[r][c];
  }
  else
  {
    return numLock ? KEY_NUMON[r][c] : KEY_NUMOFF[r][c];
  }
}

// Initial setup method, executed each time the microcontroller boots
void setup()
{
  // Set up keyboard scanning
  for (int r = 0; r < 5; r++)
  {
    pinMode(ROW_PINS[r], OUTPUT);
    digitalWrite(ROW_PINS[r], HIGH);
  }
  for (int c = 0; c < 4; c++)
  {
    pinMode(COL_PINS[c], INPUT_PULLUP);
  }

  Keyboard.begin();

  // Set up LED strip
  strip.begin();
  strip.setBrightness(90);
  strip.clear();
  strip.show();

  initColors();
  updateLEDs();
}

// Executes repeatedly as long as the keypad is plugged in
void loop()
{
  bool anyPressed = false;
  bool ledDirty = false;

  // Scan for key presses
  for (int r = 0; r < 5; r++)
  {
    digitalWrite(ROW_PINS[r], LOW);
    delayMicroseconds(10);

    for (int c = 0; c < 4; c++)
    {
      // Ignore unrecognized keys
      uint8_t kc = getKeycode(r, c);
      if (kc == KP_NONE)
        continue;

      bool reading = !digitalRead(COL_PINS[c]);

      // Debouncing
      if (reading != keyState[r][c])
      {
        if (millis() - lastChange[r][c] >= DEBOUNCE_MS)
        {
          lastChange[r][c] = millis();
          keyState[r][c] = reading;

          if (reading)
          {
            if (kc == KP_NUMLOCK)
              numLock = !numLock;
            Keyboard.press(kc);
            colPressed[c]++;
          }
          else
          {
            Keyboard.release(kc);
            if (colPressed[c] > 0)
              colPressed[c]--;
          }
          ledDirty = true;
        }
      }
      if (keyState[r][c])
        anyPressed = true;
    }

    digitalWrite(ROW_PINS[r], HIGH);
  }

  // Start breathing once no activity is detected
  if (ledDirty)
  {
    updateLEDs();
  }
  else if (!anyPressed)
  {
    breathe();
  }

  delay(1);
}