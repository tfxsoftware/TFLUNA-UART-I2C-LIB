// Trigger Mode Example for TF-Luna LiDAR Module
// This example demonstrates how to use the TF-Luna in trigger mode

#include <TFLuna.h>

// Create TFLuna instance with Serial1
TFLuna tfLuna(&Serial1);

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("TF-Luna Trigger Mode Example");
  
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
  
  // Set trigger mode
  if (tfLuna.setTriggerMode()) {
    Serial.println("Set to trigger mode");
  } else {
    Serial.println("Failed to set trigger mode");
  }
  
  // Save settings
  if (tfLuna.setSaveSettings()) {
    Serial.println("Settings saved");
  } else {
    Serial.println("Failed to save settings");
  }
  
  Serial.println("Setup complete. Press any key to trigger a measurement...");
}

void loop() {
  // Wait for user input to trigger a measurement
  if (Serial.available()) {
    // Clear the input buffer
    while (Serial.available()) {
      Serial.read();
    }
    
    Serial.println("Triggering measurement...");
    
    // Trigger a measurement
    if (tfLuna.triggerSample()) {
      Serial.println("Measurement triggered");
      
      // Wait a moment for the measurement to complete
      delay(50);
      
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
    } else {
      Serial.println("Failed to trigger measurement");
    }
    
    Serial.println("\nPress any key to trigger another measurement...");
  }
  
  // Small delay to prevent CPU hogging
  delay(10);
}
