#include <FastLED.h>
#define LED_PIN     4
#define NUM_LEDS    2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define LINE 20

#define DIAG_L 10
#define DIAG_R 5
#define FRONT 6

#define MR1 2
#define MR2 3
#define ML1 0
#define ML2 1

#define PWM_MR1 0
#define PWM_MR2 1
#define PWM_ML1 2
#define PWM_ML2 3

#define STRAT_SW 21
#define START 8
#define freq 1000

CRGB leds[2];
byte strat = 0;

void setup() {
  //defining output of the LEDs and type of LED
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //defining inputs for sensors and buttons
  pinMode(LINE, INPUT);
  pinMode(FRONT , INPUT);
  pinMode(DIAG_L , INPUT);
  pinMode(DIAG_R , INPUT);
  pinMode(STRAT_SW , INPUT);
  pinMode(START , INPUT);
  //defining PWM channels for the motors
  ledcSetup(PWM_MR1, freq, 8);
  ledcSetup(PWM_MR2, freq, 8);
  ledcSetup(PWM_ML1, freq, 8);
  ledcSetup(PWM_ML2, freq, 8);
  //attaching PWM channel to the respective motors
  ledcAttachPin(MR1, PWM_MR1);
  ledcAttachPin(MR2, PWM_MR2);
  ledcAttachPin(ML1, PWM_ML1);
  ledcAttachPin(ML2, PWM_ML2);
  Serial.begin(115200);
  Serial.println("Starting booting sequence...");
  delay(500);
}

byte SensState() {
  //sends a numeric value from 0 to 7 depending on the number of sensors detecting
  bool sens1 = digitalRead(FRONT);
  byte sens2 = digitalRead(DIAG_L) * 2;
  byte sens3 = digitalRead(DIAG_R) * 4;

  byte total = sens1 + sens2 + sens3;
  return total;
}

byte LineState() {
  //sends a numeric value from 0 to 1.
  bool line1 = digitalRead(LINE);
  byte total = line1;
  return total;
}

//functions for the control of the motors, a and b can be values from 0 to 255 to adjust the speed of the motors and t is the time
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
        leds[1].setRGB(0, 30, 0);
        leds[0].setRGB(0, 30, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        Serial.println("CASO 1");
        break;

      case 2:
        leds[0].setRGB(100, 100, 0);
        FastLED.show();
        Serial.println("CASO 2");
        break;

      case 3:
        leds[0].setRGB(70, 180, 0);
        FastLED.show();
        Serial.println("CASO 3");
        break;

      case 4:
        leds[1].setRGB(100, 100, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        Serial.println("CASO 4");
        break;

      case 5:
        leds[1].setRGB(70, 180, 0);
        FastLED.show();
        delay(10);
        FastLED.clear();
        Serial.println("CASO 5");
        break;

      case 7:
        leds[0].setRGB(0, 0, 100);
         leds[1].setRGB(0, 0, 100);       
        FastLED.show();
        delay(10);
        FastLED.clear();
        Serial.println("CASO 7");
        break;

      default:
    leds[1].setRGB(0, 0, 0); //IZQ
    leds[0].setRGB(0, 0, 0); //DER
        Serial.println("DEFAULT");        
    }
  }
}

void LineTest() {
  while (digitalRead(START) == HIGH) {
    switch (LineState()) {
      case 0:
        leds[1].setRGB(0, 180, 0); //IZQ
        leds[0].setRGB(0, 180, 0); //DER
        FastLED.show();
        Serial.println("White White");
        break;

      case 1:
        leds[1].setRGB(0, 180, 0); //IZQ
        leds[0].setRGB(180, 0, 0); //DER
        FastLED.show();
        Serial.println("Black White");
        break;

      case 2:
        leds[1].setRGB(180, 0, 0); //IZQ
        leds[0].setRGB(0, 180, 0); //DER
        Serial.println("White Black");
        FastLED.show();
        break;

      case 3:
        leds[1].setRGB(180, 0, 0); //IZQ
        leds[0].setRGB(180, 0, 0); //DER
        FastLED.show();
        Serial.println("Black Black");
        break;
    }
  }
}

byte lastSensor = 2; // Inicializamos el sentido de giro a la derecha (2 corresponde al sensor derecho)

