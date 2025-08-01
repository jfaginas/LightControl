#include "EEPROMManager.h"
#include <Arduino.h>

EEPROMManager::EEPROMManager(uint8_t deviceAddress)
    : _deviceAddress(deviceAddress) {}

void EEPROMManager::begin() {
    Wire.begin();  // SDA = GPIO21, SCL = GPIO22
}

bool EEPROMManager::writeBytes(uint16_t memAddress, const uint8_t* data, size_t length) {
    const size_t PAGE_SIZE = 32;
    size_t bytesWritten = 0;
    
    while (bytesWritten < length) {
        size_t chunkSize = min(PAGE_SIZE - (memAddress % PAGE_SIZE), length - bytesWritten);

        Wire.beginTransmission(_deviceAddress);
        Wire.write((uint8_t)(memAddress >> 8));    // MSB
        Wire.write((uint8_t)(memAddress & 0xFF));  // LSB

        Wire.write(data + bytesWritten, chunkSize);
        if (Wire.endTransmission() != 0) return false;

        delay(5);  // Esperar tiempo de escritura interna típico de EEPROM
        memAddress += chunkSize;
        bytesWritten += chunkSize;
    }
    return true;
}

bool EEPROMManager::readBytes(uint16_t memAddress, uint8_t* buffer, size_t length) {
    Wire.beginTransmission(_deviceAddress);
    Wire.write((uint8_t)(memAddress >> 8));    // MSB
    Wire.write((uint8_t)(memAddress & 0xFF));  // LSB
    if (Wire.endTransmission(false) != 0) return false;  // Repetir start

    size_t bytesRead = 0;
    while (bytesRead < length) {
        size_t toRead = min((size_t)32, length - bytesRead);
        if (Wire.requestFrom((int)_deviceAddress, (int)toRead) != (int)toRead) {
            return false;
        }

        for (size_t i = 0; i < toRead; ++i) {
            if (Wire.available()) {
                buffer[bytesRead++] = Wire.read();
            }
        }
    }
    return true;
}