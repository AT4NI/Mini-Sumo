#include <FastLED.h>
#define LED_PIN     4
#define NUM_LEDS    2
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

#define LINE_L 7

#define FRONT_L 20
#define FRONT_R 5
#define DIAG_L 6
#define DIAG_R 10
//2-3-0-1
#define MR1 3
#define MR2 2

#define ML1 0
#define ML2 1

#define PWM_MR1 0
#define PWM_MR2 1
#define PWM_ML1 2
#define PWM_ML2 3

#define STRAT_SW 21
#define START 8
#define freq 1000

int espera = 0;      // Para controlar el tiempo de espera
int espera2 = 0;     // Control de espera adicional
int espera3 = 0;     // Control de espera para cambios
int centro = 0;      // Contador para el centro
int atk = 120;       // Valor inicial para la velocidad de ataque
bool DER = false;
bool IZQ = false;

CRGB leds[2];
byte strat = 0;

void setup() {
  //defining output of the LEDs and type of LED
  pinMode(LED_PIN, OUTPUT);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //defining inputs for sensors and buttons
  pinMode(LINE_L, INPUT);
  pinMode(FRONT_L , INPUT);
  pinMode(FRONT_R , INPUT);
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
  //sends a numeric value from 0 to 15 depending on the number of sensors detecting
  bool sens1 = digitalRead(FRONT_L);
  byte sens2 = digitalRead(FRONT_R) * 2;
  byte total = sens1 + sens2;
  return total;
}

byte LineState() {
  bool line1 = digitalRead(LINE_L);
  byte total = line1;
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


void SensTest() {
  while (digitalRead(START) == HIGH) {
    switch (SensState()) {
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
        
      case 3:
        leds[1].setRGB(0, 200, 0);
         leds[0].setRGB(0, 200, 0);       
        FastLED.show();
        break;


    default:
    FastLED.clear();
    }
  }
}

void LineTest() {
  while (digitalRead(START) == HIGH) {

    leds[1].setRGB(0, 0, 0); //IZQ
    leds[0].setRGB(0, 0, 0); //DER
    FastLED.show();
    switch (LineState()) {
      case 0:
        leds[1].setRGB(0, 180, 0); //IZQ
        leds[0].setRGB(0, 180, 0); //DER
        FastLED.show();
        Serial.println("White");
        break;

      case 1:
        leds[1].setRGB(180, 0, 0); //IZQ
        leds[0].setRGB(180, 0, 0); //DER
        FastLED.show();
        Serial.println("Black");
        break;

    }
  }
}


void Main_Battle() {


  while (digitalRead(START) == HIGH) {
      leds[1].setRGB(0, 0, 0); //IZQ
      leds[0].setRGB(0, 0, 0); //DER

    byte frontL = digitalRead(FRONT_L);
    byte frontR = digitalRead(FRONT_R) * 2;
    byte line = !digitalRead(LINE_L) * 4;

    byte sensor_state = frontL + frontR + line; // Binariza el estado de los sensores
    
    switch (sensor_state) {
      case B00000001: // Sensor derecho
        DER = true;
        IZQ = false;
        Right(150, 150, 7);  // Llamada a la función para girar a la derecha
        Brake(20);           // Detener por un tiempo breve
        break;
      case B00000010: // Sensor izquierdo
        DER = false;
        IZQ = true;
        Left(150, 150, 7);   // Llamada a la función para girar a la izquierda
        Brake(20);           // Detener por un tiempo breve
        break;
      case B00000011: // Ambos sensores frontales
        DER = true;
        IZQ = false;
        if (espera2 == 0) {
          MotorOff();      // Detener motores
          espera++;
          if (espera > 500) {
            Foward(130, 100, 50); // Avanzar
            espera = 0;
            espera3++;
          }
          if (espera3 > 3) {
            espera2 = 1; // Cambiar estado de espera
          }
        } else {
          Foward(atk, atk + 2, 1); // Avanzar con incremento de velocidad
          centro++;
          delay(1);
          if (centro > 2) {
            atk++;
            centro = 0;
          }
          if (atk > 252) {
            atk = 253; // Limitar el valor máximo de atk
          }
        }
        break;
        
      case B00000000: // Línea
        DER = true;
        IZQ = false;
        Brake(100);                       // Frenar
        Backwards(200, 200, 175);         // Retroceder
        Right(200, 200, 180);             // Girar a la derecha
        break;
        
      case B00000100: // Ningún sensor detectado
        if (espera2 == 0) {
          if (DER) {
            Right(50, 50, 50);  // Girar a la derecha
          }
          if (IZQ) {
            Left(50, 50, 50);   // Girar a la izquierda
          }
        } else {
          Foward(140, 144, 1); // Avanzar
        }
        break;
    }

  }
  if(digitalRead(START)==LOW){
    MotorOff();
  }
}







void CurveRight() {
  while (digitalRead(START) == HIGH) {
    Foward(255, 80, 100);
    Brake(30);
    Main_Battle();
  }
}

void CurveLeft() {
  if (digitalRead(START) == HIGH) {
    Foward(80, 255, 400);
    Brake(30);
    Main_Battle();
  }
}

void DodgeRight() {   //NEW
  while (digitalRead(START) == HIGH) {
    Left(255, 255, 50);
    Brake(30);
    Foward(255, 245, 200);
    Brake(30);
    Right(255, 255, 100);
    Brake(30);
    Main_Battle();
  }
}

void DodgeLeft() {  //NEW
  while (digitalRead(START) == HIGH) {
    Right(255, 255, 50);
    Brake(30);
    Foward(255, 245, 200);
    Brake(30);
    Left(255, 255, 100);
    Brake(30);
    Main_Battle();
  }
}


void SpinRight() {
  while (digitalRead(START) == HIGH) {
    Left(255, 255, 125);
    Brake(30);
    Main_Battle();
  }
}

void SpinLeft() {
  if (digitalRead(START) == HIGH) {
    Right(255, 255, 100);
    Brake(30);
    Main_Battle();
  }
}

void Rocket() {
  if (digitalRead(START) == HIGH) {
    Foward(255, 245, 300);
    Main_Battle();
  }
}

void Behind() {
  if (digitalRead(START) == HIGH) {
    Left(255, 255, 220);
    Brake(30);
    Main_Battle();
  }
}


void loop() {
    Brake(2000);
    Foward(255, 255, 800); 
    Brake(2000);
      
   Backwards(255, 255, 800);
  Brake(2000);

    Right (255, 255, 1000);
  Brake(2000);
     Left (255, 255, 1000);         
                     //Brake(2000);         
}