void Main_Battle() {
  while (digitalRead(START) == HIGH) {
    leds[1].setRGB(0, 0, 0); //IZQ
    leds[0].setRGB(0, 0, 0); //DER
    FastLED.show();
    
    switch (LineState()) {
      case 1:
        switch (SensState()) {
          case 1:
            Foward(255, 100, 20);
            lastSensor = 1; // Si el sensor izquierdo detecta, guardamos que fue el sensor izquierdo
            break;

          case 2:
            Right(160, 160, 20);
            lastSensor = 2; // Si el sensor derecho detecta, guardamos que fue el sensor derecho
            break;
            
          case 3:
            Foward(255, 70, 50);
            lastSensor = 1; // Si el sensor izquierdo detecta, guardamos que fue el sensor izquierdo
            break;

          case 4:
            Left(160, 160, 20);//OK
            lastSensor = 1; // Si el sensor izquierdo detecta, guardamos que fue el sensor izquierdo
            break;

          case 5:
            Foward(100, 255, 50);
            lastSensor = 2; // Si el sensor derecho detecta, guardamos que fue el sensor derecho
            break;
          case 7:
            Foward(255, 100, 20);
            lastSensor = 2; // Si el sensor derecho detecta, guardamos que fue el sensor derecho
            break;

          default:
              if (lastSensor == 1) {
                Left(150, 150, 25);  // Girar a la izquierda
              } else if (lastSensor == 2) {
                Right(150, 150, 25);  // Girar a la derecha
              }
            break;
        }
        break;

      case 0:
        Brake(100);
        Backwards(200, 90, 200);
        Right(255, 255, 260);
        break;
    }
  }
  if (digitalRead(START) == LOW) {
    MotorOff();
  }
}





void CurveRight() {
  while (digitalRead(START) == HIGH) {
    Foward(255, 50, 500);
    Brake(30);
    Main_Battle();
  }
}

void CurveLeft() {
  if (digitalRead(START) == HIGH) {
    Foward(255, 180, 500);
    Brake(30);
    Main_Battle();
  }
}

void DodgeRight() {   //NEW
  while (digitalRead(START) == HIGH) {
    Left(255, 110, 160);
    Brake(30);
    Foward(255, 100, 500);
    Brake(30);
    Right(255, 110, 160);
    Brake(30);
    Main_Battle();
  }
}

void DodgeLeft() {  //NEW
  while (digitalRead(START) == HIGH) {
    Right(255, 110, 160);
    Brake(30);
    Foward(255, 110, 500);
    Brake(30);
    Left(255, 110, 160);
    Brake(30);
    Main_Battle();
  }
}


void SpinRight() {
  while (digitalRead(START) == HIGH) {
    Left(200, 200, 100);
    Brake(30);
    Main_Battle();
  }
}

void SpinLeft() {
  if (digitalRead(START) == HIGH) {
    Right(200, 200, 100);
    Brake(30);
    Main_Battle();
  }
}

void Rocket() {
  if (digitalRead(START) == HIGH) {
    Foward(255, 100, 300);
    Main_Battle();
  }
}

void Behind() {
  if (digitalRead(START) == HIGH) {
    Left(200, 200, 180);
    Brake(30);
    Main_Battle();
  }
}


void loop() {
  if (digitalRead(STRAT_SW) == HIGH) { //increases value of strat each time the STRAT button is pressed
    delay(250);  //increase delay for more "sensitive" button, decrease for less sensitivity
    strat ++;
    if (strat > 10) {
      strat = 0;
    }
  }
  switch (strat) {
    case 0:
      //strat 1
      leds[1].setRGB(30, 0, 0); //IZQ
      leds[0].setRGB(30, 0, 0); //DER
      FastLED.show();
      Serial.println("Case 0");
      Main_Battle();
      break;

    case 1:
      //strat 2
      leds[1].setRGB(0, 40, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER
      FastLED.show();
      Serial.println("Case 1");
      CurveRight();
      break;

    case 2:
      //strat 3
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(0, 40, 0); //DER
      FastLED.show();
      Serial.println("Case 2");
      CurveLeft();
      break;

    case 3:
      //strat 4
      leds[1].setRGB(40, 60, 0); //IZQ
      leds[0].setRGB(40, 60, 0); //DER
      FastLED.show();
      Serial.println("Case 3");
      Rocket();
      break;

    case 4:   //NEW
      //strat 8
      leds[1].setRGB(90, 0, 40); //IZQ
      leds[0].setRGB(90, 0, 40); //DER
      FastLED.show();
      Serial.println("Case 4");
      DodgeRight();
      break;

    case 5:   //NEW
      //strat 8
      leds[1].setRGB(90, 0, 20); //IZQ
      leds[0].setRGB(90, 0, 20); //DER
      FastLED.show();
      Serial.println("Case 5");
      DodgeLeft();
      break;

    case 6:
      //strat 4
      leds[1].setRGB(0, 0, 60); //IZQ
      leds[0].setRGB(0, 0, 60); //DER
      FastLED.show();
      Serial.println("Case 6");
      Behind();
      break;

    case 7:
      //strat 5
      leds[1].setRGB(30, 50, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER
      FastLED.show();
      Serial.println("Case 7");
      SpinRight();
      break;

    case 8:
      //strat 6
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(30, 50, 0); //DER
      FastLED.show();
      Serial.println("Case 8");
      SpinLeft();
      break;

    case 9:
      //strat 7
      leds[1].setRGB(90, 0, 90); //IZQ
      leds[0].setRGB(90, 0, 90); //DER
      FastLED.show();
      Serial.println("Case 9");
      LineTest();
      break;

    case 10:
      //strat 8
      leds[1].setRGB(90, 0, 40); //IZQ
      leds[0].setRGB(90, 0, 40); //DER
      FastLED.show();
      Serial.println("Case 10");
      SensTest();
      break;

  }
}
