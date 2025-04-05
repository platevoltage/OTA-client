
/*
 * ESP32 Basic Sketch for automatic update of firmware at start
 *
 * Renzo Mischianti <www.mischianti.org>
 *
 * https://mischianti.org
 */

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

#ifndef APSSID
// #define APSSID "ESP32_OTA_Host"
// #define APPSK "12345678"

#define APSSID "ESP32_OTA_Host"
#define APPSK "12345678"

#endif

WiFiMulti WiFiMulti;

#define FIRMWARE_VERSION "0.2"

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APSSID, APPSK);

  Serial.print(F("Firmware version "));
  Serial.println(FIRMWARE_VERSION);
  delay(2000);
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    // The line below is optional. It can be used to blink the LED on the board
    // during flashing The LED will be on during download of one buffer of data
    // from the network. The LED will be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection
    // the LED will be on much longer than it will be off. Other pins than
    // LED_BUILTIN may be used. The second value is used to put the LED on. If
    // the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, LOW);

    httpUpdate.rebootOnUpdate(false); // remove automatic update

    Serial.println(F("Update start now!"));

    t_httpUpdate_return ret =
        httpUpdate.update(client, "http://192.168.4.1/firmware.bin");

    switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n",
                    httpUpdate.getLastError(),
                    httpUpdate.getLastErrorString().c_str());
      Serial.println(F("Retry in 10secs!"));
      delay(10000); // Wait 10secs
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      delay(1000); // Wait a second and restart
      ESP.restart();
      break;
    }
  }
}