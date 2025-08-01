#include "DateTimeUtils.h"

bool isLeapYear(uint16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool isValidDate(uint8_t d, uint8_t m, uint16_t y) {
    if (m < 1 || m > 12 || d < 1) return false;

    const uint8_t daysInMonth[] = {
        31, (uint8_t)(isLeapYear(y) ? 29 : 28), 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    return d <= daysInMonth[m - 1];
}

uint8_t calculateWeekday(uint8_t d, uint8_t m, uint16_t y) {
    // Zeller's Congruence (adaptada)
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    uint16_t K = y % 100;
    uint16_t J = y / 100;
    uint8_t h = (d + 13*(m + 1)/5 + K + K/4 + J/4 + 5*J) % 7;
    uint8_t weekday = ((h + 6) % 7) + 1;  // 1 = Domingo, ..., 7 = Sábado
    return weekday;
}