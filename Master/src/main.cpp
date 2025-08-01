#include <Arduino.h>
#include "SystemManager.h"
#include "MasterLogic.h"

SystemManager systemManager;

void setup() {
    Serial.begin(115200);
    systemManager.begin();
    MasterLogic::begin();
}

void loop() {
    systemManager.update();
}