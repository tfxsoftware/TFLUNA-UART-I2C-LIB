#include <Arduino.h>
#include <unity.h>
#include <TFLuna.h>
#include <TFLunaAdvanced.h>

// Mock classes for testing
class MockStream : public Stream {
public:
    MockStream() {
        _available = 0;
        _readIndex = 0;
    }
    
    void setData(uint8_t* data, size_t length) {
        memcpy(_buffer, data, length);
        _available = length;
        _readIndex = 0;
    }
    
    int available() override {
        return _available - _readIndex;
    }
    
    int read() override {
        if (_readIndex < _available) {
            return _buffer[_readIndex++];
        }
        return -1;
    }
    
    int peek() override {
        if (_readIndex < _available) {
            return _buffer[_readIndex];
        }
        return -1;
    }
    
    size_t write(uint8_t data) override {
        return 1;
    }
    
    size_t write(const uint8_t* buffer, size_t size) override {
        return size;
    }
    
private:
    uint8_t _buffer[256];
    size_t _available;
    size_t _readIndex;
};

// Global variables for tests
MockStream mockStream;
TFLuna tfLuna(&mockStream);
TFLunaAdvanced tfLunaAdvanced(&mockStream);

// Test functions
void test_constructor() {
    TEST_ASSERT_EQUAL(0, tfLuna.getDistance());
    TEST_ASSERT_EQUAL(0, tfLuna.getSignalStrength());
    TEST_ASSERT_EQUAL(0, tfLuna.getTemperature());
    TEST_ASSERT_EQUAL(0, tfLuna.getErrorCode());
}

void test_uart_data_parsing() {
    // Create a valid UART data frame
    uint8_t validFrame[] = {
        0x59, 0x59,             // Header
        0x64, 0x00,             // Distance: 100 cm
        0xE8, 0x03,             // Strength: 1000
        0x2C, 0x01,             // Temperature: 300 (3.00°C)
        0x2D                    // Checksum
    };
    
    // Set the mock data
    mockStream.setData(validFrame, sizeof(validFrame));
    
    // Test data parsing
    TEST_ASSERT_TRUE(tfLuna.getData());
    TEST_ASSERT_EQUAL(100, tfLuna.getDistance());
    TEST_ASSERT_EQUAL(1000, tfLuna.getSignalStrength());
    TEST_ASSERT_EQUAL(300, tfLuna.getTemperature());
    TEST_ASSERT_EQUAL(0, tfLuna.getErrorCode());
}

void test_uart_invalid_checksum() {
    // Create a frame with invalid checksum
    uint8_t invalidFrame[] = {
        0x59, 0x59,             // Header
        0x64, 0x00,             // Distance: 100 cm
        0xE8, 0x03,             // Strength: 1000
        0x2C, 0x01,             // Temperature: 300 (3.00°C)
        0xFF                    // Invalid checksum
    };
    
    // Set the mock data
    mockStream.setData(invalidFrame, sizeof(invalidFrame));
    
    // Test data parsing with invalid checksum
    TEST_ASSERT_FALSE(tfLuna.getData());
    TEST_ASSERT_EQUAL(2, tfLuna.getErrorCode()); // TFLUNA_ERROR_CHECKSUM
}

void test_advanced_filters() {
    // Enable median filter
    tfLunaAdvanced.enableMedianFilter(3);
    
    // Create three frames with different distances
    uint8_t frame1[] = {
        0x59, 0x59,             // Header
        0x64, 0x00,             // Distance: 100 cm
        0xE8, 0x03,             // Strength: 1000
        0x2C, 0x01,             // Temperature: 300 (3.00°C)
        0x2D                    // Checksum
    };
    
    uint8_t frame2[] = {
        0x59, 0x59,             // Header
        0xC8, 0x00,             // Distance: 200 cm
        0xE8, 0x03,             // Strength: 1000
        0x2C, 0x01,             // Temperature: 300 (3.00°C)
        0x91                    // Checksum
    };
    
    uint8_t frame3[] = {
        0x59, 0x59,             // Header
        0x2C, 0x01,             // Distance: 300 cm
        0xE8, 0x03,             // Strength: 1000
        0x2C, 0x01,             // Temperature: 300 (3.00°C)
        0xF5                    // Checksum
    };
    
    // Send first frame
    mockStream.setData(frame1, sizeof(frame1));
    tfLunaAdvanced.getData();
    
    // Send second frame
    mockStream.setData(frame2, sizeof(frame2));
    tfLunaAdvanced.getData();
    
    // Send third frame
    mockStream.setData(frame3, sizeof(frame3));
    tfLunaAdvanced.getData();
    
    // The median of 100, 200, 300 should be 200
    TEST_ASSERT_EQUAL(200, tfLunaAdvanced.getDistance());
    
    // Test unit conversions
    TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, tfLunaAdvanced.getDistanceInMeters());
    TEST_ASSERT_FLOAT_WITHIN(0.1, 78.74, tfLunaAdvanced.getDistanceInInches());
    
    // Test temperature conversions
    TEST_ASSERT_FLOAT_WITHIN(0.01, 3.0, tfLunaAdvanced.getTemperatureInCelsius());
    TEST_ASSERT_FLOAT_WITHIN(0.01, 37.4, tfLunaAdvanced.getTemperatureInFahrenheit());
}

void setup() {
    delay(2000);  // Give the serial monitor time to open
    
    UNITY_BEGIN();
    
    RUN_TEST(test_constructor);
    RUN_TEST(test_uart_data_parsing);
    RUN_TEST(test_uart_invalid_checksum);
    RUN_TEST(test_advanced_filters);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
