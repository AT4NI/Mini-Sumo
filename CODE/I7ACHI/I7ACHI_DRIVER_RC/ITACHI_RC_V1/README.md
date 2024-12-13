# How does the code work?
This is the documentation of the `ITACHI_RC_V1` code.
It will explain how some of the most important functions of this code work.
The code has a BT ID block, so it does not allow the connection of any other controller than the authorised one.

Global Variables and Initial Setup

## Global Variables
```cpp
const byte authorizedBTAddress[6] = {0x9C, 0xAA, 0x1B, 0x51, 0xB6, 0xD6}; // Authorized Bluetooth address
const int motorA1 = 10; 
const int motorA2 = 7;  
const int motorB1 = 6; 
const int motorB2 = 5;  

int speedA = 0; 
int speedB = 0; 

int driftThreshold = 65; 
const int joystickMaxInput = 255;   
const int joystickMaxOutput = 185;  

ControllerPtr myControllers[BP32_MAX_GAMEPADS]; 
```
### Function
This function is automatically executed when a Bluetooth controller connects to the ESP32.
- **authorizedBTAddress:** Bluetooth address of the authorized controller. Only this controller will be allowed to connect and control the robot.
- **motorA1, motorA2, motorB1, motorB2:** Pins on the ESP32 assigned to control the two motors of the robot.
- **speedA, speedB** Variables storing the calculated speeds for each motor.
- **driftThreshold:** Threshold for joystick drift correction. If the joystick value is within this threshold, it is considered "No Movement".
- **joystickMaxInput, joystickMaxOutput** Define the maximum input and output ranges for the joysticks, respectively.
- **myControllers:** Array that stores the connected controllers.

## Callback for Controller Connection (`onConnectedController`)
```cpp
void onConnectedController(ControllerPtr ctl) {
    const auto& properties = ctl->getProperties(); // Get controller properties
    bool isAuthorized = true;
    for (int i = 0; i < 6; i++) {
        if (properties.btaddr[i] != authorizedBTAddress[i]) {
            isAuthorized = false; // If any byte differs, it's unauthorized
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
        ctl->disconnect(); // Disconnect unauthorized controller
    }
}

```
### Function

- `onConnectedController()` is a callback function triggered every time a controller attempts to connect to the ESP32.
- `properties.btaddr accesse` accesses the Bluetooth address of the controller attempting to connect. Each Bluetooth address is unique and used to identify each device.

### Authorisation by Bluetooth ID
**The authorization process verifies** that the Bluetooth address of the connecting controller matches the one stored in the authorizedBTAddress variable. This step is critical because it ensures that only a specific controller (the authorized one) can connect to and control the robot.
1. *A byte-by-byte comparison* is performed between the connecting controller's address (properties.btaddr) and the authorized address (authorizedBTAddress).
2. *If all bytes match*, `isAuthorized` is set to ` true` , meaning the controller is authorized. it is stored in the first available slot in the myControllers array so that it can be used to control the robot.
3. *If any byte doesn't match*, `isAuthorized` is set to ` false`  and A warning message is printed to the serial port, and the controller  is immediately disconnected using ctl->disconnect().

This authorization mechanism ensures that only pre-defined and authorized controllers are allowed to interact with the robot, enhancing the system's security and control.

------------

## Loop (`loop`)
```cpp
void loop() {
    bool dataUpdated = BP32.update(); 
    if (dataUpdated) {
        processControllers(); 
    }
    delay(150);
}
```
### Function
- `BP32.update()` updates the state of the Bluepad32 library, checking if new data is available from connected controllers.
- If `dataUpdated` is `true`, meaning new data is available from the controllers, `processControllers()` is called to process the movements and commands from the controllers.

## Processing Controller Data (`processControllers`)
```cpp
void processControllers() {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] && myControllers[i]->isConnected() && myControllers[i]->hasData()) {
            int joystickLY = myControllers[i]->axisY(); // Left joystick Y-axis (forward/backward movement)
            int joystickRX = myControllers[i]->axisRX(); // Right joystick X-axis (turning)

            joystickLY = adjustJoystick(joystickLY);  // Correct joystick drift
            joystickRX = adjustJoystick(joystickRX);  // Correct joystick drift

            joystickRX = map(joystickRX, -joystickMaxInput, joystickMaxInput, -joystickMaxOutput, joystickMaxOutput); // Scale

            int speedA = joystickLY + joystickRX; // Left motor speed
            int speedB = joystickLY - joystickRX; // Right motor speed

            speedA = constrain(speedA, -255, 255); // Limit the speed of motor A
            speedB = constrain(speedB, -255, 255); // Limit the speed of motor B

            controlMotor(motorA1, motorA2, speedA); // Control left motor
            controlMotor(motorB1, motorB2, speedB); // Control right motor
        }
    }
}
```
- **For each connected controller**, the joystick values are obtained:
	1. **axisY()** gets the Y-axis value from the left joystick, which is used for forward or backward movement of the robot.
	2. **axisRX()** gets the X-axis value from the right joystick, which is used to control the turning of the robot.
- **Drift** correction is applied to both joystick values using `adjustJoystick()`, ensuring that unwanted movements due to slight joystick deviations are ignored.
- **The `joystickRX` value** is mapped to an appropriate range for the motors using `map()`, ensuring the turning value is properly adjusted for motor control.
- **The speed** of the left (`speedA`) and right (`speedB`) motors is then calculated by adding and subtracting the joystick values. This formula allows the forward/backward movement and turning to be controlled independently.
- **Speeds** are limited with `constrain()` to the *range of -255 to 255*, ensuring valid PWM signals are sent to the motors.
- `controlMotor()` is called to send the control signals to the motors, enabling the robot to move according to the controller inputs.

## Motor Control (`controlMotor`)
```cpp
void controlMotor(int pin1, int pin2, int speed) {
    if (speed >= 0) {
        analogWrite(pin1, speed); // Forward direction
        analogWrite(pin2, 0);
    } else {
        analogWrite(pin1, 0);
        analogWrite(pin2, -speed); // Reverse direction
    }
}
```

`controlMotor()` controls a particular motor (either left or right) based on the value of speed.
- If speed is **positive** (moving forward), a PWM signal is sent to pin1 to make the motor rotate forward, while pin2 is kept low.
- If speed is **negative** (moving backward), pin1 is kept low, and a negative PWM signal is sent to pin2 to make the motor rotate in reverse.


# SUMMARY
> [!TIP]
> The table is referenced for a better understanding of which functions were created by me.

Type of Function   | Name
------------- | -------------
Native of Library  | `BP32.setup()`,`BP32.update()`,`BP32.forgetBluetoothKeys()`,`BP32.enableVirtualDevice()`,`ctl->getModelName()`,`ctl->getProperties()`
AT4NI  | ` onConnectedController(ControllerPtr ctl)`, `onDisconnectedController(ControllerPtr ctl)`, `processControllers()`,`controlMotor()`,`adjustJoystick()`


