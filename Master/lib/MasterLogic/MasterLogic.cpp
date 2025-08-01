#include "MasterLogic.h"

SchedulerManager schedulerManager;
DS3231Manager rtc;  // instancia global del RTC

namespace MasterLogic {

    // Variables internas
    static bool last_button_state = HIGH;
    static bool led_state = false;

    static uint8_t payload_id = 0;

    static bool currentSlot1State = false;
    static bool currentSlot2State = false;

    void onReceive(const uint8_t* mac, const uint8_t* data, int len) {
        if (len == sizeof(MsgToMaster)) {
            const MsgToMaster* msg = reinterpret_cast<const MsgToMaster*>(data);

            if (msg->type == MsgType::STATUS_REQUEST) {
                Serial.println("[MASTER] STATUS_REQUEST recibido de un Slave. Respondiendo...");
                MsgToSlave response;
                response.type      = MsgType::STATUS_UPDATE;
                response.payload_id = 0;                   // Por ahora sin uso específico
                response.led_on    = led_state;
                response.slot1_on  = currentSlot1State;
                response.slot2_on  = currentSlot2State;
                bool success = EspNowInterface::send(mac, reinterpret_cast<const uint8_t*>(&response), sizeof(response));
                Serial.printf("[MASTER] Enviado STATUS_UPDATE al Slave. Resultado: %s\n", success ? "OK" : "FALLÓ");
                return;
            }
            Serial.println("[MASTER] Mensaje no reconocido o con tamaño inesperado.");
        }
    }

    void begin() {
        Serial.begin(115200);
        delay(500);
        if (!EspNowInterface::begin()) {
            Serial.println("[ERROR] ESP-NOW no inició");
            return;
        }
        EspNowInterface::onReceive(MasterLogic::onReceive);
        for (int i = 0; i < MAX_SLAVES; ++i) {
            bool added = EspNowInterface::addPeer(SLAVE_LIST[i].mac, 1);
            Serial.printf("[MASTER] Peer SLAVE %d agregado: %s\n", SLAVE_LIST[i].id, added ? "OK" : "FALLÓ");
        }
        sendSlot1ToSlaves(schedulerManager.getSlot1State());
        sendSlot2ToSlaves(schedulerManager.getSlot2State());
    }

    void forceLedState(bool state) {
        if (led_state != state) {
            led_state = state;
            Serial.printf("[MASTER] LUZ: %s\n", led_state ? "ON" : "OFF");
            sendToAllSlaves(led_state);
        }
    }

    void sendToAllSlaves(bool led_on) {
        MsgToSlave msg = {
            .type = MsgType::STATUS_UPDATE,
            .payload_id = static_cast<uint8_t>(++payload_id),
            .led_on = led_on,
            .slot1_on = currentSlot1State,
            .slot2_on = currentSlot2State
        };
        sendToAllSlaves(msg);
    }

    void sendSlot1ToSlaves(bool state) {
        if (currentSlot1State == state) return;  // Evita reenvíos innecesarios
        currentSlot1State = state;

        MsgToSlave msg;
        msg.type = MsgType::STATUS_UPDATE;
        msg.led_on = led_state;
        msg.slot1_on = currentSlot1State;
        msg.slot2_on = currentSlot2State;

        Serial.printf("[MASTER] Enviando estado SLOT1 (%s) a Slaves\n", state ? "ON" : "OFF");
        sendToAllSlaves(msg);
    }

    void sendSlot2ToSlaves(bool state) {
        if (currentSlot2State == state) return;
        currentSlot2State = state;

        MsgToSlave msg;
        msg.type = MsgType::STATUS_UPDATE;
        msg.led_on = led_state;
        msg.slot1_on = currentSlot1State;
        msg.slot2_on = currentSlot2State;

        Serial.printf("[MASTER] Enviando estado SLOT2 (%s) a Slaves\n", state ? "ON" : "OFF");
        sendToAllSlaves(msg);
    }

    void sendToAllSlaves(const MsgToSlave& msg) {
        for (int i = 0; i < MAX_SLAVES; ++i) {
            EspNowInterface::send(SLAVE_LIST[i].mac, (uint8_t*)&msg, sizeof(msg));
        }
    }
}