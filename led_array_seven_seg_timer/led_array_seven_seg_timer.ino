#define PIN_A 3
#define PIN_B 5
#define PIN_C A3
#define PIN_D A5
#define PIN_E 11
#define PIN_F 4
#define PIN_G A2
#define PIN_DP A4
#define PIN_DIG4 6
#define PIN_DIG3 12
#define PIN_DIG2 13
#define PIN_DIG1 A1
#define LED_PIN 10

/*****7seg definition *****/
const int anode_pins[] = {PIN_A, PIN_B, PIN_C, PIN_D, PIN_E, PIN_F, PIN_G, PIN_DP}; // アノードに接続するArduinoのピン
const int cathode_pins[] = { PIN_DIG4, PIN_DIG3, PIN_DIG2, PIN_DIG1}; // カソードに接続するArduinoのピン
const int number_of_anode_pins = sizeof(anode_pins) / sizeof(anode_pins[0]);
const int number_of_cathode_pins = sizeof(cathode_pins) / sizeof(cathode_pins[0]);
const int digits[] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00100111, // 7
  0b01111111, // 8
  0b01101111, // 9
};
const int DigitNum = 2;
unsigned long    startMillis = 0, timecounter = 0, raptime = 0;
/******************************************/

int state = 0;

/***** LED ARRAY definition *****/
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
Adafruit_NeoPixel strip = Adafruit_NeoPixel(20, LED_PIN, NEO_GRB + NEO_KHZ800);
uint16_t picnum = 0;
uint16_t colval = 0;
unsigned long previousMillis = 0;
int count = 0;
/********************************************/

void display_number (int n, int point) {
  int j;
  if (point == 1)   j = digits[n] | 0b10000000;
  else j = digits[n];
  for (int i = 0; i < number_of_anode_pins; i++)  digitalWrite(anode_pins[i], j & (1 << i) ? HIGH : LOW);
  digitalWrite(anode_pins[number_of_anode_pins], HIGH);
}

void clear_segments() {
  for (int j = 0; j < number_of_anode_pins; j++) digitalWrite(anode_pins[j], LOW);
}

void display_numbers (int n) {
  bool m;
  for (int i = 0; i < number_of_cathode_pins; i++) {
    if (i == number_of_cathode_pins - DigitNum) m = 1;
    else m = 0;
    digitalWrite(cathode_pins[i], LOW);
    display_number(n % 10, m); // 最後の一桁を表示する
    delay(1);
    clear_segments();
    digitalWrite(cathode_pins[i], HIGH);
    n = n / 10; // 10で割る
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < number_of_anode_pins; i++)   pinMode(anode_pins[i], OUTPUT);
  for (int i = 0; i < number_of_cathode_pins; i++) {
    pinMode(cathode_pins[i], OUTPUT);
    digitalWrite(cathode_pins[i], HIGH);
  }
  strip.begin();
  for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
  strip.show(); // Initialize all pixels to 'off'

}
void loop() {
  if (state == 0) {                 //ready
    void   rainbowCycle();
  }
  else if (state == 1) {           //play
    led_game();
    timecounter = (millis() - startMillis) / pow(10, DigitNum - 1);
  }
  else if (state == 2) {           //goal
    void   rainbow();
  }
  display_numbers(timecounter);
}

void serialEvent() {
  state = Serial.read() - '0';
  switch (state) {
    case 0 : timecounter = 0;         //reset
      break;
    case 1 : startMillis = millis();  //start
      break;
    case 2 :  raptime = timecounter;  //stop
      break;
  }
  picnum = 0;
  colval = 0;
  for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
}
/*-------------------------------Game Mode LED---------------------------------*/
void led_game() {
  unsigned long currentMillis = millis();
  count = picnum / strip.numPixels();
  int j = picnum % strip.numPixels();
  for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
  if (count == 0) {
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      picnum++;
    }
    strip.setPixelColor(j, strip.Color(MAX_VAL, 0, MAX_VAL));
    strip.setPixelColor(j - 1, strip.Color(MAX_VAL, 0, MAX_VAL));
  }
  else if (count == 1) {
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      picnum++;
    }
    strip.setPixelColor(j, strip.Color(0, 0, MAX_VAL));
    strip.setPixelColor(j - 1, strip.Color(0, 0, MAX_VAL));
  }
  else if (count == 2) {
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      picnum++;
    }
    strip.setPixelColor(j, strip.Color(0, MAX_VAL, MAX_VAL));
    strip.setPixelColor(j - 1, strip.Color(0, MAX_VAL, MAX_VAL));
  }
  else if (count == 3) {
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      picnum++;
    }
    strip.setPixelColor(j, strip.Color(0, MAX_VAL, 0));
    strip.setPixelColor(j - 1, strip.Color(0, MAX_VAL, 0));
  }
  else if (count == 4) {
    if (currentMillis - previousMillis >= 200) {
      previousMillis = currentMillis;
      picnum++;
    }
    for (int k = 0; k < 3; k++) {
      strip.setPixelColor(j - k, strip.Color(MAX_VAL, MAX_VAL, 0));
    }
  }
  else {
    if (currentMillis - previousMillis >= 50) {
      previousMillis = currentMillis;
      picnum++;
    }
    for (int k = 0; k < 4; k++) {
      strip.setPixelColor((j - k) % 20, strip.Color(MAX_VAL, 0, 0));
    }

  }
  strip.show();
}
/*-------------------------------Wait LED---------------------------------*/
void rainbow() {
  while (1) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= DELAY_TIME1) {
      previousMillis = currentMillis;
      if (count == 1) {
        colval++;
        colval = colval % 255;
        for (int i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, Wheel(colval));
      }
      else
        for (int i = 0; i < strip.numPixels(); i++)  strip.setPixelColor(i, strip.Color(0, 0, 0));
      count = 1 - count;
    }
    strip.show();
  }
}

/*-------------------------------Wait LED---------------------------------*/
void rainbowCycle() {
  unsigned long currentMillis = millis();
  int j = picnum % strip.numPixels();
  if (currentMillis - previousMillis >= DELAY_TIME2) {
    previousMillis = currentMillis;
    colval++;
  }
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + colval) & 255));
  }
  strip.show();
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

