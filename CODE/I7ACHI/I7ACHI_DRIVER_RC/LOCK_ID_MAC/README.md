# How does the code work?
This is the documentation of the `LOCK_ID_MAC` code.
It will explain how some of the most important functions of this code work.

## 1. Callback for Connections (`onConnectedController`)
```cpp
void onConnectedController(ControllerPtr ctl) {
    Serial.println("Controller connected");
    Serial.printf("Model: %s\n", ctl->getModelName()); 
    const auto& properties = ctl->getProperties(); 
    Serial.printf("Vendor ID: %04X\n", properties.vendor_id);
    Serial.printf("Product ID: %04X\n", properties.product_id); ID
    Serial.printf("BT address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], 
                  properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
}
```
### Function
This function is automatically executed when a Bluetooth controller connects to the ESP32.
It logs key information about the connected controller to the serial monitor:
- **Model Name:** Identifies the brand and type of the connected device.
- **Vendor ID**: A hexadecimal code identifying the manufacturer of the controller.
- **Product ID:** A hexadecimal code specifying the exact model of the controller.
- **Bluetooth Address:** The unique Bluetooth address of the device in hexadecimal format.
> [!IMPORTANT]
> Verifies that the connected device is the intended controller, making it easier to identify it if there are multiple devices nearby. Useful for pairing the correct controller or troubleshooting connection issues.

## 2. Setup (`setup`)
```cpp
void setup() {
    Serial.begin(115200);
    BP32.setup(&onConnectedController, nullptr); 
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);
    Serial.println("Bluepad32 initialized");
}
```
### Function

- **Serial Communication Initialization:** 
`Serial.begin(115200)` :Sets up serial communication at 115200 baud rate, useful for debugging and monitoring.

- **Bluepad32 Setup** 
`BP32.setup(&onConnectedController, nullptr)`
	- Defines the onConnectedController function as the callback to be executed when a controller connects.
	- In this case, no callback is defined for disconnections, so nullptr is passed.

- **Forget Previously Paired Bluetooth Keys:**
`BP32.forgetBluetoothKeys()` ensures that any previously paired Bluetooth keys are erased, allowing only new devices to connect.

- **Disable Virtual Devices**
`BP32.enableVirtualDevice(false)` disables the virtual gamepad feature, ensuring it doesn't interfere with actual controllers.

- **Confirmation of Initialization**
A message is printed to the serial monitor to indicate that Bluepad32 has been initialized successfully.

> [!IMPORTANT]
> Configures the environment to detect and manage controller connections.

### 3. Loop (`loop`)
```cpp
void loop() {
    BP32.update(); // Update the state
}
```
### Function

1. This function runs in an infinite loop and continuously updates the state of the connected controllers.
2. It detects new events such as controller connections or changes in the state of already connected devices.

## SERIAL MONITOR EXAMPLE
```cpp
Device Information service found
Controller connected
Model: Xbox One
Vendor ID: 035F
Product ID: 1A15
BT address: 9C : BB : 1C : 52 : B4 : D9 //THIS ONE
```
# SUMMARY
> [!TIP]
> The table is referenced for a better understanding of which functions were created by me.

Type of Function   | Name
------------- | -------------
Native of Library  | `BP32.setup()`,`BP32.update()`,`BP32.forgetBluetoothKeys()`,`BP32.enableVirtualDevice()`,`ctl->getModelName()`,`ctl->getProperties()`
AT4NI  | ` onConnectedController(ControllerPtr ctl)`, `onDisconnectedController(ControllerPtr ctl`