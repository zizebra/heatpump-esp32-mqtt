#include <Arduino.h>
#include <RemoteDebug.h>
#include <telnet.h>
#include "Sender.h"
#include "hp.h"
#include "my_config.h"
#include "Timer.h"
#include "mqtt.h"

#ifdef ESP32
    #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
 #endif

 hp::HeatPumpController hpController;
 RemoteDebug Debug;
 
 Timer t;
 WiFiClient espClient; 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial);
  Serial.println("\nStarting Serial Port at 115200");
  Serial.println("ITS Heat Pump ESP Wifi V2");
  mqtt::initWIFI();
  mqtt::initOTA();
  mqtt::initMQTT();
  Serial.println("Initializing Wireless Logger.");
  Debug.begin(HARDWARE_HOSTNAME);
  Debug.setResetCmdEnabled(true);
  Debug.setSerialEnabled(true); 
  Debug.showProfiler(true);        // Profiler (Good to measure times, to optimize codes)
  Debug.showColors(true); 
  t.every(10000, tenSecLoop);
 // t.every(300000, fiveSecLoop);
  Debug.println("Telnet Debug Ready");
  Debug.println("Setup completed on host: " + String(HARDWARE_HOSTNAME) );
  hpController.setup();
  Debug.println("====System Ready====");        // Colors
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();
  mqtt::mqttLoop();
  hpController.loop();
  t.update();
  Debug.handle();
}
void tenSecLoop() {
 
 
}
