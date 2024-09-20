#ifndef __SENDER_H__
#define __SENDER_H__
#include <Arduino.h>
#include <RemoteDebug.h>
namespace sender
{
/**
 *  Defines
 */
#define PIN 5
#define LOW_TIME 1000
#define HIGH_1_TIME 1000
#define HIGH_0_TIME 3000

#define LOW_START_FRAME 9000 // Not used at the moment, Only "high" states are used".
#define HIGH_START_FRAME 5000

#define DURATION_MARGIN 500 // Margin of error for durations.

#define START_FRAME 1 // Indicateur d'état de la lecture d'une trame
#define IN_FRAME 2
#define END_FRAME 3

#define MAX_TIME 12000
#define MAX_FRAME_SIZE 20

#define SEND_MSG_OCCURENCE 3
    /**
     *  Enums
     */
    enum wireDirection
    {
        SENDING,
        RECEIVING
    };

    /**
     *  Variables
     */
    extern volatile unsigned long triggerTime;
    extern volatile unsigned long lastTriggerTime;

    extern volatile unsigned long triggerDeltaTime;

    extern volatile byte triggerStatus;
    extern volatile byte lastTriggerStatus;

    extern volatile boolean triggered;

    extern uint8_t read_frame[MAX_FRAME_SIZE];
    extern uint8_t frameCnt;

    /**
     *  Functions
     */

    inline unsigned long delaisWithoutRollover(unsigned long t1, unsigned long t2);
    uint8_t reverseBits(unsigned char x);
    void isrCallback(void);
    void setWireDirection(wireDirection direction);

    /* ================================================== */

    void sendHigh(uint16_t ms);
    void sendLow(uint16_t ms);
    void sendBinary0(void);
    void sendBinary1(void);
    void sendHeaderCmdFrame(void);
    void sendSpaceCmdFrame(void);
    void sendSpaceCmdFramesGroup(void);
    void sendFrame(uint8_t frame[], uint8_t size);

    /* ================================================== */

    boolean startFrame(void);
    inline uint8_t readBit(void);
    inline boolean silence(void);
    boolean longSilence();
    boolean readFrame();

}
#endif // __SENDER_H__