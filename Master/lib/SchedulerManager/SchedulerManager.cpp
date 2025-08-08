#include "SchedulerManager.h"

#define EEPROM_SCHEDULE_SIZE sizeof(WeeklySchedule)

static bool lastSentStateSlot1 = false;
static bool lastSentStateSlot2 = false;

SchedulerManager::SchedulerManager() : ledState(false) {}

void SchedulerManager::begin() {
    pinMode(SCHEDULER_LED_PIN, OUTPUT);    // GPIO02 en Master
    pinMode(SCHEDULER_LED2_PIN, OUTPUT);   // GPIO25 en Master
    loadFromEEPROM();
    loadFromEEPROM2();
}

void SchedulerManager::update(const DateTime& now, bool enableFlag) {
    applySchedule(now, enableFlag);
    digitalWrite(SCHEDULER_LED_PIN, ledState ? HIGH : LOW);
    if (ledState != lastSentStateSlot1) {
        lastSentStateSlot1 = ledState;
        MasterLogic::sendSlot1ToSlaves(ledState);
    }
}

void SchedulerManager::updateSecondary(const DateTime& now, bool enableFlag) {
    applySchedule2(now, enableFlag);
    digitalWrite(SCHEDULER_LED2_PIN, led2State ? HIGH : LOW);
    if (led2State != lastSentStateSlot2) {
        lastSentStateSlot2 = led2State;
        MasterLogic::sendSlot2ToSlaves(led2State);
    }
}

bool SchedulerManager::getSlot1State() const {
    return lastSentStateSlot1;  // o la variable correspondiente
}

bool SchedulerManager::getSlot2State() const {
    return lastSentStateSlot2;  // o la variable correspondiente
}

// Devuelve minutos transcurridos desde el inicio de la semana (lunes 00:00 = 0)
static uint16_t timeInWeek(uint8_t day, const TimePoint& time) {
    return day * 1440 + time.hour * 60 + time.minute;
}

void SchedulerManager::applySchedule(const DateTime& now, bool enableFlag) {
    if (!enableFlag) {
        ledState = false;
        return;
    }

    uint8_t indexToday = (now.weekday + 5) % 7;
    uint8_t indexYesterday = (indexToday + 6) % 7;  // día anterior

    uint16_t currentMinutes = timeInWeek(indexToday, {now.hour, now.minute});

    bool isOn = false;

    for (int s = 0; s < 2; ++s) {
        const ScheduleSlot& slotToday = schedule.days[indexToday].slots[s];
        const ScheduleSlot& slotYesterday = schedule.days[indexYesterday].slots[s];

        if (slotToday.enabled && isTimeInSlot(currentMinutes, slotToday, indexToday)) {
            isOn = true;
        }

        if (slotYesterday.enabled && isTimeInSlot(currentMinutes, slotYesterday, indexYesterday)) {
            isOn = true;
        }
    }

    ledState = isOn;
}

void SchedulerManager::applySchedule2(const DateTime& now, bool enableFlag) {
    if (!enableFlag) {
        led2State = false;
        return;
    }

    uint8_t indexToday = (now.weekday + 5) % 7;
    uint8_t indexYesterday = (indexToday + 6) % 7;

    uint16_t currentMinutes = timeInWeek(indexToday, {now.hour, now.minute});

    bool isOn = false;

    for (int s = 0; s < 2; ++s) {
        const ScheduleSlot& slotToday = schedule2.days[indexToday].slots[s];
        const ScheduleSlot& slotYesterday = schedule2.days[indexYesterday].slots[s];

        if (slotToday.enabled && isTimeInSlot(currentMinutes, slotToday, indexToday)) {
            isOn = true;
        }

        if (slotYesterday.enabled && isTimeInSlot(currentMinutes, slotYesterday, indexYesterday)) {
            isOn = true;
        }
    }

    led2State = isOn;
}

bool SchedulerManager::isTimeInSlot(uint16_t currentMin, const ScheduleSlot& slot, uint8_t dayIndex) {
    uint16_t onMin = timeInWeek(dayIndex, slot.onTime);
    uint16_t offMin = timeInWeek(dayIndex, slot.offTime);

    // Cruza medianoche
    if (offMin <= onMin) {
        offMin += 1440; // agregar un día
        if (currentMin < onMin) currentMin += 10080; // ajustar a la misma "semana extendida"
    }

    return currentMin >= onMin && currentMin < offMin;
}

void SchedulerManager::setSchedule(uint8_t day, uint8_t slot, const TimePoint& on, const TimePoint& off, bool enabled) {
    if (day > 6 || slot > 1) return;
    schedule.days[day].slots[slot] = { on, off, (uint8_t)enabled };
    saveToEEPROM();
}

const WeeklySchedule& SchedulerManager::getSchedule() const {
    return schedule;
}

void SchedulerManager::loadFromEEPROM() {
    EEPROMManager eeprom(EEPROM_ADDRESS);
    eeprom.begin();
    eeprom.readBytes(EEPROM_SCHEDULE_ADDR, (uint8_t*)&schedule, EEPROM_SCHEDULE_SIZE);
    for (int d = 0; d < 7; ++d) {
        for (int s = 0; s < 2; ++s) {
            const auto& slot = schedule.days[d].slots[s];
        }
    }
}

