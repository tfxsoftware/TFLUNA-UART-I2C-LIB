#include "TFLunaAdvanced.h"

// Distance filtering methods
void TFLunaAdvanced::enableMedianFilter(uint8_t windowSize) {
    // Ensure window size is odd for proper median calculation
    if (windowSize % 2 == 0) {
        windowSize++;
    }
    
    // Limit window size to reasonable values
    if (windowSize < 3) {
        windowSize = 3;
    } else if (windowSize > 15) {
        windowSize = 15;
    }
    
    _medianWindowSize = windowSize;
    _medianFilterEnabled = true;
    
    // Allocate or reallocate buffer if needed
    uint8_t requiredSize = _medianFilterEnabled ? _medianWindowSize : 0;
    requiredSize = _averageFilterEnabled ? max(requiredSize, _averageWindowSize) : requiredSize;
    
    if (requiredSize > _bufferSize) {
        if (_distanceBuffer != nullptr) {
            delete[] _distanceBuffer;
        }
        
        _distanceBuffer = new uint16_t[requiredSize];
        _bufferSize = requiredSize;
        _bufferIndex = 0;
        _bufferFilled = false;
        
        // Initialize buffer with zeros
        for (uint8_t i = 0; i < _bufferSize; i++) {
            _distanceBuffer[i] = 0;
        }
    }
}

void TFLunaAdvanced::disableMedianFilter() {
    _medianFilterEnabled = false;
    
    // Free buffer if no filters are enabled
    if (!_averageFilterEnabled && _distanceBuffer != nullptr) {
        delete[] _distanceBuffer;
        _distanceBuffer = nullptr;
        _bufferSize = 0;
    }
}

void TFLunaAdvanced::enableAverageFilter(uint8_t windowSize) {
    // Limit window size to reasonable values
    if (windowSize < 2) {
        windowSize = 2;
    } else if (windowSize > 20) {
        windowSize = 20;
    }
    
    _averageWindowSize = windowSize;
    _averageFilterEnabled = true;
    
    // Allocate or reallocate buffer if needed
    uint8_t requiredSize = _medianFilterEnabled ? _medianWindowSize : 0;
    requiredSize = _averageFilterEnabled ? max(requiredSize, _averageWindowSize) : requiredSize;
    
    if (requiredSize > _bufferSize) {
        if (_distanceBuffer != nullptr) {
            delete[] _distanceBuffer;
        }
        
        _distanceBuffer = new uint16_t[requiredSize];
        _bufferSize = requiredSize;
        _bufferIndex = 0;
        _bufferFilled = false;
        
        // Initialize buffer with zeros
        for (uint8_t i = 0; i < _bufferSize; i++) {
            _distanceBuffer[i] = 0;
        }
    }
}

void TFLunaAdvanced::disableAverageFilter() {
    _averageFilterEnabled = false;
    
    // Free buffer if no filters are enabled
    if (!_medianFilterEnabled && _distanceBuffer != nullptr) {
        delete[] _distanceBuffer;
        _distanceBuffer = nullptr;
        _bufferSize = 0;
    }
}

// Overridden data acquisition methods to apply filters
bool TFLunaAdvanced::getData() {
    bool result = TFLuna::getData();
    
    if (result && (_medianFilterEnabled || _averageFilterEnabled)) {
        uint16_t rawDistance = TFLuna::getDistance();
        uint16_t filteredDistance = _applyFilters(rawDistance);
        
        // Override the distance with filtered value
        _distance = filteredDistance;
    }
    
    // Log data if logging is enabled
    if (result && _loggingEnabled && _logStream != nullptr) {
        _logStream->print("Distance: ");
        _logStream->print(_distance);
        _logStream->print(" cm, Strength: ");
        _logStream->print(_strength);
        _logStream->print(", Temp: ");
        _logStream->print(_temperature / 100.0);
        _logStream->println(" °C");
    }
    
    // Check thresholds if enabled
    if (result && _thresholdsEnabled) {
        checkThresholds();
    }
    
    return result;
}

bool TFLunaAdvanced::getDataI2C(uint8_t addr) {
    bool result = TFLuna::getDataI2C(addr);
    
    if (result && (_medianFilterEnabled || _averageFilterEnabled)) {
        uint16_t rawDistance = TFLuna::getDistance();
        uint16_t filteredDistance = _applyFilters(rawDistance);
        
        // Override the distance with filtered value
        _distance = filteredDistance;
    }
    
    // Log data if logging is enabled
    if (result && _loggingEnabled && _logStream != nullptr) {
        _logStream->print("Distance: ");
        _logStream->print(_distance);
        _logStream->print(" cm, Strength: ");
        _logStream->print(_strength);
        _logStream->print(", Temp: ");
        _logStream->print(_temperature / 100.0);
        _logStream->println(" °C");
    }
    
    // Check thresholds if enabled
    if (result && _thresholdsEnabled) {
        checkThresholds();
    }
    
    return result;
}

// Distance conversion methods
float TFLunaAdvanced::getDistanceInMeters() const {
    return _distance / 100.0f;
}

float TFLunaAdvanced::getDistanceInInches() const {
    return _distance / 2.54f;
}

