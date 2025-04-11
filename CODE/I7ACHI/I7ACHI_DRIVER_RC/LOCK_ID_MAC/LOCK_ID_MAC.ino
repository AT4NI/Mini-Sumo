
#include <Bluepad32.h>

// This function is called when a controller connects
void onConnectedController(ControllerPtr ctl) {
    Serial.println("Controller connected");

    // Print detailed information about the connected controller
    Serial.printf("Model: %s\n", ctl->getModelName()); // Print the model name of the controller

    // Print detailed information about the connected device
    const auto& properties = ctl->getProperties(); // Get properties of the controller
    Serial.printf("Vendor ID: %04X\n", properties.vendor_id); // Print Vendor ID
    Serial.printf("Product ID: %04X\n", properties.product_id); // Print Product ID
    Serial.printf("BT address: %02X:%02X:%02X:%02X:%02X:%02X\n",
                  properties.btaddr[0], properties.btaddr[1], properties.btaddr[2], // Print Bluetooth address
                  properties.btaddr[3], properties.btaddr[4], properties.btaddr[5]);
}

// This function is called when a controller disconnects
void onDisconnectedController(ControllerPtr ctl) {
    Serial.println("Controller disconnected"); // Notify when the controller disconnects
}

void setup() {
    Serial.begin(115200);
    //  START  Bluepad32
    BP32.setup(&onConnectedController, &onDisconnectedController); // Set callback functions for connect/disconnect
    BP32.forgetBluetoothKeys(); // Forget any previously paired Bluetooth keys
    BP32.enableVirtualDevice(false); // Optionally disable virtual gamepad

    Serial.println("Bluepad32 initialized");
}

void loop() {
    BP32.update(); // Update the state
}
