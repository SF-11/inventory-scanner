/**
 * BarcodeScanner.ino
 */
#define _GNU_SOURCE
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "conf.h"

#define ACK 0x06
#define NAK 0x15

WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  wifiMulti.addAP(SSID, WIFI_PASS);
}


void loop() {
  int response = 0;
  do { 
    response = start_scan();
  } while (response != ACK);

  
  char* barcode = NULL;
  if (Serial.available() > 0) {
    barcode = read_barcode();
    post_barcode(barcode);
  }
  delay(200);
}


void post_barcode(char* barcode) {
  if((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin("http://" SERVER_IP ":8080/barcode"); //HTTP
    http.addHeader("Content-Type", "text/plain");
    
    int httpCode = http.POST(barcode);
    String payload = http.getString();

    http.end();
  }
}


char* read_barcode() {

  char* buf = (char *) malloc(14);
  int i = 0;
  while (Serial.available() > 0 && i < 13) {
    buf[i] = Serial.read();
    i++;
  }
  buf[13] = '\0';
  Serial.flush();
    
  return buf;
}


int start_scan() {
  int response;

  Serial.println("^_^SCAN.");
  delay(500); 
  if (Serial.available() > 0) {
    response = Serial.read();
  }
  return response;
}
