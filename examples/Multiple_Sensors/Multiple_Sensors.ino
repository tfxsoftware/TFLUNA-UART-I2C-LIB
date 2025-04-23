// Multiple Sensors Example for TF-Luna LiDAR Module
// This example demonstrates how to use multiple TF-Luna sensors with I2C

#include <TFLuna.h>
#include <Wire.h>

// Create TFLuna instances for I2C
TFLuna tfLuna1;
TFLuna tfLuna2;

// I2C addresses for the sensors
// Note: The default address is 0x10, you need to change the address of one sensor
const uint8_t TF_LUNA_ADDR1 = 0x10; // Default address
const uint8_t TF_LUNA_ADDR2 = 0x11; // Changed address

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("TF-Luna Multiple Sensors Example");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize first TF-Luna sensor
  if (tfLuna1.beginI2C()) {
    Serial.println("First TF-Luna initialized successfully");
  } else {
    Serial.print("Failed to initialize first TF-Luna. Error code: ");
    Serial.println(tfLuna1.getErrorCode());
    while (1) {
      ; // Halt if initialization fails
    }
  }
  
  // Initialize second TF-Luna sensor
  if (tfLuna2.beginI2C()) {
    Serial.println("Second TF-Luna initialized successfully");
  } else {
    Serial.print("Failed to initialize second TF-Luna. Error code: ");
    Serial.println(tfLuna2.getErrorCode());
    while (1) {
      ; // Halt if initialization fails
    }
  }
  
  // Note: Before using this example, you need to change the I2C address of one sensor
  // This can be done using the following code (uncomment to use):
  /*
  // Change the address of the second sensor (connect only one sensor at a time)
  if (tfLuna2.setI2CAddress(TF_LUNA_ADDR2, TF_LUNA_ADDR1)) {
    Serial.println("Address changed successfully");
    Serial.println("Please power cycle the sensor for the new address to take effect");
    while (1) {
      ; // Halt after changing address
    }
  } else {
    Serial.println("Failed to change address");
  }
  */
  
  Serial.println("Setup complete. Starting measurements...");
  Serial.println();
}

void loop() {
  // Get data from first TF-Luna
  if (tfLuna1.getDataI2C(TF_LUNA_ADDR1)) {
    Serial.print("Sensor 1 - Distance: ");
    Serial.print(tfLuna1.getDistance());
    Serial.print(" cm, Strength: ");
    Serial.print(tfLuna1.getSignalStrength());
    Serial.print(", Temp: ");
    Serial.print(tfLuna1.getTemperature() / 100.0);
    Serial.println(" °C");
  } else {
    Serial.print("Sensor 1 - Error: ");
    Serial.println(tfLuna1.getErrorCode());
  }
  
  // Get data from second TF-Luna
  if (tfLuna2.getDataI2C(TF_LUNA_ADDR2)) {
    Serial.print("Sensor 2 - Distance: ");
    Serial.print(tfLuna2.getDistance());
    Serial.print(" cm, Strength: ");
    Serial.print(tfLuna2.getSignalStrength());
    Serial.print(", Temp: ");
    Serial.print(tfLuna2.getTemperature() / 100.0);
    Serial.println(" °C");
  } else {
    Serial.print("Sensor 2 - Error: ");
    Serial.println(tfLuna2.getErrorCode());
  }
  
  Serial.println();
  
  // Wait 100ms before next reading
  delay(100);
}
