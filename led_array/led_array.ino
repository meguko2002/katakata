#include <Adafruit_NeoPixel.h>
#define LED_PIN 6
#define START 10
#define SELECT 13
#define FIN 8
#define MAX_VAL 8  // 0 to 255 for brightness
#define DELAY_TIME1 100 // 待機時遅延時間
#define DELAY_TIME2 25 // ゴール時遅延時間
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(SELECT,  INPUT);
  pinMode(START,  INPUT);
  pinMode(FIN,  INPUT);
}

void loop() {
  rainbow();
}

/*-------------------------------Game Mode LED---------------------------------*/
void led_game() {
  for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 100; i++) {
      strip.setPixelColor(j, strip.Color(MAX_VAL, 0, MAX_VAL));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
  }
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 70 ; i++) {
      strip.setPixelColor(j, strip.Color(0, 0, MAX_VAL));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
  }
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 50 ; i++) {
      strip.setPixelColor(j, strip.Color(0, MAX_VAL, MAX_VAL));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
  }
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 30 ; i++) {
      strip.setPixelColor(j, strip.Color(0, MAX_VAL, 0));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
  }
  for (int j = 0; j < 20; j++) {
    for (int i = 0; i < 20 ; i++) {
      strip.setPixelColor(j, strip.Color(MAX_VAL, MAX_VAL, 0));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
  }
  for (int j = 0; j < 2000; j++) {
    for (int i = 0; i < 10 ; i++) {
      strip.setPixelColor(j%20, strip.Color(MAX_VAL, 0, 0));
      strip.show();
      if (digitalRead(FIN) == HIGH) rainbowCycle();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      delay(10);
    }
    for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

/*-------------------------------Wait LED---------------------------------*/
void rainbow() {

  uint16_t i, j;
  for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show();
  while (1) {
    for (j = 0; j < 256; j++) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel((i + j) & 255));
      }
      strip.show();
      if (digitalRead(START) == HIGH) led_game();
      delay(DELAY_TIME1);
    }
  }
}

/*-------------------------------Goal LED---------------------------------*/
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t i, j;
  while (1) {
    for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      }
      strip.show();
      if (digitalRead(SELECT) == HIGH)  rainbow();
      if (digitalRead(START) == HIGH)  led_game();
      delay(DELAY_TIME2);
    }
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color((WheelPos * 3) * MAX_VAL / 255, (255 - WheelPos * 3) * MAX_VAL / 255, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3) * MAX_VAL / 255, 0, (WheelPos * 3) * MAX_VAL / 255);
  } else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3) * MAX_VAL / 255, (255 - WheelPos * 3) * MAX_VAL / 255);
  }
}

