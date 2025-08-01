#pragma once

#include <stdint.h>

// Master MAC ------------------------------------------------------------------
constexpr uint8_t MASTER_MAC[] = {0xCC, 0x50, 0xE3, 0xA1, 0x3F, 0x74};
//------------------------------------------------------------------------------

// Slave Configuracion ---------------------------------------------------------
constexpr uint8_t SLAVE_ID = 2;  // Cambiar para cada firmware del Slave

// Pines de salida controlados por el master vía ESP-NOW
constexpr uint8_t LED_CONTROL_PIN  = 2;   // LED vinculado al botón del Nextion
constexpr uint8_t SCHED_SLOT1_PIN  = 33;  // LED controlado por horario del slot 1
constexpr uint8_t SCHED_SLOT2_PIN  = 32;  // LED controlado por horario del slot 2

// Definición de tipos de mensajes para distinguir el propósito
enum class MsgType : uint8_t {
    STATUS_UPDATE,     // Master → Slave: estado actualizado de LEDs
    STATUS_REQUEST,    // Slave → Master: solicitud de estado actual
    STANDARD_COMMAND,  // Master → Slave: comandos estándar futuros
    ACK                // Slave → Master: confirmación de recepción / ejecución
};

// Mensaje que el Master envía al Slave
struct __attribute__((packed)) MsgToSlave {
    MsgType type;          // Tipo de mensaje (STATUS_UPDATE, STANDARD_COMMAND)
    uint8_t payload_id;    // Identificador para correlacionar respuestas
    bool led_on;           // Estado GPIO2
    bool slot1_on;         // Estado GPIO33
    bool slot2_on;         // Estado GPIO32
};

// Mensaje que el Slave envía al Master
struct MsgToMaster {
    MsgType type;          // Tipo de mensaje (STATUS_REQUEST, ACK)
    uint8_t slave_id;      // Identificador del Slave que envía el mensaje
};