#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "EEPROMManager.h"
#include "DateTimeUtils.h"
#include "NextionManager.h"
#include "MasterLogic.h"

#define SCHEDULER_LED_PIN 2      // Led que representa desinfección en master
#define SCHEDULER_LED2_PIN 25    // Led que representa purificación en master
#define EEPROM_ADDRESS 0x57
#define EEPROM_SCHEDULE_ADDR 0x0000
#define EEPROM_SCHEDULE2_ADDR (EEPROM_SCHEDULE_ADDR + sizeof(WeeklySchedule))

struct __attribute__((packed)) TimePoint {
    uint8_t hour;
    uint8_t minute;
};

inline bool operator==(const TimePoint& a, const TimePoint& b) {
    return a.hour == b.hour && a.minute == b.minute;
}

inline bool operator<(const TimePoint& a, const TimePoint& b) {
    return (a.hour < b.hour) || (a.hour == b.hour && a.minute < b.minute);
}

inline bool operator!=(const TimePoint& a, const TimePoint& b) { return !(a == b); }
inline bool operator>(const TimePoint& a, const TimePoint& b) { return b < a; }
inline bool operator<=(const TimePoint& a, const TimePoint& b) { return !(b < a); }
inline bool operator>=(const TimePoint& a, const TimePoint& b) { return !(a < b); }

struct __attribute__((packed)) ScheduleSlot {
    TimePoint onTime;
    TimePoint offTime;
    uint8_t enabled;  // bool de 8 bits
};

struct __attribute__((packed)) DailySchedule {
    ScheduleSlot slots[2];
};

struct __attribute__((packed)) WeeklySchedule {
    DailySchedule days[7];  // 0=Lunes ... 6=Domingo
};

class SchedulerManager {
public:
    SchedulerManager();
    void begin();
    void update(const DateTime& now, bool enableFlag);
    void updateSecondary(const DateTime& now, bool enableFlag);
    void setSchedule(uint8_t day, uint8_t slot, const TimePoint& on, const TimePoint& off, bool enabled);

    const WeeklySchedule& getSchedule() const;

    void handleSchedulerCommand(const String& cmd);                 // Procesa SCHED=
    void handleSchedulerCommand2(const String& cmd);                // Procesa SCHED2=
    void clearSchedule();
    void clear2Schedule(); //
    void showScheduleForDay(uint8_t day, NextionManager& display);
    void showSchedule2ForDay(uint8_t day, NextionManager& display); // Visualización en Nextion
    bool getSlot1State() const;
    bool getSlot2State() const;

private:
    WeeklySchedule schedule;
    bool ledState;
    WeeklySchedule schedule2;
    bool led2State;

    void applySchedule(const DateTime& now, bool enableFlag);
    void applySchedule2(const DateTime& now, bool enableFlag);
    bool isWithinInterval(const TimePoint& now, const TimePoint& on, const TimePoint& off);

    void loadFromEEPROM();
    void loadFromEEPROM2();
    void saveToEEPROM();
    void saveToEEPROM2();
};
