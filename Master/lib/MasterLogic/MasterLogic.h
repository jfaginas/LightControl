#pragma once

#include <Arduino.h>
#include "EspNowInterface.h"
#include "CommProtocol.h"
#include "SchedulerManager.h"
#include "DS3231Manager.h"

namespace MasterLogic {
    void begin();
    void forceLedState(bool state);
    void sendToAllSlaves(bool led_on);
    void sendToAllSlaves(const MsgToSlave& msg);
    void sendSlot1ToSlaves(bool state);
    void sendSlot2ToSlaves(bool state);
}
