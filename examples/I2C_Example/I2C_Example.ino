// I2C Example for TF-Luna LiDAR Module
// This example demonstrates how to use the TFLuna library in I2C mode

#include <TFLuna.h>
#include <Wire.h>

// Create TFLuna instance for I2C
TFLuna tfLuna;

// Default I2C address for TF-Luna
const uint8_t TF_LUNA_ADDRESS = 0x10;

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("TF-Luna I2C Example");
  
  // Initialize TF-Luna in I2C mode
  // Note: Make sure pin 5 of TF-Luna is connected to GND before powering on
  if (tfLuna.beginI2C()) {
    Serial.println("TF-Luna initialized in I2C mode");
  } else {
    Serial.print("Failed to initialize TF-Luna. Error code: ");
    Serial.println(tfLuna.getErrorCode());
    while (1) {
      ; // Halt if initialization fails
    }
  }
  
  // Set continuous mode (default)
  if (tfLuna.setContinuousModeI2C(TF_LUNA_ADDRESS)) {
    Serial.println("Set to continuous mode");
  } else {
    Serial.println("Failed to set continuous mode");
  }
  
  // Set frame rate to 10Hz
  if (tfLuna.setFrameRateI2C(10, TF_LUNA_ADDRESS)) {
    Serial.println("Frame rate set to 10Hz");
  } else {
    Serial.println("Failed to set frame rate");
  }
  
  // Save settings
  if (tfLuna.setSaveSettingsI2C(TF_LUNA_ADDRESS)) {
    Serial.println("Settings saved");
  } else {
    Serial.println("Failed to save settings");
  }
  
  // Get firmware version
  uint8_t version[3];
  if (tfLuna.getFirmwareVersion(version, TF_LUNA_ADDRESS)) {
    Serial.print("Firmware version: ");
    Serial.print(version[0]);
    Serial.print(".");
    Serial.print(version[1]);
    Serial.print(".");
    Serial.println(version[2]);
  } else {
    Serial.println("Failed to get firmware version");
  }
}

void loop() {
  // Get data from TF-Luna
  if (tfLuna.getDataI2C(TF_LUNA_ADDRESS)) {
    // Print distance
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
    
    // Print signal strength
    Serial.print("Signal Strength: ");
    Serial.println(tfLuna.getSignalStrength());
    
    // Print temperature
    Serial.print("Temperature: ");
    Serial.print(tfLuna.getTemperature() / 100.0);
    Serial.println(" °C");
  } else {
    // Print error if data acquisition fails
    Serial.print("Error: ");
    Serial.println(tfLuna.getErrorCode());
  }
  
  // Wait 100ms before next reading
  delay(100);
}
