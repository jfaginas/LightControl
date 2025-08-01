#pragma once

#include <Wire.h>
#include <DateTimeUtils.h>

class DS3231Manager {
public:
    void begin();
    bool isPowerLost();
    bool getDateTime(DateTime& dt);
    bool setDateTime(const DateTime& dt);

private:
    uint8_t decToBcd(uint8_t val);
    uint8_t bcdToDec(uint8_t val);
};