#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 //defines the width of the OLED screen
#define SCREEN_HEIGHT 64 //defines the height of the OLED screen
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); //sets up the OLED screen with the defined width and height

#define LINE A3

#define FRONT_L 12
#define FRONT_R A0
#define DIAG_L 8
#define DIAG_R 13

#define SENS1 9
#define SENS5 A1
#define SENS6 A2
#define SENS8 A6
#define SENS9 A7

// defines motor
#define MA1 11
#define MA2 6
#define MB2 3
#define MB1 5

#define SERVO 10

// defines remote and button input pin
#define REMOTE 4
#define BTN2 2
#define BTN1 7

byte rut = 0;
bool test_mode;

void setup() {
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3c)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  delay(2000);
  oled.clearDisplay();
  oled.setCursor(0, 0);

  pinMode(LINE, INPUT);
  pinMode(FRONT_L , INPUT);
  pinMode(FRONT_R , INPUT);
  pinMode(DIAG_L , INPUT);
  pinMode(DIAG_R , INPUT);

  pinMode(REMOTE, INPUT);
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);

  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  Serial.begin(9600);
}

void oledWrite(String current_mode) { //function to show on screen the current mode
  oled.clearDisplay();
  oled.setTextSize(3);
  oled.setCursor(0, 20);
  oled.setTextColor(WHITE);
  oled.println(current_mode);
  oled.display();
  //Serial.println(current_mode);
}

void brake(int t) {
  analogWrite(MA1, 255);
  analogWrite(MA2, 255);
  analogWrite(MB1, 255);
  analogWrite(MB2, 255);
  delay(t);
}

void off() {
  analogWrite(MA1, 0);
  analogWrite(MA2, 0);
  analogWrite(MB1, 0);
  analogWrite(MB2, 0);
}

void forwards(int a, int b, int t) {
  analogWrite(MA1, a);
  digitalWrite(MA2, LOW);
  analogWrite(MB1, b);
  digitalWrite(MB2, LOW);
  delay(t);
}

void backwards(int a, int b, int t) {
  digitalWrite(MA1, LOW);
  analogWrite(MA2, a);
  digitalWrite(MB1, LOW);
  analogWrite(MB2, b);
  delay(t);
}

void left(int a, int b, int t) {
  analogWrite(MA1, a);
  digitalWrite(MA2, LOW);
  digitalWrite(MB1, LOW);
  analogWrite(MB2, b);
  delay(t);
}

void right(int a, int b, int t) {
  digitalWrite(MA1, LOW);
  analogWrite(MA2, a);
  analogWrite(MB1, b);
  digitalWrite(MB2, LOW);
  delay(t);
}

byte SensState() {
  bool sens1 = !digitalRead(FRONT_R);
  byte sens2 = !digitalRead(FRONT_L) * 2;
  byte sens3 = digitalRead(DIAG_R) * 4;
  byte sens4 = digitalRead(DIAG_L) * 8;
  byte total = sens1 + sens2 + sens3 + sens4;
  return total;
}

void MainBattle() {
  while (digitalRead(REMOTE) == HIGH) {
    if (digitalRead(LINE) == 1) {
      switch (SensState()) {
        case 1:
          forwards(75, 100, 5);
          break;

        case 2:
          forwards(100, 75, 5);
          break;

        case 3:
          forwards(255, 255, 5);
          break;

        case 4:
          right(180, 180, 5);
          break;

        case 8:
          left(180, 180, 5);
          break;

        default:
          forwards(80, 80, 10);
          break;
      }
    }
    else {
      backwards(180, 170, 100);
      right(255, 255, 175);
    }
  }
  if (digitalRead(REMOTE) == LOW) {
    off();
  }
}



void Rocket() {
  while (digitalRead(REMOTE) == HIGH) {
    forwards(255, 240, 150);
    MainBattle();
  }
  off();
}

void CurveR() {
  while (digitalRead(REMOTE) == HIGH) {
    forwards(255, 60, 350);
    MainBattle();
  }
  off();
}

