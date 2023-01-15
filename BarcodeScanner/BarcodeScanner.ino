/**
 * BarcodeScanner.ino
 */
#define _GNU_SOURCE
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "conf.h"


WiFiMulti wifiMulti;

void setup() {
    Serial.begin(115200);
    wifiMulti.addAP(SSID, WIFI_PASS);
}


void loop() {
    if((wifiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;
        http.begin("http://" SERVER_IP ":8080/barcode"); //HTTP
        http.addHeader("Content-Type", "text/plain");
        
        int httpCode = http.POST("715515210119");
        String payload = http.getString();

        Serial.println(String(httpCode) + ": " + payload);
        http.end();
    }
    delay(5000);
}
