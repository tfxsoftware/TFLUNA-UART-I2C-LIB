#include "TFLuna.h"

// Constructors
TFLuna::TFLuna() {
    _mode = TFLUNA_I2C_MODE;
    _stream = NULL;
    _ownSerial = false;
    _distance = 0;
    _strength = 0;
    _temperature = 0;
    _errorCode = TFLUNA_OK;
}

TFLuna::TFLuna(HardwareSerial* serial) {
    _mode = TFLUNA_UART_MODE;
    _stream = serial;
    _ownSerial = false;
    _distance = 0;
    _strength = 0;
    _temperature = 0;
    _errorCode = TFLUNA_OK;
}

TFLuna::TFLuna(Stream* stream) {
    _mode = TFLUNA_UART_MODE;
    _stream = stream;
    _ownSerial = false;
    _distance = 0;
    _strength = 0;
    _temperature = 0;
    _errorCode = TFLUNA_OK;
}

// Initialization
bool TFLuna::begin(uint32_t baudRate) {
    if (_mode != TFLUNA_UART_MODE || _stream == NULL) {
        _errorCode = TFLUNA_ERROR_SERIAL;
        return false;
    }
    
    // If using HardwareSerial, initialize it
    HardwareSerial* serial = dynamic_cast<HardwareSerial*>(_stream);
    if (serial != NULL) {
        serial->begin(baudRate);
        delay(100); // Give some time to initialize
    }
    
    // Clear any existing data
    while (_stream->available()) {
        _stream->read();
    }
    
    return true;
}

bool TFLuna::beginI2C() {
    _mode = TFLUNA_I2C_MODE;
    Wire.begin();
    delay(100); // Give some time to initialize
    return true;
}

// Data acquisition
bool TFLuna::getData() {
    if (_mode != TFLUNA_UART_MODE || _stream == NULL) {
        _errorCode = TFLUNA_ERROR_SERIAL;
        return false;
    }
    
    return _readFrame();
}

bool TFLuna::getDataI2C(uint8_t addr) {
    if (_mode != TFLUNA_I2C_MODE) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    return _readData(addr);
}

// Data accessors
uint16_t TFLuna::getDistance() const {
    return _distance;
}

uint16_t TFLuna::getSignalStrength() const {
    return _strength;
}

int16_t TFLuna::getTemperature() const {
    return _temperature;
}

uint8_t TFLuna::getErrorCode() const {
    return _errorCode;
}

// Configuration methods (UART)
bool TFLuna::setFrameRate(uint16_t frameRate) {
    uint8_t payload[2];
    payload[0] = frameRate & 0xFF;         // Low byte
    payload[1] = (frameRate >> 8) & 0xFF;  // High byte
    
    return _sendCommand(0x03, payload, 2);
}

bool TFLuna::setSaveSettings() {
    return _sendCommand(0x11);
}

bool TFLuna::setSoftReset() {
    bool result = _sendCommand(0x02);
    delay(100); // Give time for the device to reset
    return result;
}

bool TFLuna::setHardReset() {
    bool result = _sendCommand(0x01);
    delay(100); // Give time for the device to reset
    return result;
}

bool TFLuna::setTriggerMode() {
    return _sendCommand(0x04, NULL, 0);
}

bool TFLuna::setContinuousMode() {
    return _sendCommand(0x05, NULL, 0);
}

bool TFLuna::triggerSample() {
    return _sendCommand(0x06, NULL, 0);
}

bool TFLuna::setEnable() {
    return _sendCommand(0x07, NULL, 0);
}

bool TFLuna::setDisable() {
    return _sendCommand(0x08, NULL, 0);
}

// Configuration methods (I2C)
bool TFLuna::setFrameRateI2C(uint16_t frameRate, uint8_t addr) {
    return _writeRegister16(TFLUNA_I2C_FRAME_RATE, frameRate, addr);
}

bool TFLuna::setI2CAddress(uint8_t newAddr, uint8_t currentAddr) {
    if (newAddr < 0x08 || newAddr > 0x77) {
        _errorCode = TFLUNA_ERROR_INVALID_PARAM;
        return false;
    }
    
    bool result = _writeRegister(TFLUNA_I2C_SET_I2C_ADDR, newAddr, currentAddr);
    if (result) {
        // Need to reset for the new address to take effect
        result = setSoftResetI2C(currentAddr);
    }
    
    return result;
}

bool TFLuna::setSaveSettingsI2C(uint8_t addr) {
    bool result = _writeRegister(TFLUNA_I2C_SAVE_SETTINGS, 0x01, addr);
    delay(200); // Save settings can take time
    return result;
}