void SchedulerManager::saveToEEPROM() {
    EEPROMManager eeprom(EEPROM_ADDRESS);
    eeprom.begin();
    eeprom.writeBytes(EEPROM_SCHEDULE_ADDR, (uint8_t*)&schedule, EEPROM_SCHEDULE_SIZE);
}

void SchedulerManager::loadFromEEPROM2() {
    EEPROMManager eeprom(EEPROM_ADDRESS);
    eeprom.begin();
    eeprom.readBytes(EEPROM_SCHEDULE2_ADDR, (uint8_t*)&schedule2, sizeof(WeeklySchedule));

    for (int d = 0; d < 7; ++d) {
        for (int s = 0; s < 2; ++s) {
            const auto& slot = schedule2.days[d].slots[s];
        }
    }
}

void SchedulerManager::saveToEEPROM2() {
    EEPROMManager eeprom(EEPROM_ADDRESS);
    eeprom.begin();
    eeprom.writeBytes(EEPROM_SCHEDULE2_ADDR, (uint8_t*)&schedule2, sizeof(WeeklySchedule));
}

void SchedulerManager::handleSchedulerCommand(const String& cmd) {
    int values[7];
    int count = 0;

    char buffer[cmd.length() + 1];
    cmd.toCharArray(buffer, sizeof(buffer));
    char* token = strtok(buffer, ",");
  
    while (token && count < 7) {
        values[count++] = atoi(token);
        token = strtok(nullptr, ",");
    }

    if (count == 7) {
        uint8_t day = values[0];
        uint8_t slot = values[1];
        TimePoint on = { (uint8_t)values[2], (uint8_t)values[3] };
        TimePoint off = { (uint8_t)values[4], (uint8_t)values[5] };
        bool enabled = values[6];
        setSchedule(day, slot, on, off, enabled);
    } else {
        Serial.println("[Scheduler] Error de formato en comando SCHED=");
    }
}

void SchedulerManager::handleSchedulerCommand2(const String& cmd) {
    int values[7];
    int count = 0;

    char buffer[cmd.length() + 1];
    cmd.toCharArray(buffer, sizeof(buffer));
    char* token = strtok(buffer, ",");
    
    while (token && count < 7) {
        values[count++] = atoi(token);
        token = strtok(nullptr, ",");
    }

    if (count == 7) {
        uint8_t day = values[0];
        uint8_t slot = values[1];
        TimePoint on = { (uint8_t)values[2], (uint8_t)values[3] };
        TimePoint off = { (uint8_t)values[4], (uint8_t)values[5] };
        bool enabled = values[6];

        if (day > 6 || slot > 1) return;
        schedule2.days[day].slots[slot] = { on, off, (uint8_t)enabled };
        saveToEEPROM2();
    } else {
        Serial.println("[Scheduler2] Error de formato en comando SCHED2=");
    }
}

void SchedulerManager::clearSchedule() {
    for (auto& day : schedule.days) {
        for (auto& slot : day.slots) {
            slot.onTime = {0, 0};
            slot.offTime = {0, 0};
            slot.enabled = false;
        }
    }
    saveToEEPROM();
}

void SchedulerManager::clear2Schedule() {
    for (auto& day : schedule2.days) {
        for (auto& slot : day.slots) {
            slot.onTime = {0, 0};
            slot.offTime = {0, 0};
            slot.enabled = false;
        }
    }
    saveToEEPROM2();
}

void SchedulerManager::showScheduleForDay(uint8_t day, NextionManager& display) {

    if (day > 6) return;

    for (int s = 0; s < 2; ++s) {
        const ScheduleSlot& slot = schedule.days[day].slots[s];

        char buf[32];
        if (slot.enabled) {
            snprintf(buf, sizeof(buf), "%02u:%02u - %02u:%02u", 
                     slot.onTime.hour, slot.onTime.minute,
                     slot.offTime.hour, slot.offTime.minute);
        } else {
            snprintf(buf, sizeof(buf), "Vacio");
        }

        String cmd = "slot" + String(s) + ".txt=\"" + String(buf) + "\"";
        display.sendCommand(cmd);  // envía comando al Nextion
    }
}

void SchedulerManager::showSchedule2ForDay(uint8_t day, NextionManager& display) {
    if (day > 6) return;

    for (int s = 0; s < 2; ++s) {
        const ScheduleSlot& slot = schedule2.days[day].slots[s];

        char buf[32];
        if (slot.enabled) {
            snprintf(buf, sizeof(buf), "%02u:%02u - %02u:%02u", 
                     slot.onTime.hour, slot.onTime.minute,
                     slot.offTime.hour, slot.offTime.minute);
        } else {
            snprintf(buf, sizeof(buf), "Vacio");
        }

        String cmd = "slot" + String(s) + ".txt=\"" + String(buf) + "\"";
        display.sendCommand(cmd);  // reusa mismo formato visual
    }
}