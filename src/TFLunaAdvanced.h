#ifndef TFLUNA_ADVANCED_H
#define TFLUNA_ADVANCED_H

#include "TFLuna.h"

// Advanced features for TF-Luna LiDAR sensor
class TFLunaAdvanced : public TFLuna {
public:
    // Inherit constructors
    using TFLuna::TFLuna;
    
    // Distance filtering methods
    void enableMedianFilter(uint8_t windowSize = 5);
    void disableMedianFilter();
    void enableAverageFilter(uint8_t windowSize = 5);
    void disableAverageFilter();
    
    // Overridden data acquisition methods to apply filters
    bool getData() override;
    bool getDataI2C(uint8_t addr = TFLUNA_DEFAULT_I2C_ADDR) override;
    
    // Distance conversion methods
    float getDistanceInMeters() const;
    float getDistanceInInches() const;
    
    // Temperature conversion methods
    float getTemperatureInCelsius() const;
    float getTemperatureInFahrenheit() const;
    
    // Signal quality assessment
    bool isSignalReliable() const;
    uint8_t getSignalQuality() const; // 0-100%
    
    // Data logging
    void beginLogging(Stream* logStream);
    void endLogging();
    
    // Distance thresholds and callbacks
    typedef void (*DistanceCallback)(uint16_t distance);
    void setMinDistanceThreshold(uint16_t threshold, DistanceCallback callback = nullptr);
    void setMaxDistanceThreshold(uint16_t threshold, DistanceCallback callback = nullptr);
    void clearDistanceThresholds();
    
    // Check thresholds after data acquisition
    bool checkThresholds();

private:
    // Filter settings
    bool _medianFilterEnabled = false;
    bool _averageFilterEnabled = false;
    uint8_t _medianWindowSize = 5;
    uint8_t _averageWindowSize = 5;
    
    // Filter buffers
    uint16_t* _distanceBuffer = nullptr;
    uint8_t _bufferIndex = 0;
    uint8_t _bufferSize = 0;
    bool _bufferFilled = false;
    
    // Apply filters to raw distance
    uint16_t _applyFilters(uint16_t rawDistance);
    uint16_t _calculateMedian();
    uint16_t _calculateAverage();
    
    // Logging
    Stream* _logStream = nullptr;
    bool _loggingEnabled = false;
    
    // Threshold settings
    uint16_t _minThreshold = 0;
    uint16_t _maxThreshold = 0;
    DistanceCallback _minCallback = nullptr;
    DistanceCallback _maxCallback = nullptr;
    bool _thresholdsEnabled = false;
};

#endif // TFLUNA_ADVANCED_H
