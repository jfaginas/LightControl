#include "SlaveApp.h"

namespace {

void sendStatusRequest() {
    MsgToMaster request;
    request.type = MsgType::STATUS_REQUEST;
    request.slave_id = SLAVE_ID;
    EspNowInterface::send(MASTER_MAC, reinterpret_cast<const uint8_t*>(&request), sizeof(request));
    Serial.println("[SLAVE] STATUS_REQUEST enviado al MASTER");
}

// Establecer valor PWM (0-255)
void setPWMValue(uint8_t value) {
    ledcWrite(PWM_CHANNEL, value);
}


void onReceive(const uint8_t* mac, const uint8_t* data, int len) {

    if (memcmp(mac, MASTER_MAC, 6) != 0) {
        Serial.println("[SLAVE] MAC desconocida, ignorando paquete");
        return;
    }

    if (len != sizeof(MsgToSlave)) {
        Serial.println("[SLAVE] Tamaño inesperado de mensaje recibido");
        return;
    }

    const MsgToSlave* msg = reinterpret_cast<const MsgToSlave*>(data);

    switch (msg->type) {

        case MsgType::STATUS_UPDATE:
            Serial.println("[SLAVE] Recibido STATUS_UPDATE del MASTER");
            digitalWrite(LED_CONTROL_PIN, msg->led_on ? HIGH : LOW);  // Led de luz ambiental
            // Control PWM en GPIO19 (solo si led_on está activo)
            if (msg->led_on) {
                setPWMValue(msg->pwm_value);
            } else {
                setPWMValue(0); // Apagar PWM
            }
            digitalWrite(SCHED_SLOT1_PIN, msg->slot1_on ? HIGH : LOW);
            digitalWrite(SCHED_SLOT2_PIN, msg->slot2_on ? HIGH : LOW);
            break;

        default:
            Serial.println("[SLAVE] Tipo de mensaje no reconocido");
            break;
    }
}

} // namespace Anonymous

void SlaveApp::begin() {
    Serial.begin(115200);
    delay(100);

    pinMode(LED_CONTROL_PIN, OUTPUT);
    pinMode(SCHED_SLOT1_PIN, OUTPUT);
    pinMode(SCHED_SLOT2_PIN, OUTPUT);

    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
    ledcAttachPin(PWM_PIN, PWM_CHANNEL);


    Serial.println("[SLAVE] Iniciando...");
    EspNowInterface::begin(1); // WIFI Channel
 
    if (EspNowInterface::addPeer(static_cast<const uint8_t*>(MASTER_MAC))) {
        Serial.println("[SLAVE] Master agregado como peer.");
    } else {
        Serial.println("[SLAVE] ERROR: No se pudo agregar al master como peer.");
    }

    EspNowInterface::onReceive(onReceive);
    
    // Enviamos solicitud de estado al master
    sendStatusRequest();
    Serial.println("[SLAVE] Esperando comando del Master");
}

void SlaveApp::loop() {}