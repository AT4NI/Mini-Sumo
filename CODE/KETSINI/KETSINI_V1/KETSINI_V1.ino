#include <FastLED.h>
#define LED_PIN     20
#define NUM_LEDS    2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define LINE_L 4
#define LINE_R 21

#define FRONT_L 2
#define FRONT_R 1
#define DIAG_L 3 
#define DIAG_R 0

#define MR1 7
#define MR2 10
#define ML1 5
#define ML2 6

#define PWM_MR1 0
#define PWM_MR2 1
#define PWM_ML1 2
#define PWM_ML2 3

#define STRAT_SW 8
#define START 9
#define freq 1000

CRGB leds[2]; 
byte strat = 0;

void setup() {                
  //defining output of the LEDs and type of LED
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //defining inputs for sensors and buttons
  pinMode(LINE_L, INPUT);
  pinMode(LINE_R, INPUT);
  pinMode(FRONT_L ,INPUT);
  pinMode(FRONT_R ,INPUT);
  pinMode(DIAG_L ,INPUT);
  pinMode(DIAG_R ,INPUT);
  pinMode(STRAT_SW ,INPUT);
  pinMode(START ,INPUT);
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

byte SensState(){
  //sends a numeric value from 0 to 15 depending on the number of sensors detecting
  bool sens1 = !digitalRead(FRONT_L);
  byte sens2 = !digitalRead(FRONT_R)*2;
  byte sens3 = digitalRead(DIAG_L)*4;
  byte sens4 = digitalRead(DIAG_R)*8;
  byte total = sens1 + sens2 + sens3 + sens4;
  return total;
}

byte LineState(){
  //sends a numeric value from 0 to 3 depending on the number of sensors detecting
  bool line1 = digitalRead(LINE_L);
  byte line2 = digitalRead(LINE_R)*2;
  byte total = line1 + line2;
  return total;
}

//functions for the control of the motors, a and b can be values from 0 to 255 to adjust the speed of the motors and t is the time
void Foward(byte a, byte b, int t){
  ledcWrite(PWM_MR1, a);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, b);
  ledcWrite(PWM_ML2, 0);
  delay(t);
}

void Backwards(byte a, byte b, int t){
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, a);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, b);
  delay(t);
}

void Left(byte a, byte b, int t){
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, a);
  ledcWrite(PWM_ML1, b);
  ledcWrite(PWM_ML2, 0);
  delay(t);
}

void Right (byte a, byte b, int t){
  ledcWrite(PWM_MR1, a);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, b);
  delay(t);
}

void MotorOff(){
  ledcWrite(PWM_MR1, 0);
  ledcWrite(PWM_MR2, 0);
  ledcWrite(PWM_ML1, 0);
  ledcWrite(PWM_ML2, 0);
}

void Brake(int t){
  ledcWrite(PWM_MR1, 255);
  ledcWrite(PWM_MR2, 255);
  ledcWrite(PWM_ML1, 255);
  ledcWrite(PWM_ML2, 255);
  delay(t);
}
 
void SensTest(){
  while (digitalRead(START)==HIGH){
   switch (SensState()){
    case 1:
      leds[0].setRGB(200, 0, 0);
      FastLED.show();
      delay(10);
      FastLED.clear();
      break;

    case 2:
      leds[1].setRGB(200, 0, 0);
      FastLED.show();
      break;
    
    case 4:
      leds[0].setRGB(180, 180, 0);
      FastLED.show();
      delay(10);
      FastLED.clear();
      break;

    case 5:
      leds[0].setRGB(0, 180, 0);
      FastLED.show();
      delay(10);
      FastLED.clear();
      break;

    case 8:
      leds[1].setRGB(180, 180, 0);
      FastLED.show();
      delay(10);
      FastLED.clear();
      break;
    
    case 10:
      leds[1].setRGB(0, 180, 0);
      FastLED.show();
      delay(10);
      FastLED.clear();
      break;

    default:
    FastLED.clear();
    } 
  }
}

