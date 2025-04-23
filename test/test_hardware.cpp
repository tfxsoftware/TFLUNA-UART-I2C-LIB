#include <Arduino.h>
#include <unity.h>
#include "test_config.h"
#include <TFLuna.h>
#include <TFLunaAdvanced.h>

#ifdef ENABLE_HARDWARE_TESTS
// Hardware test variables
TFLuna hwTfLuna(&HARDWARE_SERIAL);
TFLunaAdvanced hwTfLunaAdvanced(&HARDWARE_SERIAL);

// Hardware test functions
void test_hardware_uart_init() {
    TEST_ASSERT_TRUE(hwTfLuna.begin(115200));
    delay(100);
}

void test_hardware_uart_data() {
    TEST_ASSERT_TRUE(hwTfLuna.getData());
    
    // Print the data for manual verification
    Serial.print("Distance: ");
    Serial.print(hwTfLuna.getDistance());
    Serial.println(" cm");
    
    Serial.print("Signal Strength: ");
    Serial.println(hwTfLuna.getSignalStrength());
    
    Serial.print("Temperature: ");
    Serial.print(hwTfLuna.getTemperature() / 100.0);
    Serial.println(" °C");
    
    // Basic sanity checks
    TEST_ASSERT_LESS_OR_EQUAL(800, hwTfLuna.getDistance()); // Max range is 8m
    TEST_ASSERT_GREATER_OR_EQUAL(0, hwTfLuna.getDistance());
    TEST_ASSERT_GREATER_OR_EQUAL(0, hwTfLuna.getSignalStrength());
}

void test_hardware_i2c_init() {
    TFLuna i2cTfLuna;
    TEST_ASSERT_TRUE(i2cTfLuna.beginI2C());
    delay(100);
}

void test_hardware_i2c_data() {
    TFLuna i2cTfLuna;
    i2cTfLuna.beginI2C();
    
    TEST_ASSERT_TRUE(i2cTfLuna.getDataI2C(HARDWARE_I2C_ADDR));
    
    // Print the data for manual verification
    Serial.print("I2C Distance: ");
    Serial.print(i2cTfLuna.getDistance());
    Serial.println(" cm");
    
    // Basic sanity checks
    TEST_ASSERT_LESS_OR_EQUAL(800, i2cTfLuna.getDistance()); // Max range is 8m
    TEST_ASSERT_GREATER_OR_EQUAL(0, i2cTfLuna.getDistance());
}

void test_hardware_advanced_features() {
    hwTfLunaAdvanced.begin(115200);
    hwTfLunaAdvanced.enableMedianFilter(5);
    
    TEST_ASSERT_TRUE(hwTfLunaAdvanced.getData());
    
    // Print the data for manual verification
    Serial.print("Filtered Distance: ");
    Serial.print(hwTfLunaAdvanced.getDistance());
    Serial.println(" cm");
    
    Serial.print("Distance in meters: ");
    Serial.print(hwTfLunaAdvanced.getDistanceInMeters());
    Serial.println(" m");
    
    Serial.print("Signal Quality: ");
    Serial.print(hwTfLunaAdvanced.getSignalQuality());
    Serial.println("%");
    
    // Basic sanity checks
    TEST_ASSERT_LESS_OR_EQUAL(800, hwTfLunaAdvanced.getDistance()); // Max range is 8m
    TEST_ASSERT_GREATER_OR_EQUAL(0, hwTfLunaAdvanced.getDistance());
}
#endif

void setup() {
    delay(2000);  // Give the serial monitor time to open
    Serial.begin(115200);
    
    UNITY_BEGIN();
    
#ifdef ENABLE_HARDWARE_TESTS
    RUN_TEST(test_hardware_uart_init);
    RUN_TEST(test_hardware_uart_data);
    
    // Only run I2C tests if pin 5 is connected to GND
    // Uncomment these if the sensor is in I2C mode
    //RUN_TEST(test_hardware_i2c_init);
    //RUN_TEST(test_hardware_i2c_data);
    
    RUN_TEST(test_hardware_advanced_features);
#else
    Serial.println("Hardware tests disabled. Edit test_config.h to enable them.");
#endif
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
