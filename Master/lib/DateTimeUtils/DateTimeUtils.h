#pragma once

#include <Arduino.h>

struct DateTime {
    uint8_t day;
    uint8_t month;
    uint16_t year;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t weekday; // 1=Domingo, ..., 7=Sábado

    String toDateString() const {
        char buffer[11];
        snprintf(buffer, sizeof(buffer), "%02u/%02u/%04u", day, month, year);
        return String(buffer);
    }

    String toTimeString() const {
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "%02u:%02u", hour, minute);
        return String(buffer);
    }
};

bool isLeapYear(uint16_t year);
bool isValidDate(uint8_t d, uint8_t m, uint16_t y);

uint8_t calculateWeekday(uint8_t d, uint8_t m, uint16_t y);