#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>
#include <RemoteDebug.h>
#include <ArduinoOTA.h>
#ifdef ESP32
    #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <PubSubClient.h>
#include "my_config.h"
#include "hp.h"

namespace mqtt
{
 
void initOTA();
void initWIFI();
void initMQTT();
void pubSubErr(int8_t MQTTErr);
void mqttLoop();
void callback(char *topic, byte *payload, unsigned int length);
void reconnectMQTT();
void publishState(uint16_t ctemp_in,uint16_t ctemp_out,uint16_t cAmbTemp,uint16_t ctargetTemp);
void publishOwn(String topic, String payload);
}
#endif