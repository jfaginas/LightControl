#pragma once
#include <stdint.h>

constexpr int MAX_SLAVES = 2;   // Actualizar según la cantidad de SLAVES

enum class MsgType : uint8_t {
    STATUS_UPDATE,     // El Master informa el estado actual de los LEDs al Slave
    STATUS_REQUEST,    // El Slave solicita al Master el estado de los LEDs
    STANDARD_COMMAND   // Comandos comunes de control (ej: Master -> Slaves)
};

// Mensaje que el Master envía al Slave
struct __attribute__((packed)) MsgToSlave {
    MsgType type;       // Tipo de mensaje (ej: STATUS_UPDATE o STANDARD_COMMAND)
    uint8_t payload_id; // Opcional: identificador del payload
    bool led_on;        // Estado de GPIO2
    bool slot1_on;      // Estado de GPIO33
    bool slot2_on;      // Estado de GPIO32
    uint8_t pwm_value;  // Valor PWM (0-255) asociado al led_on
};

// Mensaje que el Slave envía al Master
struct MsgToMaster {
    MsgType type;       // STATUS_REQUEST u otros en el futuro
    uint8_t slave_id;   // Identificador del Slave
};

struct SlaveEntry {
    uint8_t id;
    uint8_t mac[6];
};

// Actualizar la lista MAC según la cantidad de SLAVES según su id ----->

const SlaveEntry SLAVE_LIST[MAX_SLAVES] = {
    {1, {0x10, 0x06, 0x1C, 0xF4, 0xC2, 0xC0}}, // SLAVE1
    {2, {0x10, 0x06, 0x1C, 0xF4, 0x5A, 0xBC}}, // SLAVE2
};