// Temperature conversion methods
float TFLunaAdvanced::getTemperatureInCelsius() const {
    return _temperature / 100.0f;
}

float TFLunaAdvanced::getTemperatureInFahrenheit() const {
    return (_temperature / 100.0f * 9.0f / 5.0f) + 32.0f;
}

// Signal quality assessment
bool TFLunaAdvanced::isSignalReliable() const {
    // Signal is considered reliable if strength is above 100
    // and distance is within the valid range (0.2m to 8m)
    return (_strength > 100 && _distance >= 20 && _distance <= 800);
}

uint8_t TFLunaAdvanced::getSignalQuality() const {
    // Convert signal strength to a quality percentage
    // Signal strength below 30 is considered poor quality (0%)
    // Signal strength above 2000 is considered excellent quality (100%)
    if (_strength < 30) {
        return 0;
    } else if (_strength > 2000) {
        return 100;
    } else {
        // Linear mapping from 30-2000 to 0-100%
        return (uint8_t)((_strength - 30) * 100 / 1970);
    }
}

// Data logging
void TFLunaAdvanced::beginLogging(Stream* logStream) {
    _logStream = logStream;
    _loggingEnabled = true;
}

void TFLunaAdvanced::endLogging() {
    _loggingEnabled = false;
}

// Distance thresholds and callbacks
void TFLunaAdvanced::setMinDistanceThreshold(uint16_t threshold, DistanceCallback callback) {
    _minThreshold = threshold;
    _minCallback = callback;
    _thresholdsEnabled = true;
}

void TFLunaAdvanced::setMaxDistanceThreshold(uint16_t threshold, DistanceCallback callback) {
    _maxThreshold = threshold;
    _maxCallback = callback;
    _thresholdsEnabled = true;
}

void TFLunaAdvanced::clearDistanceThresholds() {
    _minThreshold = 0;
    _maxThreshold = 0;
    _minCallback = nullptr;
    _maxCallback = nullptr;
    _thresholdsEnabled = false;
}

bool TFLunaAdvanced::checkThresholds() {
    bool thresholdTriggered = false;
    
    // Check minimum threshold
    if (_minThreshold > 0 && _distance <= _minThreshold) {
        if (_minCallback != nullptr) {
            _minCallback(_distance);
        }
        thresholdTriggered = true;
    }
    
    // Check maximum threshold
    if (_maxThreshold > 0 && _distance >= _maxThreshold) {
        if (_maxCallback != nullptr) {
            _maxCallback(_distance);
        }
        thresholdTriggered = true;
    }
    
    return thresholdTriggered;
}

// Private helper methods
uint16_t TFLunaAdvanced::_applyFilters(uint16_t rawDistance) {
    // Add the new distance to the buffer
    if (_distanceBuffer != nullptr && _bufferSize > 0) {
        _distanceBuffer[_bufferIndex] = rawDistance;
        _bufferIndex = (_bufferIndex + 1) % _bufferSize;
        
        // Check if buffer is filled
        if (!_bufferFilled && _bufferIndex == 0) {
            _bufferFilled = true;
        }
    }
    
    // Apply filters
    uint16_t filteredDistance = rawDistance;
    
    if (_medianFilterEnabled && _bufferFilled) {
        filteredDistance = _calculateMedian();
    }
    
    if (_averageFilterEnabled && _bufferFilled) {
        // If both filters are enabled, use median as input to average
        if (_medianFilterEnabled) {
            // We already calculated median, just use it
        } else {
            filteredDistance = _calculateAverage();
        }
    }
    
    return filteredDistance;
}

uint16_t TFLunaAdvanced::_calculateMedian() {
    if (_distanceBuffer == nullptr || _bufferSize < _medianWindowSize) {
        return 0;
    }
    
    // Copy values to a temporary array for sorting
    uint16_t tempArray[_medianWindowSize];
    
    // Fill the temporary array with the most recent values
    for (uint8_t i = 0; i < _medianWindowSize; i++) {
        uint8_t idx = (_bufferIndex - 1 - i + _bufferSize) % _bufferSize;
        tempArray[i] = _distanceBuffer[idx];
    }
    
    // Simple bubble sort
    for (uint8_t i = 0; i < _medianWindowSize - 1; i++) {
        for (uint8_t j = 0; j < _medianWindowSize - i - 1; j++) {
            if (tempArray[j] > tempArray[j + 1]) {
                // Swap
                uint16_t temp = tempArray[j];
                tempArray[j] = tempArray[j + 1];
                tempArray[j + 1] = temp;
            }
        }
    }
    
    // Return the middle value
    return tempArray[_medianWindowSize / 2];
}

uint16_t TFLunaAdvanced::_calculateAverage() {
    if (_distanceBuffer == nullptr || _bufferSize < _averageWindowSize) {
        return 0;
    }
    
    uint32_t sum = 0;
    
    // Sum the most recent values
    for (uint8_t i = 0; i < _averageWindowSize; i++) {
        uint8_t idx = (_bufferIndex - 1 - i + _bufferSize) % _bufferSize;
        sum += _distanceBuffer[idx];
    }
    
    // Calculate average
    return (uint16_t)(sum / _averageWindowSize);
}
