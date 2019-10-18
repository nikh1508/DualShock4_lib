#ifndef DualShock4_h
#define DualShock4_h
#endif
#define DualShock4_lib_DEBUG
#if ARDUINO > 22
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <HardwareSerial.h>

#define MANUAL_UPDATE false
#define AUTOMATIC_UPDATE true

enum devices
{
    LX = 101,
    LY,
    RX,
    RY,
    BUTTONS,
    TRIG_L,
    TRIG_R
};
enum buttons_list
{
    R2 = 0,
    R1,
    L2,
    L1,
    CIRCLE,
    SQUARE,
    CROSS,
    TRIANGLE,
    RB,
    LB,
    PS,
    OPTIONS,
    TOUCHPAD,
    SHARE,
    HAT_RIGHT,
    HAT_LEFT,
    DOWN,
    UP
};
constexpr byte startMarker = 254;
constexpr byte endMarker = 255;
constexpr byte specialByte = 253;

class DualShock4
{
    unsigned int LX_val;
    unsigned int LY_val;
    unsigned int RX_val;
    unsigned int RY_val;
    unsigned long buttons;
    unsigned long lastButtons;
    unsigned int TRIG_L_val;
    unsigned int TRIG_R_val;
    unsigned int sampleTime; //usable only in AUTOMATIC mode
    HardwareSerial &port;
    unsigned int newBaudRate;
    bool mode;
    bool imuState;
    long serialTimeout;
    unsigned long long lastCall[4] = {0};

    void serialFlush();
    bool serialBreak(); //the timeout depends on serialTimeOut value
    bool readBytes(byte[], byte, char);
    void decodeData(byte[], byte, byte &, byte[]);
    bool readLeftStick();
    bool readRightStick();
    bool readButtons();
    bool readTriggers();
    void debug(String);

public:
    DualShock4(HardwareSerial &newPort = Serial1, long newBaudRate = 115200, bool newMode = MANUAL_UPDATE)
        : port(newPort)
    {
        mode = newMode;
        LX_val = LY_val = RX_val = RY_val = buttons = lastButtons = TRIG_L_val = TRIG_R_val = 0;
        sampleTime = 30;
        serialTimeout = 100;
        imuState = false;
        port.begin(newBaudRate);
    }

    void setSampleTime(unsigned int);
    void setImuState(bool);
    bool readGamepad();
    bool newButtonState();
    bool newButtonState(unsigned int);
    bool button(unsigned int);
    bool buttonPressed(unsigned int);
    bool buttonReleased(unsigned int);
    unsigned int axis(unsigned int);
    unsigned int trigger(unsigned int);
};