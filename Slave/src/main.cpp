#include <Arduino.h>
#include "EspNowInterface.h"
#include "SlaveApp.h"

void setup() {
    SlaveApp::begin();
}

void loop() {
    SlaveApp::loop();
}