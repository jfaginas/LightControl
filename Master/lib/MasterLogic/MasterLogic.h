#pragma once

#include <Arduino.h>
#include "EspNowInterface.h"
#include "CommProtocol.h"
#include "SchedulerManager.h"
#include "DS3231Manager.h"

namespace MasterLogic {
    void begin();
    //void forceLedState(bool state);
    void forceLedState(bool state, uint8_t pwm_value);
    void sendToAllSlaves(bool led_on);
    void sendToAllSlaves(bool led_on, uint8_t pwm_value);
    void sendToAllSlaves(const MsgToSlave& msg);
    void sendSlot1ToSlaves(bool state);
    void sendSlot2ToSlaves(bool state);
}
