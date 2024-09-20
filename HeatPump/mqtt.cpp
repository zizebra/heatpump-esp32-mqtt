#include "mqtt.h"
extern RemoteDebug Debug;
extern hp::HeatPumpController hpController;
namespace mqtt
{
 
  
  WiFiClient espClient;
  PubSubClient mqttClient(espClient);
   

 
  int wifiConnectionFailedCount=0;
  int mqttConnectionFailedCount=0;
  
  void mqttLoop()
    {
     if (!mqttClient.connected()) 
      {
        reconnectMQTT();
      }
       
    mqttClient.loop();

  }
  
  void initOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
Serial.println("Initializing OTA Endpoint.");
  
  ArduinoOTA.setHostname(HARDWARE_HOSTNAME);

  // No authentication by default
  ArduinoOTA.setPassword(OTA_PASSWORD);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

  void initWIFI() {
  Serial.print("Connecting to WiFi AP: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.hostname(HARDWARE_HOSTNAME);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) 
    {
      if(wifiConnectionFailedCount==1)
        {
           Serial.print("Connection Failed! Trying again...");
        }
      wifiConnectionFailedCount++;
      delay(5000);
      Serial.print(".");
    }
  Serial.println("");
  Serial.println("WiFi connected successfully.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 // t.every(5000, fiveSecLoop);
 // t.every(10000, tenSecLoop);
  wifiConnectionFailedCount=0;  
  
}

void initMQTT() {
  Debug.println("Initializing MQTT");
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCallback(callback);
  mqttClient.setKeepAlive( 90 );
  reconnectMQTT();
  
}
void pubSubErr(int8_t MQTTErr)
{
  if (MQTTErr == MQTT_CONNECTION_TIMEOUT)           { Debug.println("MQTT Error.Connection tiemout");       }
  else if (MQTTErr == MQTT_CONNECTION_LOST)         { Debug.println("MQTT Error.Connection lost");          }
  else if (MQTTErr == MQTT_CONNECT_FAILED)          { Debug.println("MQTT Error.Connect failed");           }
  else if (MQTTErr == MQTT_DISCONNECTED)            { Debug.println("MQTT Error.Disconnected");             }
  else if (MQTTErr == MQTT_CONNECTED)               { Debug.println("MQTT Error.Connected");                }
  else if (MQTTErr == MQTT_CONNECT_BAD_PROTOCOL)    { Debug.println("MQTT Error.Connect bad protocol");     }
  else if (MQTTErr == MQTT_CONNECT_BAD_CLIENT_ID)   { Debug.println("MQTT Error. Connect bad Client-ID");   }
  else if (MQTTErr == MQTT_CONNECT_UNAVAILABLE)     { Debug.println("MQTT Error. Connect unavailable");     }
  else if (MQTTErr == MQTT_CONNECT_BAD_CREDENTIALS) { Debug.println("MQTT Error. Connect bad credentials"); }
  else if (MQTTErr == MQTT_CONNECT_UNAUTHORIZED)    { Debug.println("MQTT Error. Connect unauthorized");    }
}



void reconnectMQTT() 
{
  String clientId = MQTT_CLIENT_ID;
 mqttClient.connect(clientId.c_str(), "", "", MQTT_AVAILABILITY_TOPIC, 1, true, "Offline");
   while (!mqttClient.connected()) 
   {
    if (mqttConnectionFailedCount ==1)
      {
        Debug.print("mqttClient Attempting connection...");
        mqttClient.publish(MQTT_AVAILABILITY_TOPIC, "Offline", true);
      }
        Serial.print("failed, rc=");
        Serial.print(mqttClient.state());
        Serial.println(" try again in 5 seconds");
        // Wait 5 seconds before retrying
        delay(5000);
        mqttConnectionFailedCount++;
        if (mqttConnectionFailedCount % 10 == 0) 
         {
          delay(5000);
          ESP.restart();
        }
       
     }
    Serial.println("MQTT Client connected");
    if (!mqttClient.subscribe(MQTT_POWER_SET_TOPIC)) pubSubErr(mqttClient.state());
		if (!mqttClient.subscribe(MQTT_MODE_SET_TOPIC))  pubSubErr(mqttClient.state());
		if (!mqttClient.subscribe(MQTT_TEMP_SET_TOPIC))  pubSubErr(mqttClient.state());
		if (!mqttClient.subscribe(MQTT_FAN_SET_TOPIC))   pubSubErr(mqttClient.state());
    if (!mqttClient.subscribe(MQTT_POWER_TOPIC))     pubSubErr(mqttClient.state());
    if (!mqttClient.subscribe(MQTT_MPOWER_TOPIC))     pubSubErr(mqttClient.state());
        mqttClient.publish(MQTT_AVAILABILITY_TOPIC, "Online", true);
		    mqttConnectionFailedCount = 0;
       
}
void callback(char *topic, byte *payload, unsigned int length) 
{
payload[length] = '\0';
char message[length + 1];
strncpy (message, (char*)payload, length);
message[length] = '\0';

  if(strcmp(topic,MQTT_MPOWER_TOPIC) == 0 && strcmp((char *)payload, "0") ==0)//Stop publishing if there is no power on HP
      {
        debugD("Heat Pump Lost Power");
        hpController.setOn(0);
      }
  
      // --------------------------------------We received set command from OH via MQTT---------------------------------------------------
      char* str ="test";
       if ( strstr(topic,str) )
          {
           
           debugV("Command Received: Topic- %s", String(topic));

            if (strcmp(topic,MQTT_POWER_SET_TOPIC) == 0)
              {  
               Debug.println("NewMessage :"+ String(message)); 
              if(((strcmp((strupr((char *)payload)), "ON") == 0)) ||(strcmp((char *)payload, "1") == 0))
                // if (strcmp((char *)payload, "on") == 0)
                  {
                  
                  Debug.println("Power On Command received. Switching on HP");
                  hpController.setOn(1);
                  
                  
                
                  }else if(((strcmp((strupr((char *)payload)), "OFF") == 0)) ||(strcmp((char *)payload, "0") == 0))
                  {
                  Debug.println("Power Off Command received. Switching off HP");
                  hpController.setOn(0);
                  }else
                 {
                  Debug.println("Invalid Power Command");
             //do nothing
                 }
              // --------------------Set Target Temp - Frame2-----------------------
              } else if (strcmp(topic,MQTT_TEMP_SET_TOPIC) == 0)
              {
                Debug.print("Temperature Value: ");
                Serial.println((char *)payload);
                // Handle temperature commands
                String s = String((char *)payload);
                float temp = s.toFloat();
        
                if (temp > 0) 
                    {
                       Debug.println("New Target Temperature Received. Adjusting on HP");
                      hpController.setTargetTemp(temp);
                    } else 
                    {
                     Debug.println("Invalid temperature value");
                    }
          
              } else if (strcmp(topic,MQTT_HEAT_SET_TOPIC) == 0)
              {
                  if(strcmp((char *)payload, "on") == 0)
                  {
                 // cmdPower=1;
                  Debug.println("Set Heating On");
                  
                
                  }else if(strcmp((char *)payload, "off") == 0)
                  {
                   Debug.println("Set Heating Off");
                  
                  }else
                 {
                  Debug.println("Invalid Power Command");
             //do nothing
                 }
              }
              
              
              
              else
              {
                Debug.println("Undefined Topic");
              }
    
             // mqttClient.publish("heatpump/stat/callback", "Ready for next command", true);
              //-------------------------------------End of OHCommand-------------------------------------------------------

  
          }
}
void publishState(uint16_t ctemp_in,uint16_t ctemp_out,uint16_t cAmbTemp,uint16_t ctargetTemp) {
    debugV("Publishing Values");
    
    String rssi;
    if( ctemp_in > 0 )    
    {    
        mqttClient.publish(MQTT_CURRENT_TEMP_IN_TOPIC, String(ctemp_in).c_str()); 
        mqttClient.publish(MQTT_CURRENT_TEMP_OUT_TOPIC, String(ctemp_out).c_str());
        mqttClient.publish(MQTT_AMBIENT_TEMP_OUT_TOPIC, String(cAmbTemp).c_str());
        mqttClient.publish(MQTT_PROG_TEMP_TOPIC, String(ctargetTemp).c_str());
        mqttClient.publish(MQTT_WIFI_RSSI_TOPIC, String(rssi = WiFi.RSSI()).c_str());
    
    }
    
   
}
void publishOwn(  String topic, String payload)
{
debugV("Publishing Payload %s", payload);
mqttClient.publish(topic.c_str(),  payload.c_str() );
}


}