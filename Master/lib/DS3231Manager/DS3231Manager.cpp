#include "DS3231Manager.h"

void DS3231Manager::begin() {
    Wire.begin(); // SDA = GPIO21, SCL = GPIO22
}

bool DS3231Manager::isPowerLost() {
    Wire.beginTransmission(0x68);
    Wire.write(0x0F);                      // Status register
    Wire.endTransmission();
    Wire.requestFrom(0x68, 1);
    uint8_t status = Wire.read();
    return status & 0x80;                  // Oscillator Stop Flag (OSF)
}

bool DS3231Manager::getDateTime(DateTime& dt) {
    Wire.beginTransmission(0x68);
    Wire.write(0x00);                      // Start at register 0
    if (Wire.endTransmission() != 0) return false;

    Wire.requestFrom(0x68, 7);
    if (Wire.available() < 7) return false;

    dt.second  = bcdToDec(Wire.read());
    dt.minute  = bcdToDec(Wire.read());
    dt.hour    = bcdToDec(Wire.read());
    dt.weekday = bcdToDec(Wire.read());
    if (dt.weekday < 1 || dt.weekday > 7) dt.weekday = 1;
    dt.day     = bcdToDec(Wire.read());
    dt.month   = bcdToDec(Wire.read());
    dt.year    = 2000 + bcdToDec(Wire.read());

    return true;
}

bool DS3231Manager::setDateTime(const DateTime& dt) {
    if (!isValidDate(dt.day, dt.month, dt.year)) return false;

    Wire.beginTransmission(0x68);
    Wire.write(0x00);                     // Start at register 0
    Wire.write(decToBcd(0));              // Reset seconds to 0
    Wire.write(decToBcd(dt.minute));
    Wire.write(decToBcd(dt.hour));
    Wire.write(decToBcd(dt.weekday));     // 1=Dom, ..., 7=Sáb (Nextion lo usa así)
    Wire.write(decToBcd(dt.day));
    Wire.write(decToBcd(dt.month));
    Wire.write(decToBcd(dt.year - 2000));
    return Wire.endTransmission() == 0;
}

uint8_t DS3231Manager::decToBcd(uint8_t val) {
    return (val / 10 * 16) + (val % 10);
}

uint8_t DS3231Manager::bcdToDec(uint8_t val) {
    return (val / 16 * 10) + (val % 16);
}