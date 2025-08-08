#include "NextionManager.h"

NextionManager::NextionManager(HardwareSerial& serial)
    : _serial(serial), _commandReady(false) {}

void NextionManager::begin(uint32_t baudrate) {
    _serial.begin(baudrate);
}

void NextionManager::update() {
    parseSerial();
}

void NextionManager::parseSerial() {
    static uint8_t ff_count = 0;

    while (_serial.available()) {
        char c = _serial.read();

        // Ignorar caracteres de control no imprimibles (excepto salto de línea si lo usás)
        if ((uint8_t)c < 32 && c != '\r' && c != '\n') {
            continue;  // ← descarta caracteres como 0x1A (SUB)
        }

        if ((uint8_t)c == 0xFF) {
            ff_count++;
            if (ff_count == 3) {
                ff_count = 0;
                if (_buffer.length() > 0) {
                    _lastCommand = _buffer;
                    _buffer = "";
                    _commandReady = true;
                }
            }
        } else {
            _buffer += c;
            ff_count = 0;

            // Seguridad: evitar que el buffer crezca indefinidamente si llega ruido
            if (_buffer.length() > 40) {
                Serial.println("[Nextion] Buffer saturado. Descartando.");
                _buffer = "";
                ff_count = 0;
            }
        }
    }
}

bool NextionManager::isCommandAvailable() const {
    return _commandReady;
}

String NextionManager::getLastCommand() {
    _commandReady = false;
    return _lastCommand;
}

void NextionManager::showDateTime(const DateTime& dt) {
    //sendCommand("tdate.txt=\"" + dt.toDateString() + "\"");
    sendCommand("thour.txt=\"" + dt.toTimeString() + "\"");
}

void NextionManager::showWeekday(uint8_t weekday) {
    sendCommand("tddls.txt=\"" + String(weekdayToString(weekday)) + "\"");
}

void NextionManager::showError(const String& msg) {
    sendCommand("terror.txt=\"" + msg + "\"");
}

void NextionManager::gotoPage(const String& pageName) {
    sendCommand("page " + pageName);
}

void NextionManager::sendCommand(const String& cmd) {
    _serial.print(cmd);
    _serial.write(0xFF); _serial.write(0xFF); _serial.write(0xFF);
}

const char* NextionManager::weekdayToString(uint8_t weekday) {
    static const char* days[] = {
        // "Lunes", "Martes", "Miercoles", "Jueves",
        // "Viernes", "Sabado", "Domingo"
        "Mon", "Tue", "Wed", "Thu",
        "Fri", "Sat", "Sun"
    };

    if (weekday >= 1 && weekday <= 7)
        return days[(weekday + 5) % 7];  // misma conversión
    return "???";
}
void NextionManager::showDateComponents(const DateTime& dt) {
    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%02u", dt.day);
    sendCommand("tday.txt=\"" + String(buffer) + "\"");
    //sendCommand("tday.txt=\"" + String(dt.day) + "\"");
    sendCommand("tmonth.txt=\"" + String(monthToString(dt.month)) + "\"");
    sendCommand("tyear.txt=\"" + String(dt.year) + "\"");
    //sendCommand("tddls.txt=\"" + String(weekdayToString(weekday)) + "\"");
}

const char* NextionManager::monthToString(uint8_t month) {
    static const char* months[] = {
        // "Enero", "Febrero", "Marzo", "Abril",
        // "Mayo", "Junio", "Julio", "Agosto",
        // "Septiembre", "Octubre", "Noviembre", "Diciembre"

        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Set", "Oct", "Nov", "Dec"        
    };

    if (month >= 1 && month <= 12)
        return months[month - 1];
    return "???";
}