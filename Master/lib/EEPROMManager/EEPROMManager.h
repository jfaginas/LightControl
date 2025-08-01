#pragma once
#include <Wire.h>

class EEPROMManager {
public:
    explicit EEPROMManager(uint8_t deviceAddress);
    void begin();

    bool writeBytes(uint16_t memAddress, const uint8_t* data, size_t length);
    bool readBytes(uint16_t memAddress, uint8_t* buffer, size_t length);

private:
    uint8_t _deviceAddress;
};