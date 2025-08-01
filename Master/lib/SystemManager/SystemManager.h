#pragma once

#include <Arduino.h>
#include <DS3231Manager.h>
#include <SchedulerManager.h>
#include <NextionManager.h>
#include <DateTimeUtils.h>

class SystemManager {
public:
    SystemManager();
    void begin();
    void update();

private:
    DS3231Manager rtc;
    SchedulerManager scheduler;
    NextionManager nextion;

    bool timeSetStepPending = false;   // Si falta completar SETTIME después de SETDATE
    DateTime tempDate;                 // Guarda la fecha parcialmente seteada

    void handleCommand(const String& cmd);
    void handleSetDate(const String& data);
    void handleSetTime(const String& data);
};
