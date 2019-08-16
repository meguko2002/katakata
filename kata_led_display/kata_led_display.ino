#include "Ws2822s.h"
#define NUM_PIXELS 38 // 使用するWS2822Sの数
#define LED_PIN 13 // WS2822SのDAIピンにつなげるArduinoのピン番号
#define MAX_VAL 6  // 0 to 255 for brightness
Ws2822s LED(LED_PIN, NUM_PIXELS);

void setup() {
  Serial.begin(9600);
}

void loop() {
  statuscheck();
}

void statuscheck() {

  while (1) {
    if (Serial.available()) {
      char state = Serial.read();
      Serial.print(state);
      switch (state) {
        case 'R' :      //ready start
          three_count();
          break;
        case 'S':     //start
          game_start();
          break;
        case 'G':        //goal
          goal_display();
          statuscheck();
          ready_display();
          break;
        case 'H':        //high score goal
          for (int i = 0; i < 10; i++) roulette(10);
          statuscheck();
          ready_display();
          break;
        case 'W' :     //waiting
          ready_display();
          break;
      }
    }
    else return;
  }
}


/*-------------------------------readyLED---------------------------------*/
void  three_count() {
  alllight(0, 0, 0);

  alllight(0, MAX_VAL, 0);
  delay(500);
  alllight(0, 0, 0);
  delay(500);
  alllight(0, MAX_VAL, 0);
  delay(500);
  alllight(0, 0, 0);
  delay(500);
  alllight(0, MAX_VAL, 0);
  delay(500);
  alllight(0, 0, 0);
  delay(500);
  alllight(255, 0, 0);
  delay(500);
  alllight(255, 0, 0);
  delay(500);
  statuscheck();

}
void game_start() {
  //  alllight(255, 0, 0);
  //  delay(1000);
  while (1) {
    downlight(500, MAX_VAL / 3, MAX_VAL / 3, MAX_VAL / 3); //白
    downlight(500, 0, 0, MAX_VAL); //青
    downlight(500, MAX_VAL, MAX_VAL, 0); //黄
    downlight(500, 0, MAX_VAL, 0); //緑
    downlight(500, MAX_VAL , 0, 0); //赤
  }
}
void alllight(int r, int g, int b) {
  for (int i = 0; i < NUM_PIXELS ; i++) LED.setColor(i, r, g, b);
  LED.send();
}
void downlight(int interval, int r, int g, int b) {
  for (int j = 0; j < NUM_PIXELS / 2; j++) {
    for (int i = 0; i < NUM_PIXELS ; i++) {
      if (i < j || i >= (NUM_PIXELS - j))LED.setColor(i, r, g, b);
      else LED.setColor(i, 0, 0, 0);
    }
    LED.send();
    delay(interval);
    statuscheck();
  }
}
/*-------------------------------finish LED---------------------------------*/
void goal_display() {
  //  alllight(0, 0, 0);
  for (int j = 0; j < 3; j++) {
    alllight(MAX_VAL, 0, 0);
    delay(500);
    alllight(0, MAX_VAL, 0);
    delay(500);
  }

}

/*-------------------------------Wait LED---------------------------------*/
void ready_display() {
  for (int i = 0; i < 10; i++) {
    alllight(0, 0, 0);
    delay(100);
    statuscheck();
  }
  rainbow(100);
  theaterChase(127, 127, 127, 50);
  rainbow(100);
  theaterChase(127, 0, 0, 50);
  rainbow(100);
  theaterChase(0, 127, 0, 50);
  rainbow(100);
  theaterChase(0, 0, 127, 50);
}
void rainbow(uint8_t duration) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < NUM_PIXELS; i++) {
      byte WheelPos = 255 - (i + j) & 255;
      if (WheelPos < 85) {
        LED.setColor(i , (255 - WheelPos * 3) / 3, 0, WheelPos );
      }
      else if (WheelPos < 170) {
        WheelPos -= 85;
        LED.setColor(i , 0, WheelPos , (255 - WheelPos * 3) / 3 );
      }
      else {
        WheelPos -= 170;
        LED.setColor(i , WheelPos , (255 - WheelPos * 3) / 3 , 0);
      }
    }
    LED.send();
    delay(duration);
    statuscheck();
  }
}
void theaterChase(uint8_t r, uint8_t g, uint8_t b, uint8_t duration) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < NUM_PIXELS; i = i + 3) {
        LED.setColor(i + q, r, g, b); //turn every third pixel on
      }
      LED.send();
      delay(duration);
      statuscheck();

      for (uint16_t i = 0; i < NUM_PIXELS; i = i + 3) {
        LED.setColor(i + q, r, g, b);    //turn every third pixel off
      }
    }
  }
}
void roulette(int duration) {
  for (int i = 0; i < NUM_PIXELS; ++i) {
    LED.setColor(i, MAX_VAL, 0x00, 0x00);
    LED.setColor((i + 1) % NUM_PIXELS, MAX_VAL, MAX_VAL, 0x00);
    LED.setColor((i + 2) % NUM_PIXELS, 0x00, MAX_VAL, 0x00);
    LED.setColor((i + 3) % NUM_PIXELS, 0x00, MAX_VAL, MAX_VAL);
    LED.setColor((i + 4) % NUM_PIXELS, 0x00, 0x00, MAX_VAL);
    LED.setColor((i + 5) % NUM_PIXELS, MAX_VAL, 0x00, MAX_VAL);
    LED.setColor((i + 6) % NUM_PIXELS, MAX_VAL, 0x00, 0x00);
    LED.setColor((i + 7) % NUM_PIXELS, MAX_VAL, 0x00, 0x00);
    LED.setColor((i + 8) % NUM_PIXELS, MAX_VAL, 0x00, 0x00);
    LED.setColor((i + 9) % NUM_PIXELS, MAX_VAL, 0x00, 0x00);
    LED.send();
    delay(duration);
  }
}
