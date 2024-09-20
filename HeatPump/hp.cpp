#include "hp.h"
extern RemoteDebug Debug;
//extern WiFiClient espClient;
//extern PubSubClient mqttClient(espClient);
namespace hp
    {
        // uint8_t HeatPumpController::frame[HP_FRAME_LEN];
        // hpInfo HeatPumpController::hpData;
        void HeatPumpController::setup()
        {
           // this->high_freq_.start();
            settings::ctrlSettings defaultSettings = {
                55,                  // 1 uint8_t targetTemp;  f2
                45,                  // 2 uint8_t defrostAutoEnableTime; f3
                7,                   // 3 uint8_t defrostEnableTemp; f4
                13,                  // 4 uint8_t defrostDisableTemp; f5
                160,                 // 5 uint8_t defrostMaxDuration prev 8 ; f6
                2,                   // 6 uint8_t restartOffsetTemp; 
                5,                   // 7 uint8_t minTemp2enableHeatElement;
                35,                 // 8 uint8_t manualEEValve; f9
                0,                  // 9 uint8_t maximumTemp;
                0,                  // 10 uint8_t stopWhenReachedDelay;
                true,                // 11 bool specialCtrlMode;f7
                false,               // 12 bool on; f7
                settings::HEAT,      // 13 actionEnum action; f7
                true,                // 14 bool autoRestart; f7
                settings::HEAT_ONLY, // 15 modeEnum opMode;  f7
            };
            this->hpSettings = defaultSettings;
            
            this->sendControl(this->hpSettings);
            sender::setWireDirection(sender::RECEIVING);
            Debug.println("HP Controller Succesful setup !");
        }

        void HeatPumpController::sendControl(settings::ctrlSettings settings)
        {
            uint8_t frame[HP_FRAME_LEN];
            frame[0] = 0xCC; // header                                  //MessageID             0xCC-
            frame[1] = 0x0F; // header                                  //Reserved              0x0F-
            frame[2] = settings.targetTemp;                             //Target Temp           0x37-
            frame[3] = settings.defrostAutoEnableTime;                  //Defrosting Cycle run  0X2D
            frame[4] = settings.defrostEnableTemp;                      //Defrost Minimum Time  0x07
            frame[5] = settings.defrostDisableTemp;                     //Defrost Stop temp     0x0D
            frame[6] = settings.defrostMaxDuration ;                    //Defrost Duration      0xA0
            frame[7] = 0x00; // Clear any current flag                      //Multivalue        sets to all Zeros
            frame[7] |= (settings.specialCtrlMode ? 0x80 : 0x00);           //Multivalue        0xFC  Bit8 Set On
            frame[7] |= (settings.on ? 0x40 : 0x00);                        //Multivalue        0xFC  Bit7 Set On
            frame[7] |= ((settings.action == settings::HEAT) ? 0x20 : 0x00);//Multivalue        0xFC  Bit6 Set On 
            frame[7] |= 0x10;                                               //Multivalue        0xFC  Bit5 Set On                     
            frame[7] |= (settings.autoRestart ? 0x08 : 0x00);               //Multivalue        0xFC  Bit4 Set On
            frame[7] |= 0x04;                                               //Multivalue        0xFC  Bit2-3 (Bit3 Set On, Bit 2 set Off)
            //frame[7] |= ((int)settings.opMode & 0x02) << 1;               //Multivalue        0xFC/0xBC  Bit2-3
            //frame[7] |= 0x00                                              //Multivalue        0xFC Bit 1 Set Off
            frame[8] = settings.restartOffsetTemp;                          // 0x1D; // MODE 2 (NOT A HYBRID PUMP)                  //MODE??                0x02
            frame[9] = settings.manualEEValve;                                               //Manual control of EE valve       0x23
            frame[10] = settings.minTemp2enableHeatElement;                                               //Temperature to start the heating element (ambient)     0x05
            frame[11] = this->computeChecksum(frame, HP_FRAME_LEN);         //CRC                    0x4d/0x0D    
            Debug.println("Send settings down the line;");
            sender::sendFrame(frame, HP_FRAME_LEN);
        }

        bool HeatPumpController::decode(uint8_t frame[])
        {
            if (frame[0] == 0xD2)
            {
                this->hpData.targetTemp = frame[2];
                this->hpData.defrostAutoEnableTime = frame[3];
                this->hpData.defrostEnableTemp = frame[4];
                this->hpData.defrostDisableTemp = frame[5];
                this->hpData.defrostMaxDuration = frame[6] / 20;
                this->hpData.on = frame[7] & 0x40;                                    //bit7 Frame 7
                this->hpData.autoRestart = frame[7] & 0x08;                           //bit4 Frame 7
                this->hpData.opMode = static_cast<settings::modeEnum>(frame[7] & 0x06); // Bit2 and 3
                this->hpData.action = static_cast<settings::actionEnum>(frame[7] & 0x20);  //bit5
                this->hpData.specialCtrlMode = frame[7] & 0x80;                            //bit8
                this->hpData.restartOffsetTemp = frame[8]; 
                this->hpData.manualEEValve = frame[9]; 
                this->hpData.minTemp2enableHeatElement = frame[10];
                // this->hpData.manualEEValve = frame[11];
                // this->hpData.maximumTemp = frame[12];
                // this->hpData.stopWhenReachedDelay = frame[13];
                // this->hpData.targetTemp = frame[14];
                return true;
            }
            else if (frame[0] == 0xDD)
            {
                this->hpData.waterTempIn = frame[1];                                        //TempIn
                this->hpData.waterTempOut = frame[2];                                       //TempOut
                this->hpData.coilTemp = frame[3];                                           //Coil Temp
                this->hpData.airOutletTemp = frame[4];                                      //Outlet Temp
                this->hpData.outdoorAirTemp = frame[5];                                     //AirTemp/Outdoor Temp
                this->hpData.errorCode = static_cast<settings::hpErrorEnum>(frame[7]);      //Error Code
                this->hpData.dischargeGasTemp = frame[9];                                      //
                this->hpData.returnGasTemp = frame[8];
                this->hpData.maximumTemp = frame[12];
                this->hpData.stopWhenReachedDelay = frame[13];
                return true;
            }
            
           char buffer[sizeof(frame)*10 + 1];
           char *ptr = &buffer[0];
            for (byte i = 0; i < sender::frameCnt ; i++)
                { 
                  if (ptr != &buffer[0])
                      {
                        *ptr++ = ':';
                      }
                ptr += sprintf(ptr, "%02X", frame[i]);     
                                     
               }
            
            mqtt::publishOwn( MQTT_UKNOWN_FRAME,buffer);
            debugD("UNKNOWN MESSAGE! %s ",buffer );
                     
            return false;
        }

        

            
        void HeatPumpController::loop()
        {

            if (sender::readFrame())
            {
              
                if (this->frameIsValid(sender::read_frame, sender::frameCnt))
                {
                    
                    debugV("Got a frame");
                    
                    this->decode(sender::read_frame);
                   
                    if(sender::read_frame[0] == 0xD2)
                      {
                      debugD("Water temp IN %d, Water temp OUT: %d", this->hpData.waterTempIn, this->hpData.waterTempOut);
                      debugD("Outdoor temp %d, Coil Temp: %d , Target Temp %d", this->hpData.outdoorAirTemp, this->hpData.coilTemp,this->hpData.targetTemp);
                      mqtt::publishState(this->hpData.waterTempIn,this->hpData.waterTempOut,this->hpData.outdoorAirTemp,this->hpData.targetTemp);
                      }else if(sender::read_frame[0] == 0xDD)
                      {
                         debugD("PAC %s, temp target: %d", (this->hpData.on ? "ON" : "OFF"), this->hpData.targetTemp);
                         mqtt::publishOwn( MQTT_POWER_TOPIC,String(this->hpData.on).c_str());
                      } else
                      {
                         debugV("Frame has no match");
                      }
                }
                else
                { 
                    
                    this->decode(sender::read_frame);
                    debugD("PAC %s, temp target: %d", (this->hpData.on ? "ON" : "OFF"), this->hpData.targetTemp);
                    debugD("Invalid or corrupt frame ");
                    //debugD("Number of Bytes %s",sender::frameCnt);
                }
            }
            //if(this->hpSettings. != hpController.getOn()){hpController.setOn(settings::hpInfo::cmdPower);}
            //if(settings::hpInfo::cmdtargetTemp != hpController.getTargetTemp()){hpController.setTargetTemp(settings::hpInfo::cmdtargetTemp);}
        }

        void HeatPumpController::setOn(bool value)
        {
            this->hpSettings.on = value;
            this->sendControl(this->hpSettings);
        }
        void HeatPumpController::setTargetTemp(uint16_t value)
        {
            this->hpSettings.targetTemp = value;
            this->sendControl(this->hpSettings);
        }
        float HeatPumpController::getTargetTemp()
        {
            return (float)this->hpData.targetTemp;
        }
        float HeatPumpController::getWaterInTemp()
        {

            return (float)this->hpData.waterTempIn;
        }

        float HeatPumpController::getWaterOutTemp()
        {
            return this->hpData.waterTempOut;
        }

        float HeatPumpController::getOutdoorTemp()
        {
            return (float)this->hpData.outdoorAirTemp;
        }

        bool HeatPumpController::getOn()
        {
            return this->hpData.on;
        }

         bool HeatPumpController::get_MainsOn()
        {
            return this->hpData.mains_on;
        }
        uint16_t HeatPumpController::getdischargeGasTemp()
        {
            return this->hpData.dischargeGasTemp;
        }
       
        uint16_t HeatPumpController::getreturnGasTemp()
        {
            return this->hpData.returnGasTemp;
        }
        
        uint16_t HeatPumpController::getErrorCode()
        {
            return this->hpData.errorCode;
        }

        bool HeatPumpController::frameIsValid(uint8_t frame[], uint8_t size)
        {
         
            if (size == HP_FRAME_LEN || size == HP_FRAME_LEN +4)
            {
              
                return this->checksumIsValid(frame, size);
            }
            else
            {
                return false;
            }
            return false;
        }

        bool HeatPumpController::checksumIsValid(uint8_t frame[], uint8_t size)
        {
            unsigned char computed_checksum = this->computeChecksum(frame, size);
            unsigned char checksum = frame[size - 1];
            
            Debug.print("Checksum:");
            Debug.print(checksum);
            Debug.print(" Computed checksum: ");
            Debug.println(computed_checksum);
            return computed_checksum == checksum;
        }

        uint8_t HeatPumpController::computeChecksum(uint8_t frame[], uint8_t size)
        {
            unsigned int total = 0;     
            Debug.print ("Calculating CRC: ");
            for (byte i = 1; i < size - 1; i++)
            { 
                
                Debug.print(frame[i],HEX);
                Debug.print(" ");
                total += frame[i];
            }
            Debug.println("");
            byte checksum = total % 256;
            
            return checksum;
        }

    }

