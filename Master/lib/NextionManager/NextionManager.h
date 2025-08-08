#pragma once

#include <Arduino.h>
#include <DateTimeUtils.h>

class NextionManager {
public:
    explicit NextionManager(HardwareSerial& serial);
    void begin(uint32_t baudrate = 9600);
    void update();
    
    void sendCommand(const String& cmd);
    bool isCommandAvailable() const;
    String getLastCommand();

    void showDateTime(const DateTime& dt);
    void showWeekday(uint8_t weekday);
    void showDateComponents(const DateTime &dt);
    void showError(const String& msg);
    void gotoPage(const String& pageName);

    const char* weekdayToString(uint8_t weekday);
    const char *monthToString(uint8_t month);

private:
    HardwareSerial& _serial;
    String _buffer;
    String _lastCommand;
    bool _commandReady;

    void parseSerial();
};