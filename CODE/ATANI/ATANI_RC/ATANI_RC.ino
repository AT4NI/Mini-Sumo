    //MOTOR A
int Pin_AI1 = 5;
int Pin_AI2 = 4;
int Pin_PWA = 3;  //A PWM
  //MOTOR B
int Pin_BI1 = 7;
int Pin_BI2 = 8;
int Pin_PWB = 9;  //B PWM

unsigned long time = 0;

int sp1; // PWM for Motor A
int sp2; // PWM for Motor B

void setup() {
  Serial.begin(9600);
  pinMode(Pin_AI1, OUTPUT);
  pinMode(Pin_AI2, OUTPUT);
  pinMode(Pin_PWA, OUTPUT);

  pinMode(Pin_BI1, OUTPUT);
  pinMode(Pin_BI2, OUTPUT);
  pinMode(Pin_PWB, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    Serial.println(data);
    time = millis();
    
              //Setting PWM values
    switch (data) {
      case 'F':       //FOWARD
        sp1 = 255; 
        sp2 = 235; 
        Forward(sp1, sp2);
        break;
      case 'B':         //BACKWARD
        sp1 = 255; 
        sp2 = 255; 
        Backward(sp1, sp2);
        break;
      case 'L':       //TURN LEFT
        sp1 = 150; 
        sp2 = 150;
        Right(sp1, sp2);
        break;
      case 'R':     //TURN RIGHT
        sp1 = 150; 
        sp2 = 150; 
        Left(sp1, sp2);
        break;
        
                            //ROUND
      case 'G':
        sp1 = 80;
        sp2 = 255; 
        F_Left(sp1, sp2);
        break;

      case 'H':
        sp1 = 80; 
        sp2 = 255; 
        B_Left(sp1, sp2);
        break;

      case 'I':
        sp1 = 255; 
        sp2 = 80; 
        F_Right(sp1, sp2);
        break;

      case 'J':
        sp1 = 255; 
        sp2 = 80; 
        B_Right(sp1, sp2);
        break;

      default:
        Stop();
        break;
    }
  }

  if (millis() - time >= 100) { // If 100 ms have passed since the last command
    Stop();
  }
}

void Forward(int speed1, int speed2) {
  digitalWrite(Pin_AI1, HIGH);
  digitalWrite(Pin_AI2, LOW);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, HIGH);
  digitalWrite(Pin_BI2, LOW);
  analogWrite(Pin_PWB, speed2);
}

void Backward(int speed1, int speed2) {
  digitalWrite(Pin_AI1, LOW);
  digitalWrite(Pin_AI2, HIGH);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, LOW);
  digitalWrite(Pin_BI2, HIGH);
  analogWrite(Pin_PWB, speed2);
}

void Right(int speed1, int speed2) {
  digitalWrite(Pin_AI1, LOW);
  digitalWrite(Pin_AI2, HIGH);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, HIGH);
  digitalWrite(Pin_BI2, LOW);
  analogWrite(Pin_PWB, speed2);
}

void Left(int speed1, int speed2) {
  digitalWrite(Pin_AI1, HIGH);
  digitalWrite(Pin_AI2, LOW);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, LOW);
  digitalWrite(Pin_BI2, HIGH);
  analogWrite(Pin_PWB, speed2);
}

// GIROS

void F_Left(int speed1, int speed2) {
  digitalWrite(Pin_AI1, HIGH);
  digitalWrite(Pin_AI2, LOW);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, HIGH);
  digitalWrite(Pin_BI2, LOW);
  analogWrite(Pin_PWB, speed2);
}

void B_Left(int speed1, int speed2) {
  digitalWrite(Pin_AI1, LOW);
  digitalWrite(Pin_AI2, HIGH);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, LOW);
  digitalWrite(Pin_BI2, HIGH);
  analogWrite(Pin_PWB, speed2);
}

void B_Right(int speed1, int speed2) {
  digitalWrite(Pin_AI1, LOW);
  digitalWrite(Pin_AI2, HIGH);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, LOW);
  digitalWrite(Pin_BI2, HIGH);
  analogWrite(Pin_PWB, speed2);
}

void F_Right(int speed1, int speed2) {
  digitalWrite(Pin_AI1, HIGH);
  digitalWrite(Pin_AI2, LOW);
  analogWrite(Pin_PWA, speed1);

  digitalWrite(Pin_BI1, HIGH);
  digitalWrite(Pin_BI2, LOW);
  analogWrite(Pin_PWB, speed2);
}

void Stop() {
  digitalWrite(Pin_AI1, LOW);
  digitalWrite(Pin_AI2, LOW);
  analogWrite(Pin_PWA, 0); 

  digitalWrite(Pin_BI1, LOW);
  digitalWrite(Pin_BI2, LOW);
  analogWrite(Pin_PWB, 0); 
}