bool TFLuna::setSoftResetI2C(uint8_t addr) {
    bool result = _writeRegister(TFLUNA_I2C_SOFT_RESET, 0x02, addr);
    delay(100); // Give time for the device to reset
    return result;
}

bool TFLuna::setHardResetI2C(uint8_t addr) {
    bool result = _writeRegister(TFLUNA_I2C_SOFT_RESET, 0x01, addr);
    delay(200); // Hard reset takes longer
    return result;
}

bool TFLuna::setTriggerModeI2C(uint8_t addr) {
    return _writeRegister(TFLUNA_I2C_TRIG_MODE, 0x01, addr);
}

bool TFLuna::setContinuousModeI2C(uint8_t addr) {
    return _writeRegister(TFLUNA_I2C_CONT_MODE, 0x01, addr);
}

bool TFLuna::triggerSampleI2C(uint8_t addr) {
    return _writeRegister(TFLUNA_I2C_TRIG_SAMPLE, 0x01, addr);
}

bool TFLuna::setEnableI2C(uint8_t addr) {
    return _writeRegister(TFLUNA_I2C_ENABLE, 0x01, addr);
}

bool TFLuna::setDisableI2C(uint8_t addr) {
    return _writeRegister(TFLUNA_I2C_DISABLE, 0x01, addr);
}

// Information methods (I2C)
bool TFLuna::getFirmwareVersion(uint8_t version[3], uint8_t addr) {
    uint8_t value;
    
    if (!_readRegister(TFLUNA_I2C_FIRMWARE_L, value, addr)) {
        return false;
    }
    version[0] = value;
    
    if (!_readRegister(TFLUNA_I2C_FIRMWARE_M, value, addr)) {
        return false;
    }
    version[1] = value;
    
    if (!_readRegister(TFLUNA_I2C_FIRMWARE_H, value, addr)) {
        return false;
    }
    version[2] = value;
    
    return true;
}

bool TFLuna::getFrameRate(uint16_t &frameRate, uint8_t addr) {
    return _readRegister16(TFLUNA_I2C_FRAME_RATE, frameRate, addr);
}

bool TFLuna::getProductCode(char code[14], uint8_t addr) {
    // Product code is stored in registers 0x10-0x1D
    Wire.beginTransmission(addr);
    Wire.write(0x10);
    if (Wire.endTransmission() != 0) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    Wire.requestFrom(addr, (uint8_t)14);
    if (Wire.available() < 14) {
        _errorCode = TFLUNA_ERROR_I2C_DATA;
        return false;
    }
    
    for (uint8_t i = 0; i < 14; i++) {
        code[i] = Wire.read();
    }
    
    return true;
}

bool TFLuna::getTime(uint16_t &time, uint8_t addr) {
    // Time is stored in registers 0x30-0x31
    return _readRegister16(0x30, time, addr);
}

// Private helper methods
bool TFLuna::_readFrame() {
    uint8_t buffer[TFLUNA_FRAME_LENGTH];
    uint8_t checksum = 0;
    
    // Wait for header bytes
    uint32_t startTime = millis();
    int headerCount = 0;
    
    while (headerCount < 2) {
        if (millis() - startTime > 500) { // 500ms timeout
            _errorCode = TFLUNA_ERROR_TIMEOUT;
            return false;
        }
        
        if (_stream->available()) {
            uint8_t byte = _stream->read();
            if (byte == TFLUNA_FRAME_HEADER) {
                buffer[headerCount] = byte;
                headerCount++;
            } else {
                headerCount = 0; // Reset if we don't get consecutive header bytes
            }
        }
    }
    
    // Read the rest of the frame
    uint8_t bytesRead = 2;
    startTime = millis();
    
    while (bytesRead < TFLUNA_FRAME_LENGTH) {
        if (millis() - startTime > 500) { // 500ms timeout
            _errorCode = TFLUNA_ERROR_TIMEOUT;
            return false;
        }
        
        if (_stream->available()) {
            buffer[bytesRead] = _stream->read();
            bytesRead++;
        }
    }
    
    // Calculate checksum
    for (uint8_t i = 0; i < TFLUNA_FRAME_LENGTH - 1; i++) {
        checksum += buffer[i];
    }
    
    // Verify checksum
    if (checksum != buffer[TFLUNA_FRAME_LENGTH - 1]) {
        _errorCode = TFLUNA_ERROR_CHECKSUM;
        return false;
    }
    
    // Parse data
    _distance = (buffer[3] << 8) | buffer[2];
    _strength = (buffer[5] << 8) | buffer[4];
    _temperature = (buffer[7] << 8) | buffer[6];
    
    _errorCode = TFLUNA_OK;
    return true;
}

