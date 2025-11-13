#include <FastLED.h>
#include <ESP32Servo.h>

#define LED_PIN     8
#define NUM_LEDS    2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define LINE_L 17
#define LINE_R 5
#define FRONT_L 15
#define FRONT_R 7
#define DIAG_L 16
#define DIAG_R 6

#define MR1 12
#define MR2 13
#define ML1 10
#define ML2 9

#define PWM_MR1 4
#define PWM_MR2 5
#define PWM_ML1 6
#define PWM_ML2 7

#define BT1 40
#define BT2 41

#define START 42
#define freq 1000

Servo FLAG;
#define serv 14

CRGB leds[2];
int strat = 0;
int ang;
int L = 170;
int R = 0;
int thresholdL = 895;  
int thresholdR = 530;
byte total;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  pinMode(LINE_L, INPUT);
  pinMode(LINE_R, INPUT);
  pinMode(FRONT_L , INPUT);
  pinMode(FRONT_R , INPUT);
  pinMode(DIAG_L , INPUT);
  pinMode(DIAG_R , INPUT);
  pinMode(BT1 , INPUT_PULLDOWN);
  pinMode(BT2 , INPUT_PULLDOWN);
  pinMode(START , INPUT);

  ledcSetup(PWM_MR1, freq, 8);
  ledcSetup(PWM_MR2, freq, 8);
  ledcSetup(PWM_ML1, freq, 8);
  ledcSetup(PWM_ML2, freq, 8);

  ledcAttachPin(MR1, PWM_MR1);
  ledcAttachPin(MR2, PWM_MR2);
  ledcAttachPin(ML1, PWM_ML1);
  ledcAttachPin(ML2, PWM_ML2);

  FLAG.setPeriodHertz(50);
  FLAG.attach(serv, 500, 2500);
  FLAG.write(90);

  Serial.begin(115200);
  Serial.println("Starting booting sequence...");
}

byte SensState() {
  bool sens1 = !digitalRead(FRONT_L);
  byte sens2 = !digitalRead(FRONT_R) * 2;
  byte sens3 = digitalRead(DIAG_L) * 4;
  byte sens4 = digitalRead(DIAG_R) * 8;
  byte total = sens1 + sens2 + sens3 + sens4;
  return total;
}

byte LineState() {

  bool line1 = digitalRead(LINE_L);
  byte line2 = digitalRead(LINE_R)*2;
  byte total = line1 + line2;
  return total;
}

void Foward(byte a, byte b, int t) {
  ledcWrite(PWM_MR1, a);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, b);
  ledcWrite(PWM_ML2, 0);
  delay(t);
}

void Backwards(byte a, byte b, int t) {
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, a);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, b);
  delay(t);
}

void Left(byte a, byte b, int t) {
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, a);
  ledcWrite(PWM_ML1, b);
  ledcWrite(PWM_ML2, 0);
  delay(t);
}

void Right (byte a, byte b, int t) {
  ledcWrite(PWM_MR1, a);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, b);
  delay(t);
}

void MotorOff() {
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, 0);
}

void Brake(int t) {
  ledcWrite(PWM_MR1, 255);
  ledcWrite(PWM_MR2, 255);
  ledcWrite(PWM_ML1, 255);
  ledcWrite(PWM_ML2, 255);
  delay(t);
}

