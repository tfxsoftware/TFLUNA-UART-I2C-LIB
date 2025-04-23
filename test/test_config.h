// Test configuration file for TFLuna library
#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

// Uncomment to enable real hardware tests
// Note: These tests require actual TF-Luna hardware connected
//#define ENABLE_HARDWARE_TESTS

// Serial port for hardware tests (if enabled)
#define HARDWARE_SERIAL Serial1

// I2C address for hardware tests (if enabled)
#define HARDWARE_I2C_ADDR 0x10

#endif // TEST_CONFIG_H
