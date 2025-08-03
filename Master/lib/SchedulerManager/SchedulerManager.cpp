#include "SchedulerManager.h"

#define EEPROM_SCHEDULE_SIZE sizeof(WeeklySchedule)

static bool lastSentStateSlot1 = false;
static bool lastSentStateSlot2 = false;

SchedulerManager::SchedulerManager() : ledState(false) {}

void SchedulerManager::begin() {
    // La definición y uso de estos pines como salidas en el MASTER
    // se han habilitado para ver el estado de las salidas en los slaves
    pinMode(SCHEDULER_LED_PIN, OUTPUT);    // GPIO02 desinfección en master
    pinMode(SCHEDULER_LED2_PIN, OUTPUT);   // GPIO25 purificación en master
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

void SchedulerManager::applySchedule(const DateTime& now, bool enableFlag) {
    ledState = false;
    if (!enableFlag) return;

    TimePoint current = { now.hour, now.minute };

    // Día actual
    uint8_t indexToday = (now.weekday + 5) % 7;
    const DailySchedule& today = schedule.days[indexToday];

    // Día anterior
    uint8_t indexYesterday = (indexToday + 6) % 7;  // día anterior
    const DailySchedule& yesterday = schedule.days[indexYesterday];

    // Ver slots del día actual
    for (const auto& slot : today.slots) {
        if (!slot.enabled) continue;
        if (isWithinInterval(current, slot.onTime, slot.offTime)) {
            ledState = true;
            return;
        }
    }

    // Ver slots del día anterior que cruzan medianoche
    for (const auto& slot : yesterday.slots) {
        if (!slot.enabled) continue;
        uint16_t onMin  = slot.onTime.hour * 60 + slot.onTime.minute;
        uint16_t offMin = slot.offTime.hour * 60 + slot.offTime.minute;
/*      if (onMin > offMin) {  // cruza medianoche
          if (isWithinInterval(current, slot.onTime, slot.offTime)) {
            // Validación extra: si estamos en el "día siguiente",
            // y es después del offTime, no activar
            if (current >= slot.offTime) {
                continue; // Ya pasó el offTime → no debemos reactivar
            }
            ledState = true;
            return;
          }
        } */
        if (onMin > offMin) {  // cruza medianoche
          if (now.hour < 12 && isWithinInterval(current, slot.onTime, slot.offTime)) {
            // Estamos en la madrugada del día actual → válido
            ledState = true;
            return;
          } 
        }  
    }
}

void SchedulerManager::applySchedule2(const DateTime& now, bool enableFlag) {
    led2State = false;
    if (!enableFlag) return;

    TimePoint current = { now.hour, now.minute };

    uint8_t indexToday = (now.weekday + 5) % 7;
    const DailySchedule& today = schedule2.days[indexToday];

    uint8_t indexYesterday = (indexToday + 6) % 7;
    const DailySchedule& yesterday = schedule2.days[indexYesterday];

    for (const auto& slot : today.slots) {
        if (!slot.enabled) continue;
        if (isWithinInterval(current, slot.onTime, slot.offTime)) {
          led2State = true;
          return;
        }
    }

    for (const auto& slot : yesterday.slots) {
        if (!slot.enabled) continue;
        uint16_t onMin  = slot.onTime.hour * 60 + slot.onTime.minute;
        uint16_t offMin = slot.offTime.hour * 60 + slot.offTime.minute;
/*      if (onMin > offMin) {
          if (isWithinInterval(current, slot.onTime, slot.offTime)) {
            // Validación extra: si estamos en el "día siguiente", 
            // y es después del offTime, no activar
            if (current >= slot.offTime) {
              continue; // Ya pasó el offTime → no debemos reactivar
            }
            led2State = true;
            return;
          }
        } */
        if (onMin > offMin) {  // cruza medianoche
          if (now.hour < 12 && isWithinInterval(current, slot.onTime, slot.offTime)) {
            // Estamos en la madrugada del día actual → válido
            ledState = true;
            return;
          } 
        }
    }
}

bool SchedulerManager::isWithinInterval(const TimePoint& now, const TimePoint& start, const TimePoint& end) {
    uint16_t nowMin   = now.hour * 60 + now.minute;
    uint16_t startMin = start.hour * 60 + start.minute;
    uint16_t endMin   = end.hour * 60 + end.minute;

    if (startMin < endMin) {
        // Caso normal: dentro del mismo día
        return nowMin >= startMin && nowMin < endMin;
    } else if (startMin > endMin) {
        // Caso overnight: el intervalo cruza medianoche
        return nowMin >= startMin || nowMin < endMin;
    } else {
        // Caso especial: start == end → sin duración
        return false;
    }
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
        
        if (day > 6 || slot > 1) return;
        schedule.days[day].slots[slot] = { on, off, (uint8_t)enabled };
        saveToEEPROM();
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
        display.sendCommand(cmd);
    }
}