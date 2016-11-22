#include <Adafruit_NeoPixel.h>
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, 6, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(){

  unsigned long timecounter;

  led_game(timecounter); // ゲーム中に実行　 関数game_mode()から呼び出し
  rainbow(); // 待機時に実行
  rainbowCycle(); //ゴール時に実行

}

/*-------------------------------Game Mode LED---------------------------------*/
void led_game(unsigned long timecnt){
  if(timecnt < 30000){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(MAX_VAL, 0, MAX_VAL)); // Purple
      strip.show();
      delay(1000);
    }
  }
  else if(timecnt >= 30000 && timecnt < 45000){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, MAX_VAL)); // Blue
      strip.show();
      delay(700);
    }
  }
  else if(timecnt >= 45000 && timecnt < 60000){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, MAX_VAL, MAX_VAL)); // Cyan
      strip.show();
      delay(500);
    }
  }
  else if(timecnt >= 60000 && timecnt < 90000){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, MAX_VAL, 0)); // Green
      strip.show();
      delay(300);
    }
  }
  else if(timecnt >= 90000 && timecnt < 120000){
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, MAX_VAL, 0)); // Yellow
      strip.show();
      delay(200);
    }
  }
  else{
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(MAX_VAL, 0, 0)); // Red
      strip.show();
      delay(100);
    }
  }
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); // Black 全消灯
    strip.show();
  }
}

/*-------------------------------Wait LED---------------------------------*/
void rainbow() {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(DELAY_TIME1);
  }
}

/*-------------------------------Goal LED---------------------------------*/
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle() {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(DELAY_TIME2);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color((WheelPos * 3)*MAX_VAL/255, (255 - WheelPos * 3)*MAX_VAL/255, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((255 - WheelPos * 3)*MAX_VAL/255, 0, (WheelPos * 3)*MAX_VAL/255);
  } else {
   WheelPos -= 170;
   return strip.Color(0, (WheelPos * 3)*MAX_VAL/255, (255 - WheelPos * 3)*MAX_VAL/255);
  }
}
