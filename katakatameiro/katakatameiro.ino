/*******************************
   KATAKATA MEIRO !!
 ******************************/

#include <LiquidCrystal_I2C.h>   //LCD
#include <Wire.h>                   //LCD
#include <Servo.h>                 //Servo motor
#include <SNESpaduino.h>          //Super Famicon controller

#define LATCH 2                     //orange line
#define CLOCK 4                     //yellow line
#define DAT 3                      //red line
#define START 10                    //to Raz 
#define SELECT 13                   //to Raz 
#define FIN 8                       //to Raz 
#define SERVO_L 5
#define SERVO_R 6
#define READY 7
#define PIEZO 9                    //beep
#define SENSOR 11                     //goal sensor photo diode

int MOTOR_DELAY = 12;       //must over 8[msec] (= MaxVelocity * MotorSpeed )
/******                         MaxVelocity = 3deg/Loop (up+left)
*******                         MotorSpeed = 160msec/60deg               */

int CTR = 90;                         //center degree
uint16_t btns = 0b11111111111;        //button input
uint16_t pre_btns = 0b11111111111;    //button input(連続押し判定防止のため２度読み)
bool Sound = HIGH;                    //If HIGH ,sound on ,else off

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
Servo servo_l;
Servo servo_r;
SNESpaduino pad(LATCH, CLOCK, DAT);  //controller

/*----------------------------setup 初期設定------------------------------*/

void setup()
{
  pinMode(READY,  OUTPUT);
  pinMode(START,  OUTPUT);
  pinMode(SELECT,  OUTPUT);
  pinMode(FIN,  OUTPUT);
  pinMode(SERVO_L,  OUTPUT);
  pinMode(SERVO_R,  OUTPUT);
  pinMode(PIEZO,  OUTPUT);
  pinMode(SENSOR,  INPUT);
  servo_l.attach(SERVO_L);
  servo_r.attach(SERVO_R);
  lcd.begin(16, 2);
  lcd.clear();
  lcd.backlight();
}

/*------------------------------main status ここから開始------------------------------*/
void loop() {

  /* ---液晶表示---*/

  unsigned long previousMillis = 0;
  const long interval = 500;
  bool flip = LOW;
  int i = 0;

  while (1) {
    unsigned long currentMillis = millis();              //currentMillisに現在の時間を記憶
    digitalWrite(READY, HIGH);
    if (currentMillis - previousMillis > interval) {     //"press START"と"KataKta Meiro !!"をinterval間隔で交互表示
      if (i < 10) {
        lcd.setCursor(0, 0);
        if (flip == LOW)      lcd.print("press START     ");
        else   lcd.print("                ");
        flip =  !flip;
      }
      else  if (i < 20) {
        lcd.setCursor(0, 0);
        lcd.print("KataKta Meiro !!");

      }
      previousMillis = currentMillis;
      i++;
      if (i == 30)i = 0;
    }

    /*---ボタン読み取り---*/

    pre_btns = pad.getButtons(false);                         // 連押し判定防止のため50msecあけて再判定
    delay(50);                                                //
    btns = pad.getButtons(false);                             //
    /*--------------------*/
    if (!(btns & BTN_START) & (!(pre_btns) == 0)) {          // STARTでゲーム開始
      game_mode();
      break;
    }
    if (!(btns & BTN_SELECT) & (!(pre_btns) == 0)) {         // SELECTでリセット
      status_reset();
      break;
    }
    if (!(btns & (BTN_L | BTN_R)) & (!(pre_btns) == 0)) {    // L+Rでテストモード
      test_mode();
      break;
    }
  }
}

