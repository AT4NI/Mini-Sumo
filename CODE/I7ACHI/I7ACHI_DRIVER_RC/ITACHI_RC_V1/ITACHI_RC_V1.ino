#include <Bluepad32.h>

// Example: Authorized Bluetooth address
const byte authorizedBTAddress[6] = {0x9C, 0xAA, 0x1B, 0x51, 0xB6, 0xD6};

// Motor driver pinout
const int motorA1 = 10; // Motor A direction control pin 1
const int motorA2 = 7;  // Motor A direction control pin 2
const int motorB1 = 6;  // Motor B direction control pin 1
const int motorB2 = 5;  // Motor B direction control pin 2

int speedA = 0; // Speed for Motor A
int speedB = 0; // Speed for Motor B

int driftThreshold = 65; // Threshold for joystick drift correction

const int joystickMaxInput = 255;   // Maximum joystick input value
const int joystickMaxOutput = 185; // Maximum output value after scaling

ControllerPtr myControllers[BP32_MAX_GAMEPADS]; // Array to store connected controllers

// Callback function when a controller connects
void onConnectedController(ControllerPtr ctl) {
    const auto& properties = ctl->getProperties(); // Retrieve controller properties
    
    // Check if the controller's Bluetooth address matches the authorized one
    bool isAuthorized = true;
    for (int i = 0; i < 6; i++) {
        if (properties.btaddr[i] != authorizedBTAddress[i]) {
            isAuthorized = false; // If any byte differs, mark as unauthorized
            break;
        }
    }

    if (isAuthorized) {
        // Store the authorized controller in the first available slot
        for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
            if (!myControllers[i]) {
                myControllers[i] = ctl;
                Serial.printf("Authorized controller connected at index %d\n", i);
                break;
            }
        }
    } else {
        // Disconnect unauthorized controller
        Serial.println("Unauthorized controller attempted to connect. Disconnecting...");
        ctl->disconnect();
    }
}

// Callback function when a controller disconnects
void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            myControllers[i] = nullptr; // Remove the disconnected controller
            Serial.printf("Controller disconnected from index %d\n", i);
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);

    // Set motor pins as outputs
    pinMode(motorA1, OUTPUT);
    pinMode(motorA2, OUTPUT);
    pinMode(motorB1, OUTPUT);
    pinMode(motorB2, OUTPUT);

    // Initialize Bluepad32 library
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys(); // Forget previous Bluetooth pairings
    BP32.enableVirtualDevice(false); // Optionally disable virtual gamepad

    Serial.println("Bluepad32 initialized");
}

void loop() {
    bool dataUpdated = BP32.update(); // Update Bluepad32 state
    if (dataUpdated) {
        processControllers(); // Process data from connected controllers
    }
    delay(150); // Delay to prevent loop saturation
}

void processControllers() {
    // Iterate through all connected controllers
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] && myControllers[i]->isConnected() && myControllers[i]->hasData()) {
            // Read the left joystick Y-axis for forward/backward motion
            int joystickLY = myControllers[i]->axisY();

            // Read the right joystick X-axis for turning
            int joystickRX = myControllers[i]->axisRX();

            // Apply drift correction to joystick values
            joystickLY = adjustJoystick(joystickLY);
            joystickRX = adjustJoystick(joystickRX);

            // Map joystick input to output range
            joystickRX = map(joystickRX, -joystickMaxInput, joystickMaxInput, -joystickMaxOutput, joystickMaxOutput);

            // Calculate motor speeds based on joystick input
            int speedA = joystickLY + joystickRX; // Left motor
            int speedB = joystickLY - joystickRX; // Right motor
            
            // Constrain motor speeds to valid PWM range
            speedA = constrain(speedA, -255, 255);
            speedB = constrain(speedB, -255, 255);

            // Control motors based on calculated speeds
            controlMotor(motorA1, motorA2, speedA);
            controlMotor(motorB1, motorB2, speedB);
        }
    }
}

void controlMotor(int pin1, int pin2, int speed) {
    // Set motor direction and speed
    if (speed >= 0) {
        analogWrite(pin1, speed); // Forward direction
        analogWrite(pin2, 0);
    } else {
        analogWrite(pin1, 0);
        analogWrite(pin2, -speed); // Reverse direction
    }
}

int adjustJoystick(int value) {
    // Apply drift correction to joystick input
    if (abs(value) < driftThreshold) {
        return 0; // Ignore small movements within drift threshold
    } else {
        return value; // Return original value if outside threshold
    }
}