void LineTest(){
  while (digitalRead(START)==HIGH){
    switch (LineState()){
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

void Main_Battle(){
  while (digitalRead(START)==HIGH){
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER
    FastLED.show();
    switch(LineState()){
      case 3:
          switch (SensState()){
          case 1:
            Foward(75,100,20); 
            break;
          case 2:
            Foward(100,75,20);
            break;
          case 3:
            Foward(255, 255, 100);
            break;
          case 4:
            Brake(50);
            Right(255, 175, 40);
            break;
          case 8:
            Brake(50);
            Left(175, 255, 40);
            break;
          default:
            Foward(60,70,30);
            break;
          }
          break;

      case 0:
        Brake(100);
        Backwards(200,200,175);
        Right(200,200,180);
        break; 

      case 1:
        // Right line sensor detecting
        Brake(50);
        Backwards(200,200,100);
        Right(200,200,100);
        break;
        
      case 2:
        // Left line sensor detecting
        Brake(50);
        Backwards(200,200,100);
        Left(200,200,100);
        break;
    }
  }
  if(digitalRead(START)==LOW){
    MotorOff();
  }
}

void CurveLeft(){
  while (digitalRead(START)==HIGH){
    Foward(255, 80, 600);
          Brake(30);
    Main_Battle();
  }
}

void CurveRight(){
  if (digitalRead(START)==HIGH){
    Foward(80, 255, 600);
          Brake(30);
    Main_Battle();
  }
}


void CurveLeft_T(){
  while (digitalRead(START)==HIGH){
    Foward(255, 80, 700);
      Brake(30);
    Right(255, 255, 200);
      Brake(30);
    Main_Battle();
  }
}

void CurveRight_T(){
  if (digitalRead(START)==HIGH){
    Foward(80, 255, 600);
      Brake(30);    
    Left(255, 255, 200);
      Brake(30);      
    Main_Battle();
  }
}

void Rocket(){
  if (digitalRead(START)==HIGH){
    Foward(255, 245, 300);
    Main_Battle();
  }
}

void loop(){
  if(digitalRead(STRAT_SW)==LOW){  //increases value of strat each time the STRAT button is pressed
    delay(250);  //increase delay for more "sensitive" button, decrease for less sensitivity
    strat ++;
    if (strat > 7){
      strat = 0;
    }
  }
  switch (strat){
  case 0:
    //strat 1
      leds[1].setRGB(60, 0, 0); //IZQ
      leds[0].setRGB(60, 0, 0); //DER
    FastLED.show();
    Serial.println("Case 0");
    Main_Battle();
    break;
  
  case 1:
    //strat 2
      leds[1].setRGB(0, 80, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER
    FastLED.show();
    Serial.println("Case 1");
    CurveLeft();
    break;

  case 2:
    //strat 3
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(0, 80, 0); //DER
    FastLED.show();
    Serial.println("Case 2");
    CurveRight();
    break; 
  
  case 3:
    //strat 4
      leds[1].setRGB(70, 90, 0); //IZQ
      leds[0].setRGB(70, 90, 0); //DER
    FastLED.show();
    Serial.println("Case 3");
    Rocket();
    break;


  case 4:
    //strat 4
      leds[1].setRGB(50, 70, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER
    FastLED.show();
    Serial.println("Case 4");
    CurveLeft_T();
    break;
    
  case 5:
    //strat 5
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(50, 70, 0); //DER
    FastLED.show();
    Serial.println("Case 5");
    CurveRight_T();
    break;    

  case 6:
    //strat 6
      leds[1].setRGB(180, 0, 180); //IZQ
      leds[0].setRGB(180, 0, 180); //DER
    FastLED.show();
    Serial.println("Case 6");
    LineTest();
    break;

  case 7:
    //strat 7
      leds[1].setRGB(180, 0, 80); //IZQ
      leds[0].setRGB(180, 0, 80); //DER
    FastLED.show();
    Serial.println("Case 7");
    SensTest();
    break;
  }
}
