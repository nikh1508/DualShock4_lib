#ifndef DualShock4_h
    #define DualShock4_h
#endif

#if ARDUINO > 22
    #include <Arduino.h>
#else
    #include <WProgram.h>
#endif

#include <HardwareSerial.h>

#define MANUAL false
#define AUTOMATIC true


class DualShock4 {
    int LX_val;
    int LY_val;
    int RX_val;
    int RY_val;
    unsigned long buttons;
    unsigned long lastButtons;
    int TRIG_L_val;
    int TRIG_R_val;
    unsigned int sampleTime;                            //usable only in AUTOMATIC mode
    HardwareSerial *port;
    unsigned int newBaudRate;
    bool mode;
    bool imuState;
    long serialTimeout;
    enum devices {LX = 101, LY, RX, RY, BUTTONS, TRIG_L, TRIG_R};
    enum buttons_list {R2 = 0, R1, L2, L1, CIRCLE, SQUARE, CROSS, TRIANGLE, RB, LB, PS, OPTIONS, TOUCHPAD, SHARE, RIGHT, LEFT, DOWN, UP};
    unsigned long long lastCall[4] = {0};

    void serialFlush();
    bool serialBreak();                                 //the timeout depends on serialTimeOut value
    bool readBytes(byte[], int, byte);
    void readLeftAxis();
    void readRightAxis();
    void readButtons();
    void readTriggers();

    public:

    DualShock4(HardwareSerial newPort = Serial1, int newBaudRate = 115200, bool newMode = MANUAL) {
        mode = newMode;
        port = &newPort;
        LX_val = LY_val = RX_val = RY_val = buttons = lastButtons = TRIG_L_val = TRIG_R_val = 0;
        sampleTime = 30;
        serialTimeout = 100;
        imuState = false;
        port->begin(newBaudRate);
    }

    void setSampleTime(unsigned int);
    void setImuState(bool);
    void readGamepad();
    bool newButtonState();
    bool newButtonState(unsigned int);
    bool button(unsigned int);
    bool buttonPressed(unsigned int);
    bool buttonReleased(unsigned int);
    int axis(unsigned int);
    int trigger(unsigned int);
};