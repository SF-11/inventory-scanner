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
#define OK_PIN 2
#define FAIL_PIN 4

WiFiMulti wifiMulti;


void setup() {
  pinMode(OK_PIN, OUTPUT);
  pinMode(FAIL_PIN, OUTPUT);
  
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
    int http_code = post_barcode(barcode);
    set_output(http_code);
  }
  delay(2000);
  digitalWrite(OK_PIN, LOW);
  digitalWrite(FAIL_PIN, LOW);
}


int post_barcode(char* barcode) {
  int http_code = 0;
  
  if((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    http.begin("http://" SERVER_IP ":8080/barcode"); //HTTP
    http.addHeader("Content-Type", "text/plain");
    http_code = http.POST(barcode);
    http.end();
  }
  return http_code;
}


char* read_barcode() {

  char* buf = (char *) malloc(14);
  int i = 0;
  while (Serial.available() > 0 && i < 13) {
    buf[i] = Serial.read();
    i++;
  }
  buf[i] = '\0';
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


void set_output(int http_code) {
  switch(http_code) {
    case 200:
      digitalWrite(OK_PIN, HIGH);
      break;
    case 429:
      digitalWrite(FAIL_PIN, HIGH);
      delay(200);
      digitalWrite(FAIL_PIN, LOW);
      delay(200);
      digitalWrite(FAIL_PIN, HIGH);
      break;
    default:
      digitalWrite(FAIL_PIN, HIGH);
  }
}
