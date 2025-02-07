// NeoPixel test program showing use of the WHITE channel for RGBW
// pixels only (won't look correct on regular RGB NeoPixel strips).

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1:
#define LED_PIN 2

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 7

// NeoPixel brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 255  // Set BRIGHTNESS to about 1/5 (max = 255)

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)

int r1 = 255;
int r2 = 255;
int g1 = 40;
int g2 = 50;
int b1 = 2;
int b2 = 5;
int w1 = 10;
int w2 = 20;

int maxBrightness = 255;
int minBrightness = 50;
int lerpProgress[LED_COUNT] = {255, 200, 140, 255, 20, 255, 255};
int lerpDirection[LED_COUNT] = {0, 0, -1, 0, -1, 0, -1};
int lerpInterval[LED_COUNT] = {5, 5, 10, 5, 7, 5, 5};
int red[LED_COUNT] = {r1, r1, r1, r1, r1, r1, r1};
int green[LED_COUNT] = {g1, g1, g2, g1, g2, g1, g2};
int blue[LED_COUNT] = {b1, b1, b2, b1, b2, b1, b2};
int white[LED_COUNT] = {w1, w1, w2, w1, w2, w1, w2};
long lastUpdateTime[LED_COUNT];

int blinkInterval = 0;
int blinkPixel = -1;
int maxBlinkSteps = 255;
int minBlinkSteps = 100;
int blinkProgress = maxBlinkSteps;
int blinkDirection = -1;
long lastBlinkTime;

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);
  Serial.begin(9600);

  long timeNow = millis();
  for (int i = 0; i < LED_COUNT; i++) {
    lastUpdateTime[i] = timeNow;
  }
  lastBlinkTime = millis();
}

void loop() {
  if (blinkPixel == -1 && millis() - lastBlinkTime > blinkInterval) {
    blinkPixel = random(LED_COUNT);
    blinkDirection = -1;
    blinkProgress = maxBlinkSteps + blinkDirection;
  }
  for (int i = 0; i < LED_COUNT; i++) {
    if (i == blinkPixel){
      blinkLight(i);
    }
    else {
      lerpLight(i);
    }
  }
  strip.show();
}

void lerpLight(int pixel) {
  if (millis() - lastUpdateTime[pixel] < lerpInterval[pixel]) return;
  int r = red[pixel] * lerpProgress[pixel] / maxBrightness;
  int g = green[pixel] * lerpProgress[pixel] / maxBrightness;
  int b = blue[pixel] * lerpProgress[pixel] / maxBrightness;
  int w = white[pixel] * lerpProgress[pixel] / maxBrightness;
  strip.setPixelColor(pixel, r, g, b, w);
  lerpProgress[pixel] += lerpDirection[pixel];
  if (lerpProgress[pixel] <=  minBrightness) {
    lerpProgress[pixel] = minBrightness;
    lerpDirection[pixel] *= -1;
  } else if (lerpProgress[pixel] >= maxBrightness) {
    lerpProgress[pixel] = maxBrightness;
    lerpDirection[pixel] *= -1;
  }
  lastUpdateTime[pixel] = millis();
}

void blinkLight(int pixel) {
  int r = red[pixel] * blinkProgress / maxBlinkSteps;
  int g = green[pixel] * blinkProgress / maxBlinkSteps;
  int b = blue[pixel] * blinkProgress / maxBlinkSteps;
  int w = white[pixel] * blinkProgress / maxBlinkSteps;
  strip.setPixelColor(pixel, r, g, b, w);
  blinkProgress += blinkDirection;
  if (blinkProgress == minBlinkSteps) {
    blinkDirection *= -1;
  }
  if (blinkProgress == maxBlinkSteps){
    blinkPixel = -1;
    lastBlinkTime = millis();
    blinkInterval = getBlinkInterval();
  }
}

int getBlinkInterval() {
  return random(2000, 6000);
}
