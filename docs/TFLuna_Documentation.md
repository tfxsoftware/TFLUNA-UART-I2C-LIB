# TF-Luna LiDAR Arduino Library Documentation

## Overview
This document provides detailed information about the TF-Luna LiDAR Arduino library, including hardware specifications, communication protocols, library architecture, and usage examples.

## Table of Contents
1. [Hardware Specifications](#hardware-specifications)
2. [Communication Protocols](#communication-protocols)
3. [Library Architecture](#library-architecture)
4. [Basic Usage](#basic-usage)
5. [Advanced Features](#advanced-features)
6. [Troubleshooting](#troubleshooting)
7. [API Reference](#api-reference)

## Hardware Specifications

The TF-Luna is a single-point ranging LiDAR sensor based on the Time of Flight (TOF) principle:

| Parameter | Specification |
|-----------|---------------|
| Range | 0.2m - 8m (depending on reflectivity and ambient light) |
| Accuracy | ±6cm (0.2m-3m), ±2% (3m-8m) |
| Distance Resolution | 1cm |
| Frame Rate | 1-250Hz (adjustable) |
| Field of View | 2° |
| Power Supply | 3.7V - 5.2V |
| Average Current | ≤70mA |
| Peak Current | 150mA |
| Communication | UART (default) / I2C |
| Default Baud Rate | 115200 |
| Default I2C Address | 0x10 |
| Dimensions | 35mm × 21.25mm × 13.5mm |
| Weight | <5g |

## Communication Protocols

### UART Mode
- Default communication mode
- Settings: 115200 baud, 8 data bits, 1 stop bit, no parity
- Data frame format: [0x59][0x59][Dist_L][Dist_H][Strength_L][Strength_H][Temp_L][Temp_H][Checksum]
- Command format: [0x5A][Length][Cmd][Payload][Checksum]

### I2C Mode
- Set by connecting pin 5 to GND before powering on
- Default I2C address: 0x10 (configurable from 0x08 to 0x77)
- Maximum transmission rate: 400kbps
- Register-based communication

## Library Architecture

The library consists of two main classes:

1. **TFLuna**: Base class providing core functionality for both UART and I2C modes
   - Data acquisition
   - Configuration methods
   - Error handling

2. **TFLunaAdvanced**: Extended class with additional features
   - Distance filtering (median and average)
   - Unit conversions
   - Signal quality assessment
   - Data logging
   - Distance thresholds and callbacks

## Basic Usage

### UART Mode

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
  }
  delay(100);  // Wait 100ms between readings
}
```

### I2C Mode

```cpp
#include <TFLuna.h>
#include <Wire.h>

// Create TFLuna instance for I2C
TFLuna tfLuna;
const uint8_t TF_LUNA_ADDRESS = 0x10;  // Default I2C address

void setup() {
  Serial.begin(115200);  // Initialize serial monitor
  tfLuna.beginI2C();     // Initialize TF-Luna in I2C mode
}

void loop() {
  if (tfLuna.getDataI2C(TF_LUNA_ADDRESS)) {
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
  }
  delay(100);  // Wait 100ms between readings
}
```

## Advanced Features

### Distance Filtering

```cpp
#include <TFLunaAdvanced.h>

TFLunaAdvanced tfLuna(&Serial1);

void setup() {
  Serial.begin(115200);
  tfLuna.begin(115200);
  
  // Enable median filter with window size of 5
  tfLuna.enableMedianFilter(5);
  
  // Or enable average filter
  // tfLuna.enableAverageFilter(5);
}

void loop() {
  if (tfLuna.getData()) {
    // Distance will be filtered automatically
    Serial.print("Filtered Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
  }
  delay(100);
}
```

### Distance Thresholds and Callbacks

```cpp
#include <TFLunaAdvanced.h>

TFLunaAdvanced tfLuna(&Serial1);

void minDistanceCallback(uint16_t distance) {
  Serial.print("WARNING: Object too close! Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}

void setup() {
  Serial.begin(115200);
  tfLuna.begin(115200);
  
  // Set minimum distance threshold with callback
  tfLuna.setMinDistanceThreshold(30, minDistanceCallback);
}

void loop() {
  if (tfLuna.getData()) {
    // Threshold will be checked automatically
    Serial.print("Distance: ");
    Serial.print(tfLuna.getDistance());
    Serial.println(" cm");
  }
  delay(100);
}
```

### Data Logging

```cpp
#include <TFLunaAdvanced.h>

TFLunaAdvanced tfLuna(&Serial1);

void setup() {
  Serial.begin(115200);
  tfLuna.begin(115200);
  
  // Enable logging to Serial
  tfLuna.beginLogging(&Serial);
}

void loop() {
  tfLuna.getData();  // Data will be logged automatically
  delay(100);
}
```

## Troubleshooting

### Common Issues

1. **No data received in UART mode**
   - Check wiring: TXD of TF-Luna to RXD of Arduino, RXD of TF-Luna to TXD of Arduino
   - Verify baud rate (default is 115200)
   - Ensure pin 5 is NOT connected to GND

2. **No data received in I2C mode**
   - Check wiring: SDA and SCL connections
   - Verify I2C address (default is 0x10)
   - Ensure pin 5 is connected to GND before powering on

3. **Incorrect distance readings**
   - Check if the sensor is within the valid range (0.2m - 8m)
   - Verify that the target has sufficient reflectivity
   - Check for interference from ambient light
   - Try enabling filtering for noisy environments

### Error Codes

| Error Code | Description |
|------------|-------------|
| 0 | No error |
| 1 | Serial communication error |
| 2 | Checksum error |
| 3 | Timeout error |
| 4 | Header error |
| 5 | I2C NACK error |
| 6 | I2C data error |
| 7 | Invalid parameter |

## API Reference

### TFLuna Class

#### Constructors
- `TFLuna()`: For I2C mode
- `TFLuna(HardwareSerial* serial)`: For UART mode with HardwareSerial
- `TFLuna(Stream* stream)`: For UART mode with any Stream

#### Initialization
- `bool begin(uint32_t baudRate = 115200)`: Initialize UART mode
- `bool beginI2C()`: Initialize I2C mode

#### Data Acquisition
- `bool getData()`: Get data in UART mode
- `bool getDataI2C(uint8_t addr = 0x10)`: Get data in I2C mode

#### Data Accessors
- `uint16_t getDistance() const`: Get distance in cm
- `uint16_t getSignalStrength() const`: Get signal strength
- `int16_t getTemperature() const`: Get temperature in 0.01°C
- `uint8_t getErrorCode() const`: Get last error code

#### Configuration Methods (UART)
- `bool setFrameRate(uint16_t frameRate)`
- `bool setSaveSettings()`
- `bool setSoftReset()`
- `bool setHardReset()`
- `bool setTriggerMode()`
- `bool setContinuousMode()`
- `bool triggerSample()`
- `bool setEnable()`
- `bool setDisable()`

#### Configuration Methods (I2C)
- `bool setFrameRateI2C(uint16_t frameRate, uint8_t addr = 0x10)`
- `bool setI2CAddress(uint8_t newAddr, uint8_t currentAddr = 0x10)`
- `bool setSaveSettingsI2C(uint8_t addr = 0x10)`
- `bool setSoftResetI2C(uint8_t addr = 0x10)`
- `bool setHardResetI2C(uint8_t addr = 0x10)`
- `bool setTriggerModeI2C(uint8_t addr = 0x10)`
- `bool setContinuousModeI2C(uint8_t addr = 0x10)`
- `bool triggerSampleI2C(uint8_t addr = 0x10)`
- `bool setEnableI2C(uint8_t addr = 0x10)`
- `bool setDisableI2C(uint8_t addr = 0x10)`

#### Information Methods (I2C)
- `bool getFirmwareVersion(uint8_t version[3], uint8_t addr = 0x10)`
- `bool getFrameRate(uint16_t &frameRate, uint8_t addr = 0x10)`
- `bool getProductCode(char code[14], uint8_t addr = 0x10)`
- `bool getTime(uint16_t &time, uint8_t addr = 0x10)`

### TFLunaAdvanced Class

#### Distance Filtering Methods
- `void enableMedianFilter(uint8_t windowSize = 5)`
- `void disableMedianFilter()`
- `void enableAverageFilter(uint8_t windowSize = 5)`
- `void disableAverageFilter()`

#### Distance Conversion Methods
- `float getDistanceInMeters() const`
- `float getDistanceInInches() const`

#### Temperature Conversion Methods
- `float getTemperatureInCelsius() const`
- `float getTemperatureInFahrenheit() const`

#### Signal Quality Assessment
- `bool isSignalReliable() const`
- `uint8_t getSignalQuality() const`: Returns 0-100%

#### Data Logging
- `void beginLogging(Stream* logStream)`
- `void endLogging()`

#### Distance Thresholds and Callbacks
- `void setMinDistanceThreshold(uint16_t threshold, DistanceCallback callback = nullptr)`
- `void setMaxDistanceThreshold(uint16_t threshold, DistanceCallback callback = nullptr)`
- `void clearDistanceThresholds()`
- `bool checkThresholds()`
