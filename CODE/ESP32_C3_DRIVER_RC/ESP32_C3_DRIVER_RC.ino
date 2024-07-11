#include <Bluepad32.h>

// Pinout del Puente H
const int motorA1 = 10; // Pin de dirección 1 para motor A
const int motorA2 = 7; // Pin de dirección 2 para motor A
const int motorB1 = 6; // Pin de dirección 1 para motor B
const int motorB2 = 5; // Pin de dirección 2 para motor B

// Velocidades de los motores
int speedA = 0;
int speedB = 0;

// Umbral de deriva
int driftThreshold = 65; // Ajusta este valor según sea necesario

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (!myControllers[i]) {
            myControllers[i] = ctl;
            Serial.printf("Controller connected at index %d\n", i);
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr;
            Serial.printf("Controller disconnected from index %d\n", i);
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(motorA1, OUTPUT);
    pinMode(motorA2, OUTPUT);
    pinMode(motorB1, OUTPUT);
    pinMode(motorB2, OUTPUT);

    // Inicializar Bluepad32
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false); // Opcional: deshabilitar el dispositivo virtual

    Serial.println("Bluepad32 inicializado");
}

void loop() {
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        processControllers();
    }
    delay(150); // Retardo para evitar saturación del bucle
}

void processControllers() {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] && myControllers[i]->isConnected() && myControllers[i]->hasData()) {
            // Obtener el valor del joystick izquierdo en el eje Y
            int joystickLY = myControllers[i]->axisY();

            // Obtener el valor del joystick derecho en el eje X para giros
            int joystickRX = myControllers[i]->axisRX(); // Joystick derecho para giros

            // Aplicar ajuste de drift a los joysticks
            joystickLY = adjustJoystick(joystickLY);
            joystickRX = adjustJoystick(joystickRX);

            // Calcular la velocidad y dirección del movimiento
            speedA = joystickLY + joystickRX;
            speedB = joystickLY - joystickRX;

            // Limitar el rango de velocidad entre -255 y 255
            speedA = constrain(speedA, -255, 255);
            speedB = constrain(speedB, -255, 255);

            // Asignar la dirección y velocidad a los motores usando PWM en ambos pines
            controlMotor(motorA1, motorA2, speedA);
            controlMotor(motorB1, motorB2, speedB);
        }
    }
}

void controlMotor(int pin1, int pin2, int speed) {
    if (speed >= 0) {
        analogWrite(pin1, speed);
        analogWrite(pin2, 0);
    } else {
        analogWrite(pin1, 0);
        analogWrite(pin2, -speed);
    }
}

int adjustJoystick(int value) {
    // Aplicar ajuste de drift al joystick
    if (abs(value) < driftThreshold) {
        return 0; // Si el valor del joystick está dentro del umbral de deriva, establecerlo en cero
    } else {
        return value; // Si está fuera del umbral de deriva, mantener el valor original
    }
}
