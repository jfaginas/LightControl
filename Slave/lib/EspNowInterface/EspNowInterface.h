#pragma once

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

using ReceiveCallback = void (*)(const uint8_t* mac, const uint8_t* data, int len);
using SentCallback    = void (*)(const uint8_t* mac, esp_now_send_status_t status);

class EspNowInterface {
public:
    static bool begin(uint8_t wifi_channel = 1);
    static bool send(const uint8_t* mac, const uint8_t* data, size_t len);
    static bool addPeer(const uint8_t* mac, uint8_t channel = 1);
    static void onReceive(ReceiveCallback callback);
    static void onSent(SentCallback callback);
};