void CurveL() {
  while (digitalRead(REMOTE) == HIGH) {
    forwards(130, 255, 350);
    MainBattle();
  }
  off();
}

void DodgeL() {
  while (digitalRead(REMOTE) == HIGH) {
    left(255, 180, 70);
    brake(50);
    forwards(255, 240, 150);
    brake(50);     
    right(255, 240, 100);   
    MainBattle();
  }
  off();
}

void DodgeR() {
  while (digitalRead(REMOTE) == HIGH) {
    right(255, 240, 50);  
    brake(50);    
    forwards(255, 240, 180);
    brake(50);      
    left(255, 180, 70);
    brake(50);
    MainBattle();
  }
  off();
}

void Spin_L() {
  while (digitalRead(REMOTE) == HIGH) {
    left(255, 255, 80);     //255 255 50
    brake(50);
    MainBattle();
  }
  off();
}
void Spin_R() {
  while (digitalRead(REMOTE) == HIGH) {
    right(255, 255, 100);
    brake(50);
    MainBattle();
  }
  off();
}

void Behind(){
  while (digitalRead(REMOTE) == HIGH) {
    right(255, 255, 170);
    brake(50);
    MainBattle();
  }
  off();
}

void motortest() {
  while (digitalRead(REMOTE) == HIGH) {
    for (int i = 0; i <= 255; i++) {
      forwards(i, i, 10);
    }
    for (int i = 0; i <= 255; i++) {
      backwards(i, i, 10);
    }
    for (int i = 0; i <= 255; i++) {
      right(i, i, 10);
    }
    for (int i = 0; i <= 255; i++) {
      left(i, i, 10);
    }
  }

}

void sensor_test() {
  while (digitalRead(REMOTE) == HIGH) {
    if (digitalRead(BTN1) == HIGH) {
      test_mode = true;
    }
    if (digitalRead(BTN2) == HIGH) {
      test_mode = false;
    }
    switch (test_mode) {
      case 0:
        oledWrite(String(SensState()));
        break;

      case 1:
        oledWrite(String(digitalRead(LINE)));
        break;
    }
  }
}

void loop() {
  off();
  if (digitalRead(BTN1) == HIGH) { //increase routine cout if button 1 is pressed
    delay(150); //increase delay for less sensitivity (longer press), decrease for more sensitivity (shorter press)
    rut++;
    Serial.println("Button 1 pressed");
    if (rut >= 12) {
      rut = 0;
    }
  }
  if (digitalRead(BTN2) == HIGH) { //decrease routine cout if button 2 is pressed
    delay(150);
    rut--;
    Serial.println("Button 2 pressed");
    if (rut <= 0) {
      rut = 12;
    }
  }
  switch (rut) {
    //BASIC
    case 0:
      oledWrite("Default");
      MainBattle();
      break;

    case 1:
      oledWrite("Rocket");
      Rocket();   //READY
      break;

    case 2:
      oledWrite("Dodge R");
      DodgeR();
      break;
      
    case 3:
      oledWrite("Dodge L");
      DodgeL();
      break;

    case 4:
      oledWrite("Curve R");
      CurveR();     //READY
      break;

    case 5:
      oledWrite("Curve L");
      CurveL();   //READY
      break;
    //NEW
    case 6:
      oledWrite("FK     Dodge L");
      //FkDodgeL();
      break;

    case 7:
      oledWrite("FK     Dodge R");
      //  FkDodgeR();
      break;

    //POSITIONS
    case 8:
      oledWrite("Spin R");  //READY
      Spin_R();
      break;

    case 9:
      oledWrite("Spin L");  //READY
      Spin_L();
      break;

    case 10:
      oledWrite("Behind");
      Behind();
      break;

    //SETTINGS
    case 11:
      oledWrite("Motor     Test");
      motortest();
      break;

    case 12:
      oledWrite("Sensor  Test");
      sensor_test();
      break;

  }
}
