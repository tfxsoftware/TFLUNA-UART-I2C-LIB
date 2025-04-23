// UART Example for TF-Luna LiDAR Module
// This example demonstrates how to use the TFLuna library in UART mode

#include <TFLuna.h>

// Create TFLuna instance with Serial1
// If your Arduino doesn't have Serial1, you can use SoftwareSerial instead
TFLuna tfLuna(&Serial1);

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("TF-Luna UART Example");
  
  // Initialize TF-Luna with 115200 baud rate (default)
  if (tfLuna.begin(115200)) {
    Serial.println("TF-Luna initialized successfully");
  } else {
    Serial.print("Failed to initialize TF-Luna. Error code: ");
    Serial.println(tfLuna.getErrorCode());
    while (1) {
      ; // Halt if initialization fails
    }
  }
  
  // Set continuous mode (default)
  if (tfLuna.setContinuousMode()) {
    Serial.println("Set to continuous mode");
  } else {
    Serial.println("Failed to set continuous mode");
  }
  
  // Set frame rate to 10Hz
  if (tfLuna.setFrameRate(10)) {
    Serial.println("Frame rate set to 10Hz");
  } else {
    Serial.println("Failed to set frame rate");
  }
  
  // Save settings
  if (tfLuna.setSaveSettings()) {
    Serial.println("Settings saved");
  } else {
    Serial.println("Failed to save settings");
  }
}

void loop() {
  // Get data from TF-Luna
  if (tfLuna.getData()) {
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