uint8_t TFLuna::_calculateChecksum(uint8_t *data, uint8_t length) {
    uint8_t sum = 0;
    for (uint8_t i = 0; i < length; i++) {
        sum += data[i];
    }
    return sum;
}

bool TFLuna::_sendCommand(uint8_t cmd, uint8_t *payload, uint8_t payloadLen) {
    if (_mode != TFLUNA_UART_MODE || _stream == NULL) {
        _errorCode = TFLUNA_ERROR_SERIAL;
        return false;
    }
    
    // Command format: [0x5A][Length][Cmd][Payload][Checksum]
    uint8_t length = payloadLen + 2; // Cmd + Payload
    uint8_t buffer[32]; // Max command length
    uint8_t idx = 0;
    
    buffer[idx++] = 0x5A; // Header
    buffer[idx++] = length; // Length
    buffer[idx++] = cmd; // Command
    
    // Add payload if any
    for (uint8_t i = 0; i < payloadLen; i++) {
        buffer[idx++] = payload[i];
    }
    
    // Calculate checksum
    buffer[idx] = _calculateChecksum(buffer, idx);
    idx++;
    
    // Send command
    _stream->write(buffer, idx);
    
    // Wait for response
    uint32_t startTime = millis();
    while (_stream->available() < 5) { // Minimum response length
        if (millis() - startTime > 500) { // 500ms timeout
            _errorCode = TFLUNA_ERROR_TIMEOUT;
            return false;
        }
    }
    
    // Read response header
    if (_stream->read() != 0x5A) {
        _errorCode = TFLUNA_ERROR_HEADER;
        return false;
    }
    
    // Read response length
    uint8_t respLength = _stream->read();
    
    // Read response command
    uint8_t respCmd = _stream->read();
    if (respCmd != cmd) {
        _errorCode = TFLUNA_ERROR_HEADER;
        return false;
    }
    
    // Read response status
    uint8_t status = _stream->read();
    
    // Read checksum
    uint8_t respChecksum = _stream->read();
    
    // Calculate expected checksum
    uint8_t expectedChecksum = 0x5A + respLength + respCmd + status;
    
    // Verify checksum
    if (respChecksum != expectedChecksum) {
        _errorCode = TFLUNA_ERROR_CHECKSUM;
        return false;
    }
    
    // Check status
    if (status != 0) {
        _errorCode = status;
        return false;
    }
    
    _errorCode = TFLUNA_OK;
    return true;
}

bool TFLuna::_writeRegister(uint8_t reg, uint8_t value, uint8_t addr) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value);
    if (Wire.endTransmission() != 0) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    _errorCode = TFLUNA_OK;
    return true;
}

bool TFLuna::_writeRegister16(uint8_t reg, uint16_t value, uint8_t addr) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(value & 0xFF);         // Low byte
    Wire.write((value >> 8) & 0xFF);  // High byte
    if (Wire.endTransmission() != 0) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    _errorCode = TFLUNA_OK;
    return true;
}

bool TFLuna::_readRegister(uint8_t reg, uint8_t &value, uint8_t addr) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    Wire.requestFrom(addr, (uint8_t)1);
    if (Wire.available() < 1) {
        _errorCode = TFLUNA_ERROR_I2C_DATA;
        return false;
    }
    
    value = Wire.read();
    
    _errorCode = TFLUNA_OK;
    return true;
}

bool TFLuna::_readRegister16(uint8_t reg, uint16_t &value, uint8_t addr) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0) {
        _errorCode = TFLUNA_ERROR_I2C_NACK;
        return false;
    }
    
    Wire.requestFrom(addr, (uint8_t)2);
    if (Wire.available() < 2) {
        _errorCode = TFLUNA_ERROR_I2C_DATA;
        return false;
    }
    
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    value = (high << 8) | low;
    
    _errorCode = TFLUNA_OK;
    return true;
}

bool TFLuna::_readData(uint8_t addr) {
    uint16_t dist, strength;
    int16_t temp;
    
    // Read distance
    if (!_readRegister16(TFLUNA_I2C_DIST_L, dist, addr)) {
        return false;
    }
    
    // Read signal strength
    if (!_readRegister16(TFLUNA_I2C_STRENGTH_L, strength, addr)) {
        return false;
    }
    
    // Read temperature
    if (!_readRegister16(TFLUNA_I2C_TEMP_L, temp, addr)) {
        return false;
    }
    
    _distance = dist;
    _strength = strength;
    _temperature = temp;
    
    _errorCode = TFLUNA_OK;
    return true;
}
