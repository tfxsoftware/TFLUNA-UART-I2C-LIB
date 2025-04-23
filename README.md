# TFLuna Arduino Library

This Arduino library provides an interface for the TF-Luna LiDAR Module sensor, supporting both UART and I2C communication modes.

## Features

- Support for both UART and I2C communication interfaces
- Distance measurement (0.2m to 8m range)
- Signal strength reading
- Temperature monitoring
- Configuration of sensor parameters (frame rate, I2C address, etc.)
- Support for continuous and trigger measurement modes
- Error handling and status reporting

## Installation

1. Download the library as a ZIP file
2. In the Arduino IDE, go to Sketch > Include Library > Add .ZIP Library...
3. Select the downloaded ZIP file
4. The library will be installed and available in the Arduino IDE

## Usage

See the examples folder for detailed usage examples.

### Basic Usage (UART Mode)

```cpp
#include <TFLuna.h>

// Create TFLuna instance with Serial1
TFLuna tfLuna(&Serial1);

void setup() {
  Serial.begin(115200);  // Initialize serial monitor
  tfLuna.begin(115200);  // Initialize TF-Luna with 115200 baud rate
}

void loop() {
  if (tfLuna.getData()) {
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
    
    Serial.print("Signal Strength: ");
    Serial.println(tfLuna.getSignalStrength());
    
    Serial.print("Temperature: ");
    Serial.print(tfLuna.getTemperature() / 100.0);
    Serial.println(" °C");
  } else {
    Serial.print("Error: ");
    Serial.println(tfLuna.getErrorCode());
  }
  
  delay(100);  // Wait 100ms between readings
}
```

### Basic Usage (I2C Mode)

```cpp
#include <TFLuna.h>

// Create TFLuna instance for I2C
TFLuna tfLuna;
const uint8_t TF_LUNA_ADDRESS = 0x10;  // Default I2C address

void setup() {
  Serial.begin(115200);  // Initialize serial monitor
  Wire.begin();          // Initialize I2C
  tfLuna.beginI2C();     // Initialize TF-Luna in I2C mode
}

void loop() {
  if (tfLuna.getDataI2C(TF_LUNA_ADDRESS)) {
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
    
    Serial.print("Signal Strength: ");
    Serial.println(tfLuna.getSignalStrength());
    
    Serial.print("Temperature: ");
    Serial.print(tfLuna.getTemperature() / 100.0);
    Serial.println(" °C");
  } else {
    Serial.print("Error: ");
    Serial.println(tfLuna.getErrorCode());
  }
  
  delay(100);  // Wait 100ms between readings
}
```

## License

This library is released under the MIT License.
