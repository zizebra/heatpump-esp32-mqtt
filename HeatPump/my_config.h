#ifndef __MY_CONFIG_H__
#define __MY_CONFIG_H__
#define WIFI_SSID "your-ssid"
#define WIFI_PASS "your-ssid-password"
#define MQTT_HOST "your-mqqt-host"
#define MQTT_PORT 1883
#define MQTT_USER "YOURMQTTUSER"
#define MQTT_PASS "YOURMQTTPASS"
#define HARDWARE_HOSTNAME "HeatPump"
#define OTA_PASSWORD "set-a-new-ota-password"
#define MQTT_CLIENT_ID "HeatPump"
#define MQTT_AVAILABILITY_TOPIC "heatpump/node1/state"

// Status Topics - See climate.yaml for an example of how I have this set up
#define MQTT_CURRENT_TEMP_IN_TOPIC "heatpump/stat/current_temp_in"
#define MQTT_CURRENT_TEMP_OUT_TOPIC "heatpump/stat/current_temp_out"
#define MQTT_POWER_TOPIC "heatpump/stat/power"
#define MQTT_MPOWER_TOPIC "heatpump/stat/mainspower"
#define MQTT_MODE_TOPIC "heatpump/stat/mode"
#define MQTT_PROG_TEMP_TOPIC "heatpump/stat/progtemp"
#define MQTT_FAN_TOPIC "heatpump/stat/fan"
#define MQTT_UKNOWN_FRAME "heatpump/stat/uframe"
#define MQTT_WIFI_RSSI_TOPIC "heatpump/stat/wifi_rssi"
#define MQTT_AMBIENT_TEMP_OUT_TOPIC "heatpump/stat/ambient_temp"


// Control Topics
#define MQTT_POWER_SET_TOPIC "heatpump/cmnd/power"
#define MQTT_MODE_SET_TOPIC "heatpump/cmnd/mode"
#define MQTT_TEMP_SET_TOPIC "heatpump/cmnd/temp"
#define MQTT_FAN_SET_TOPIC "heatpump/cmnd/fan"
#define MQTT_HEAT_SET_TOPIC "heatpump/cmnd/heat"
namespace settings
{
    
    enum actionEnum
    {
        HEAT = 1,
        AUTO = 0
    };
   enum  pumpWorkMode
    {
        PUMPMODE = 1,
        NO_PMODE = 0
    };
    enum modeEnum
    {
        HEAT_ONLY = 1,
        COLD_ONLY = 0,
        BOTH = 2
    };
    enum hpErrorEnum
    {
        NONE = 0x0,
        PD = 0x04
    };
    typedef struct
    {
        // ============================
        uint8_t targetTemp;
        uint8_t defrostAutoEnableTime;
        uint8_t defrostEnableTemp;
        uint8_t defrostDisableTemp;
        uint8_t defrostMaxDuration;
        uint8_t restartOffsetTemp;
        uint8_t minTemp2enableHeatElement;
        uint8_t manualEEValve;
        uint8_t maximumTemp;
        uint8_t stopWhenReachedDelay;
        // =============MODE==========
        bool specialCtrlMode;
        bool on;
        actionEnum action;
        bool autoRestart;
        modeEnum opMode;
        pumpWorkMode pumpWMode;
    } ctrlSettings;

    typedef struct
    {
        // ============================
        uint8_t targetTemp;
        uint8_t defrostAutoEnableTime;
        uint8_t defrostEnableTemp;
        uint8_t defrostDisableTemp;
        uint8_t defrostMaxDuration;
        uint8_t restartOffsetTemp;
        uint8_t minTemp2enableHeatElement;
        uint8_t manualEEValve;
        uint8_t maximumTemp;
        uint8_t stopWhenReachedDelay;
        // =============MODE==========
        bool specialCtrlMode;
        bool on;
        bool mains_on;
        actionEnum action;
        bool autoRestart;
        modeEnum opMode;
        pumpWorkMode pumpWMode;
        // =============DATA==========

        uint8_t waterTempIn;
        uint8_t waterTempOut;
        uint8_t coilTemp;
        uint8_t airOutletTemp;
        uint8_t outdoorAirTemp;
        hpErrorEnum errorCode;
        uint8_t dischargeGasTemp;
        uint8_t returnGasTemp;
    } hpInfo;
}
#endif // __MY_CONFIG_H__