#ifndef __HP_H__
#define __HP_H__
#include <Arduino.h>
#include <RemoteDebug.h>
#include "my_config.h"
#include "Sender.h"
#include "mqtt.h"


namespace hp
  {
    #define HP_FRAME_LEN 12 // Heat pump frame length

    class HeatPumpController 
    {
    public:
      void setup(); 
     // float get_setup_priority() const { return setup_priority::BUS; }

      void loop();
      void setTargetTemp(uint16_t value);
       const char* trameToString(byte t[], byte n);
      void setOn(bool value);
      float getTargetTemp();
      float getWaterInTemp();
      float getWaterOutTemp();
      float getOutdoorTemp();
      bool getOn();
      bool get_MainsOn();
      uint16_t getdischargeGasTemp();
      uint16_t getreturnGasTemp();
      uint16_t getErrorCode();

    protected:
      settings::hpInfo hpData;
      settings::ctrlSettings hpSettings;
      //HighFrequencyLoopRequester high_freq_;

      bool checksumIsValid(uint8_t frame[], uint8_t size);
      uint8_t computeChecksum(uint8_t frame[], uint8_t size);
      bool frameIsValid(uint8_t frame[], uint8_t size);
      void sendControl(settings::ctrlSettings settings);

      bool decode(uint8_t frame[]);
    };

  }


#endif // __HP_H__