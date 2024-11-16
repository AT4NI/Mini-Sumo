#include <Bluepad32.h>

// Dirección MAC de tu mando específico 9C:AA:1B:51:B6:D3
const uint8_t authorizedBTAddress[6] = {0x9C, 0xAA, 0x1B, 0x51, 0xB6, 0xD3};

// Pinout del Puente H
const int motorA1 = 10; 
const int motorA2 = 7;  
const int motorB1 = 6;  
const int motorB2 = 5;  

int speedA = 0;
int speedB = 0;

int driftThreshold = 65; // Ajusta este valor según sea necesario

const int joystickMaxInput = 255;  
const int joystickMaxOutput = 185;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    // Obtener la dirección Bluetooth del controlador conectado
    const auto& properties = ctl->getProperties();
    
    bool isAuthorized = true;
    for (int i = 0; i < 6; i++) {
        if (properties.btaddr[i] != authorizedBTAddress[i]) {
            isAuthorized = false;
            break;
        }
    }

    if (isAuthorized) {
        for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
            if (!myControllers[i]) {
                myControllers[i] = ctl;
                Serial.printf("Authorized controller connected at index %d\n", i);
                break;
            }
        }
    } else {
        Serial.println("Unauthorized controller attempted to connect. Disconnecting...");
        ctl->disconnect();
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
            int joystickRX = myControllers[i]->axisRX();

            // Aplicar ajuste de drift a los joysticks
            joystickLY = adjustJoystick(joystickLY);
            joystickRX = adjustJoystick(joystickRX);

            // Limitar el movimiento máximo del joystick
            joystickRX = map(joystickRX, -joystickMaxInput, joystickMaxInput, -joystickMaxOutput, joystickMaxOutput);

            // Calcular la velocidad y dirección del movimiento
            int speedA = joystickLY + joystickRX;
            int speedB = joystickLY - joystickRX;
            
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