/*------------------------------game mode------------------------------*/
void game_mode() {
  int pos_l = CTR , pos_r = CTR ;
  unsigned long    startMillis = 0, timecounter = 0;
  bool sensor = HIGH, pres;

  status_reset();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ready");
  for (int i = 3; i > 0; i--) {
    lcd.setCursor(0, 1);
    lcd.print(i);
    digitalWrite(READY, HIGH);
    beep(1000, 500);
    delay(500);
    digitalWrite(READY, LOW);
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Go !!");
  beep(2000, 1000);
  digitalWrite(START, HIGH);
  startMillis = millis();
  while (1) {
    btns = pad.getButtons(false);

    if (!(btns & BTN_UP)) {
      pos_l += 2;
      pos_r -= 2;
    }
    if (!(btns & BTN_DOWN)) {
      pos_l -= 2;
      pos_r += 2;
    }
    if (!(btns & BTN_LEFT)) {
      pos_l -= 2;
      pos_r -= 2;
    }
    if (!(btns & BTN_RIGHT)) {
      pos_l += 2;
      pos_r += 2;
    }
    if (!(btns & (BTN_UP | BTN_LEFT)))  pos_r -= 3 ;   // = MaxVelocity
    if (!(btns & (BTN_UP | BTN_RIGHT)))  pos_l += 3 ;
    if (!(btns & (BTN_DOWN | BTN_LEFT))) pos_l -= 3 ;
    if (!(btns & (BTN_DOWN | BTN_RIGHT))) pos_r += 3 ;

    if (pos_l < 30)pos_l = 30;
    if (pos_l > 150)pos_l = 150;
    if (pos_r < 30)pos_r = 30;
    if (pos_r > 150)pos_r = 150;

    servo_l.write(pos_l);                            //move motors
    servo_r.write(pos_r);
    timecounter = millis() - startMillis;

    sensor = digitalRead(SENSOR);
    if ((sensor == LOW) & (pres == HIGH)) {
      unsigned long finishMillis = timecounter;    //finishMillisにラップタイムを代入
      lcd.setCursor(0, 1);
      lcd.print(add_point(finishMillis));
      digitalWrite(FIN,  HIGH);
      if (finishMillis <20000) {                  //miracle finish　なくてもよい
        lcd.setCursor(0, 0);
        lcd.print("Finish !!       ");
        for (int i = 0; i < 3; i++) {
          delay(500);
          lcd.noBacklight();
          delay(500);
          lcd.backlight();
        }
        song();
        deadsong();
      }
      else  {                             //normal finish
        beep(2000, 100);
        lcd.setCursor(0, 0);
        for (int i = 0; i < 6 ; i++) {
          lcd.setCursor(0, 0);
          if (i % 2 == 0) lcd.print("Finish !!       ");
          else  lcd.print("                ");
          delay(500);
        }
        return;
      }


      return;
    }
    else {                                          //not yet finish
      lcd.setCursor(0, 1);
      lcd.print(add_point(timecounter));
      if (!(btns & BTN_SELECT)) {
        lcd.setCursor(0, 0);
        lcd.print("Suspended");
        for (int i = 0; i < 3; i++) {
          beep(1000, 100);
          delay(200);
        }
        status_reset();
        delay(2000);
        return;
      }
      pres = sensor;
    }
    delay(MOTOR_DELAY);
  }
}

/*------------------------------status reset------------------------------*/
void status_reset() {
  servo_l.write(CTR);
  servo_r.write(CTR);
  digitalWrite(SELECT, HIGH);
  digitalWrite(READY, HIGH);
  digitalWrite(START, LOW);
  digitalWrite(FIN, LOW);
  delay(100);
  digitalWrite(SELECT, LOW);
}

/*------------------------------test mode-なくてもよい-----------------------------*/
void test_mode() {
  char message[][16] = {
    "1.Beep on/off",
    "2.START",
    "3.LED_ARRAY1",
    "4.LED_ARRAY2",
    "5.FIN",
    "6.SELECT",
    "7.Sensor adjust",
    "8.Mario Theme",
  };
  digitalWrite(READY, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Test mode");
  lcd.setCursor(0, 1);
  lcd.print("[L+R] to return");
  delay(1000);
  lcd.clear();

  int i = 0 , cursor = 0, label;
  int m_num = sizeof(message) / sizeof(message[16]);

  while (1) {
    if (i < 0)i += m_num;
    if (i >= m_num ) i -= m_num;
    lcd.setCursor(0, cursor);
    lcd.print(">");
    lcd.setCursor(0, 1 - cursor);
    lcd.print(" ");
    lcd.setCursor(1, 0);
    lcd.print(message[i]);
    lcd.setCursor(1, 1);
    lcd.print(message[(i + 1) % m_num]);
    label = i + cursor + 1;
    if (label > m_num) label = label % m_num;
    pre_btns = pad.getButtons(false);
    delay(50);
    btns = pad.getButtons(false);

    if (!(btns & BTN_UP) & (!(pre_btns) == 0) ) {
      if (cursor == 1) cursor = 0;
      else {
        lcd.clear();
        i--;
      }
    }
    else if (!(btns & BTN_DOWN) & (!(pre_btns) == 0)) {
      if (cursor == 0)  cursor = 1;
      else {
        lcd.clear();
        i++;
      }
    }
    else if (!(btns &  BTN_A) & (!(pre_btns) == 0)) {
      switch (label) {
        case 1:
          sound_switch();
          break;
        case 2:
          sound_test(START);
          break;
        case 3:

          break;
        case 4:

          break;
        case 5:
          sound_test(FIN);
          break;
        case 6:
          sound_test(SELECT);
          break;
        case 7:
          digitalWrite(READY, HIGH);
          break;
        case 8:
          song();
          deadsong();
          break;
      }
    }
    else if (!(btns & (BTN_L | BTN_R)) & (!(pre_btns) == 0)) {
      lcd.clear();
      delay(100);
      return;
    }
    pre_btns == pad.getButtons(false);
    delay(100);
  }
}

/*-------------------------------------------------------------*/
void beep(int frequency, int duration) {
  if (Sound == HIGH) tone(PIEZO, frequency, duration);
  else {}
}

void  sound_switch() {
  int cursor = 0;
  lcd.clear();
  delay(200);
  while (1) {
    lcd.setCursor(0, cursor);
    lcd.print(">");
    lcd.setCursor(0, 1 - cursor);
    lcd.print(" ");
    lcd.setCursor(1, 0);
    lcd.print("Sound on");
    lcd.setCursor(1, 1);
    lcd.print("Sound off");
    while (1) {
      if (!btns == 0)  break;
    }
    pre_btns = pad.getButtons(false);
    delay(50);
    btns = pad.getButtons(false);
    if (!(btns & BTN_UP) & (!(pre_btns) == 0)) {
      cursor = 0;
    }
    else if (!(btns & BTN_DOWN) & (!(pre_btns) == 0)) {
      cursor = 1;
    }
    else if (!(btns & BTN_A) & (!(pre_btns) == 0)) {
      if (cursor == 0)  Sound = HIGH;
      else  Sound = LOW;
      lcd.setCursor(0, 1 - cursor);
      lcd.print("                ");
      beep(2000, 500);
      delay(500);
      return;
    }
  }
}

String add_point(unsigned long n) {     // millisec  to  "sec + "." + millisec"
  String tmp;
  tmp = n / 1000;
  tmp += ".";
  tmp += n % 1000;
  return tmp;
}

void sound_test(int sw) {
  digitalWrite(sw, HIGH);
  delay(50);
  digitalWrite(sw, LOW);
  delay(50);
  return;
}


/*------------------------------OMAKE Mario Theme-なくてもよい-----------------------------*/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

//Mario main theme song
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};
//Mario main them tempo
int tempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};
int deadmelody[] = {
  NOTE_B6, NOTE_F7, 0, NOTE_F7,
  NOTE_F7, NOTE_E7, NOTE_D7, NOTE_C7,
};

int deadtempo[] = {
  12, 12, 12, 12,
  9, 9, 9, 12
};

void song() {
  //Serial.println(" 'Mario Theme'");
  int size = sizeof(melody) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {
    pre_btns = pad.getButtons(false);

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];
    buzz(PIEZO, melody[thisNote], noteDuration);
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    buzz(PIEZO, 0, noteDuration);
    btns = pad.getButtons(false);
    if (!(btns & BTN_A) & (!(pre_btns) == 0)) {
      return;
    }
  }
}

void deadsong() {
  //  Serial.println(" 'Mario Dead'");
  int size = sizeof(deadmelody) / sizeof(int);
  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000 / deadtempo[thisNote];
    buzz(PIEZO, deadmelody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    buzz(PIEZO, 0, noteDuration);
  }
}

void buzz(int targetPin, long frequency, long length) {
  digitalWrite(READY, HIGH);
  long delayValue = 1000000 / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(targetPin, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
  digitalWrite(READY, LOW);
}
