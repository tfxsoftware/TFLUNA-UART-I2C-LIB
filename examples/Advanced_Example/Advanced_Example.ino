// Advanced Example for TF-Luna LiDAR Module
// This example demonstrates the advanced features of the TFLuna library

#include <TFLunaAdvanced.h>

// Create TFLunaAdvanced instance with Serial1
TFLunaAdvanced tfLuna(&Serial1);

// Callback functions for distance thresholds
void minDistanceCallback(uint16_t distance) {
  Serial.print("WARNING: Object too close! Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void maxDistanceCallback(uint16_t distance) {
  Serial.print("WARNING: Object too far! Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);
  while (!Serial) {
    ; // Wait for serial port to connect
  }
  
  Serial.println("TF-Luna Advanced Example");
  
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
  
  // Enable median filter with window size of 5
  tfLuna.enableMedianFilter(5);
  Serial.println("Median filter enabled with window size of 5");
  
  // Set distance thresholds with callbacks
  tfLuna.setMinDistanceThreshold(30, minDistanceCallback);  // 30cm minimum
  tfLuna.setMaxDistanceThreshold(500, maxDistanceCallback); // 500cm maximum
  Serial.println("Distance thresholds set: 30cm min, 500cm max");
  
  // Enable logging to Serial
  tfLuna.beginLogging(&Serial);
  Serial.println("Logging enabled");
  
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
  
  Serial.println("Setup complete. Starting measurements...");
  Serial.println();
  
  // Disable logging to avoid duplicate output
  tfLuna.endLogging();
}

void loop() {
  // Get data from TF-Luna
  if (tfLuna.getData()) {
    // Print distance in different units
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.print(" cm / ");
    Serial.print(tfLuna.getDistanceInMeters(), 2);
    Serial.print(" m / ");
    Serial.print(tfLuna.getDistanceInInches(), 1);
    Serial.println(" inches");
    
    // Print signal strength and quality
    Serial.print("Signal Strength: ");
    Serial.print(tfLuna.getSignalStrength());
    Serial.print(" (Quality: ");
    Serial.print(tfLuna.getSignalQuality());
    Serial.print("%, ");
    Serial.print(tfLuna.isSignalReliable() ? "Reliable" : "Unreliable");
    Serial.println(")");
    
    // Print temperature in different units
    Serial.print("Temperature: ");
    Serial.print(tfLuna.getTemperatureInCelsius(), 2);
    Serial.print(" °C / ");
    Serial.print(tfLuna.getTemperatureInFahrenheit(), 2);
    Serial.println(" °F");
    
    Serial.println();
  } else {
    // Print error if data acquisition fails
    Serial.print("Error: ");
    Serial.println(tfLuna.getErrorCode());
  }
  
  // Wait 500ms before next reading
  delay(500);
}
