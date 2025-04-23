#ifndef TFLUNA_H
#define TFLUNA_H

#include <Arduino.h>
#include <Wire.h>

// Communication modes
#define TFLUNA_UART_MODE 0
#define TFLUNA_I2C_MODE  1

// Default I2C address
#define TFLUNA_DEFAULT_I2C_ADDR 0x10

// Error codes
#define TFLUNA_OK                  0
#define TFLUNA_ERROR_SERIAL        1
#define TFLUNA_ERROR_CHECKSUM      2
#define TFLUNA_ERROR_TIMEOUT       3
#define TFLUNA_ERROR_HEADER        4
#define TFLUNA_ERROR_I2C_NACK      5
#define TFLUNA_ERROR_I2C_DATA      6
#define TFLUNA_ERROR_INVALID_PARAM 7

// UART frame format
#define TFLUNA_FRAME_HEADER        0x59
#define TFLUNA_FRAME_LENGTH        9

// I2C registers
#define TFLUNA_I2C_DIST_L          0x00
#define TFLUNA_I2C_DIST_H          0x01
#define TFLUNA_I2C_STRENGTH_L      0x02
#define TFLUNA_I2C_STRENGTH_H      0x03
#define TFLUNA_I2C_TEMP_L          0x04
#define TFLUNA_I2C_TEMP_H          0x05
#define TFLUNA_I2C_FIRMWARE_L      0x0A
#define TFLUNA_I2C_FIRMWARE_M      0x0B
#define TFLUNA_I2C_FIRMWARE_H      0x0C
#define TFLUNA_I2C_SAVE_SETTINGS   0x20
#define TFLUNA_I2C_SOFT_RESET      0x21
#define TFLUNA_I2C_SET_I2C_ADDR    0x22
#define TFLUNA_I2C_FRAME_RATE      0x25
#define TFLUNA_I2C_TRIG_MODE       0x40
#define TFLUNA_I2C_CONT_MODE       0x41
#define TFLUNA_I2C_TRIG_SAMPLE     0x42
#define TFLUNA_I2C_ENABLE          0x60
#define TFLUNA_I2C_DISABLE         0x65

class TFLuna {
public:
    // Constructors
    TFLuna();                      // For I2C mode
    TFLuna(HardwareSerial* serial); // For UART mode with HardwareSerial
    TFLuna(Stream* stream);        // For UART mode with any Stream

    // Initialization
    bool begin(uint32_t baudRate = 115200);  // Initialize UART mode
    bool beginI2C();                         // Initialize I2C mode

    // Data acquisition
    bool getData();                          // Get data in UART mode
    bool getDataI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR); // Get data in I2C mode

    // Data accessors
    uint16_t getDistance() const;      // Get distance in cm
    uint16_t getSignalStrength() const; // Get signal strength
    int16_t getTemperature() const;    // Get temperature in 0.01°C
    uint8_t getErrorCode() const;      // Get last error code

    // Configuration methods (UART)
    bool setFrameRate(uint16_t frameRate);
    bool setSaveSettings();
    bool setSoftReset();
    bool setHardReset();
    bool setTriggerMode();
    bool setContinuousMode();
    bool triggerSample();
    bool setEnable();
    bool setDisable();

    // Configuration methods (I2C)
    bool setFrameRateI2C(uint16_t frameRate, uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setI2CAddress(uint8_t newAddr, uint8_t currentAddr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setSaveSettingsI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setSoftResetI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setHardResetI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setTriggerModeI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setContinuousModeI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool triggerSampleI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setEnableI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool setDisableI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);

    // Information methods (I2C)
    bool getFirmwareVersion(uint8_t version[3], uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool getFrameRate(uint16_t &frameRate, uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool getProductCode(char code[14], uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);
    bool getTime(uint16_t &time, uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR);

private:
    // Communication mode
    uint8_t _mode;

    // UART related
    Stream* _stream;
    bool _ownSerial;

    // Data storage
    uint16_t _distance;
    uint16_t _strength;
    int16_t _temperature;
    uint8_t _errorCode;

    // UART helper methods
    bool _readFrame();
    uint8_t _calculateChecksum(uint8_t *data, uint8_t length);
    bool _sendCommand(uint8_t cmd, uint8_t *payload = NULL, uint8_t payloadLen = 0);

    // I2C helper methods
    bool _writeRegister(uint8_t reg, uint8_t value, uint8_t addr);
    bool _writeRegister16(uint8_t reg, uint16_t value, uint8_t addr);
    bool _readRegister(uint8_t reg, uint8_t &value, uint8_t addr);
    bool _readRegister16(uint8_t reg, uint16_t &value, uint8_t addr);
    bool _readData(uint8_t addr);
};

#endif // TFLUNA_H
