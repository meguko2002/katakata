const int anode_pins[] = {3, 5, A3, A5, 11, 4, A2, A4}; // アノードに接続するArduinoのピン{A,B,C,D,E,F,G,DP}
const int cathode_pins[] = {6, 12, 13, A1}; // カソードに接続するArduinoのピン{DIG4,3,2,1}9->12,10->13
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

const int buttonPin = 8;     // the number of the pushbutton pin
const int DigitNum = 2;

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
  pinMode(buttonPin, INPUT_PULLUP);
}
void loop() {
  static unsigned long    startMillis = 0, timecounter = 0,raptime=0;
  static int state = 0;

  if (Serial.available()) {
    state = Serial.read() - '0';
    switch (state) {
      case 0 : startMillis = millis();  //start
        break;
      case 1 :  raptime=timecounter;    //stop
        break;  
      case 2 : timecounter = 0;         //reset
        break;
    }
  }
  if (state == 0)    timecounter = (millis() - startMillis) / pow(10, DigitNum - 1);
  display_numbers(timecounter);
}