void SensTest() {
  while (digitalRead(START) == HIGH) {
    switch (SensState()) {
      case 1:
        leds[1].setRGB(200, 0, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        break;

      case 2:
        leds[0].setRGB(200, 0, 0);
        FastLED.show();
        break;

      case 3:
        leds[0].setRGB(230, 24, 118);
        leds[1].setRGB(230, 24, 118);
        FastLED.show();
        break;

      case 4:
        leds[1].setRGB(180, 180, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        break;

      case 5:
        leds[1].setRGB(0, 180, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        break;

      case 8:
        leds[0].setRGB(180, 180, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        break;

      case 10:
        leds[0].setRGB(0, 180, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        break;

      default:
        leds[0].setRGB(0, 0, 0);
        leds[1].setRGB(0, 0, 0);
    }
  }
}

void LineTest() {
  while (digitalRead(START) == HIGH) {
    switch (LineState()) {
      case 0:
        leds[0].setRGB(0, 180, 0); //IZQ
        leds[1].setRGB(0, 180, 0); //DER
        FastLED.show();
        Serial.println("White White");
        break;

      case 1:
        leds[0].setRGB(0, 180, 0); //IZQ
        leds[1].setRGB(180, 0, 0); //DER
        FastLED.show();
        Serial.println("Black White");
        break;

      case 2:
        leds[0].setRGB(180, 0, 0); //IZQ
        leds[1].setRGB(0, 180, 0); //DER
        Serial.println("White Black");
        FastLED.show();
        break;

      case 3:
        leds[0].setRGB(180, 0, 0); //IZQ
        leds[1].setRGB(180, 0, 0); //DER
        FastLED.show();
        Serial.println("Black Black");
        break;
    }
  }
}

void Main_Battle() {
  while (digitalRead(START) == HIGH) {
    leds[1].setRGB(0, 0, 0);
    leds[0].setRGB(0, 0, 0);
    FastLED.show();
    FLAG.write(ang);

    switch (LineState()) {
      case 3:
        switch (SensState()) {
          case 1:
            Foward(80, 100, 20);
            break;
          case 2:
            Foward(100, 80, 20);
            break;
          case 3:
            Foward(255, 255, 100);
            break;
          case 4:
            Brake(50);
            Right(255, 180, 40);
            break;
          case 8:
            Brake(50);
            Left(180, 255, 40);
            break;
          default:
            Foward(25, 25, 30);
            break;
        }
        break;

      case 0:
        Brake(100);
        Backwards(200, 200, 175);
        Right(200, 200, 180);
        break;

      case 1:
        Brake(50);
        Backwards(200, 200, 100);
        Right(200, 200, 100);
        break;

      case 2:
        Brake(50);
        Backwards(200, 200, 100);
        Left(200, 200, 100);
        break;
    }
  }
  if (digitalRead(START) == LOW) {
    MotorOff();
  }
}

void CurveRight() {
  if (digitalRead(START) == HIGH) {
    Foward(255, 80, 400);
    Brake(30);
    ang = L;
    Main_Battle();
  }
}


void CurveLeft() {
  if (digitalRead(START) == HIGH) {
    Foward(70, 255, 420);
    Brake(30);
    ang = R;
    Main_Battle();
  }
}

void DodgeRight() {   //NEW
  while (digitalRead(START) == HIGH) {
    Left(255, 255, 50);
    Brake(30);
    Foward(255, 245, 220);
    Brake(30);
    Right(255, 255, 120);
    Brake(30);
    ang = L;
    Main_Battle();
  }
}

void DodgeLeft() {  //NEW
  while (digitalRead(START) == HIGH) {
    Right(255, 255, 50);
    Brake(30);
    Foward(255, 245, 220);
    Brake(30);
    Left(255, 255, 100);
    Brake(30);
    ang = R;
    Main_Battle();
  }
}

void SpinRight() {
  while (digitalRead(START) == HIGH) {
    Right(255, 255, 100);
    Brake(30);
    ang = L;
    Main_Battle();
  }
}

void SpinLeft() {
  if (digitalRead(START) == HIGH) {
    Left(255, 255, 100);
    Brake(30);
    ang = R;
    Main_Battle();
  }
}

void Rocket() {
  if (digitalRead(START) == HIGH) {
    Foward(255, 240, 300);
    Main_Battle();
    ang = L;
  }
}

void Behind() {
  if (digitalRead(START) == HIGH) {
    Left(255, 255, 150);
    Brake(30);
    ang = L;
    Main_Battle();
  }
}


void loop() {
  FLAG.write(90);
  if (digitalRead(BT1) == HIGH) {
    delay(350);
    strat ++;
    if (strat > 11) {
      strat = 0;
    }
  }

  if (digitalRead(BT2) == HIGH) {
    delay(350);
    strat --;
    if (strat < 0) {
      strat = 11;
    }
  }

  switch (strat) {
    case 0:
      leds[0].setRGB(0, 0, 0); 
      leds[1].setRGB(30, 0, 0); 
      FastLED.show();
      Serial.println("Case 0");
      ang = L;
      Main_Battle();
      break;

    case 1:
      leds[0].setRGB(30, 0, 0); 
      leds[1].setRGB(0, 0, 0); 
      FastLED.show();
      Serial.println("Case 1");
      ang = R;
      Main_Battle();
      break;

    case 2:
      leds[0].setRGB(0, 40, 0); 
      leds[1].setRGB(0, 0, 0); 
      FastLED.show();
      Serial.println("Case 2");
      CurveRight();
      break;

    case 3:
      leds[0].setRGB(0, 0, 0); //IZQ
      leds[1].setRGB(0, 40, 0); //DER
      FastLED.show();
      Serial.println("Case 3");
      CurveLeft();
      break;

    case 4:
      leds[0].setRGB(40, 60, 0); //IZQ
      leds[1].setRGB(40, 60, 0); //DER
      FastLED.show();
      Serial.println("Case 4");
      Rocket();
      break;

    case 5:
      leds[0].setRGB(90, 0, 40); //IZQ
      leds[1].setRGB(90, 0, 40); //DER
      FastLED.show();
      Serial.println("Case 5");
      DodgeRight();
      break;

    case 6:  
      leds[0].setRGB(90, 0, 20); //IZQ
      leds[1].setRGB(90, 0, 20); //DER
      FastLED.show();
      Serial.println("Case 6");
      DodgeLeft();
      break;

    case 7:
      leds[0].setRGB(0, 0, 60); //IZQ
      leds[1].setRGB(0, 0, 60); //DER
      FastLED.show();
      Serial.println("Case 7");
      Behind();
      break;

    case 8:
      leds[0].setRGB(30, 50, 0); //IZQ
      leds[1].setRGB(0, 0, 0); //DER
      FastLED.show();
      Serial.println("Case 8");
      SpinRight();
      break;

    case 9:
      leds[0].setRGB(0, 0, 0); //IZQ
      leds[1].setRGB(30, 50, 0); //DER
      FastLED.show();
      Serial.println("Case 9");
      SpinLeft();
      break;

    case 10:
      leds[0].setRGB(90, 0, 90); //IZQ
      leds[1].setRGB(90, 0, 90); //DER
      FastLED.show();
      Serial.println("Case 10");
      LineTest();
      break;

    case 11:
      leds[0].setRGB(90, 0, 40); //IZQ
      leds[1].setRGB(90, 0, 40); //DER
      FastLED.show();
      Serial.println("Case 11");
      SensTest();
      break;

  }
}
