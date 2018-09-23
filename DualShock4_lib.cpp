#include <DualShock4_lib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#if ARDUINO > 22
    #include <Arduino.h>
#else
    #include <WProgram.h>
#endif

void DualShock4::serialFlush() {
    while(port->available())
        char ch = port->read();
}

bool DualShock4::serialBreak(){
    unsigned long long lastTime = millis();
    while(!port->available() && (millis() - lastTime) < serialTimeout);
    return (port->available() > 0);
}

bool DualShock4::readBytes(byte data[], int n, byte toSend) {
    serialFlush();
    port->write(toSend);
    for(int i = 0 ; i < n ; i++){
        if (!serialBreak())
            return false;
        else data[i] = port->read();
    }
}

void DualShock4::readLeftAxis() {
    if(mode == AUTOMATIC && (millis() - lastCall[0]) < sampleTime)
        return;
    byte data[4];
    lastCall[0] = millis();
    readBytes(data, 4, LX);
    LX_val = data[0] << 8 | data[1];
    LY_val = data[2] << 8 | data[3];
}

void DualShock4::readRightAxis() {
    if(mode == AUTOMATIC && (millis() - lastCall[1]) < sampleTime)
        return;
    lastCall[1] = millis();
    byte data[4];
    readBytes(data, 4, RX);
    RX_val = data[0] << 8 | data[1];
    RY_val = data[2] << 8 | data[3];
}

void DualShock4::readButtons() {
    if(mode == AUTOMATIC && (millis() - lastCall[2]) < sampleTime)
        return;
    lastCall[2] = millis();
    byte data[3];
    readBytes(data, 3, BUTTONS);
    lastButtons = buttons;
    buttons = 0;
    buttons = (data[0] << 16 | data[1] << 8) | data[2];
}

void DualShock4::readTriggers() {
    if(mode == AUTOMATIC && (millis() - lastCall[3]) < sampleTime)
        return;
    lastCall[3] = millis();
    byte data[4];
    readBytes(data, 4, RX);
    TRIG_L_val = data[0] << 8 | data[1];
    TRIG_R_val = data[2] << 8 | data[3];
}

void DualShock4::setSampleTime(unsigned int newSampleTime) {
    sampleTime = constrain(newSampleTime, 0, 500);
}

void DualShock4::setImuState(bool newState) {
    imuState = newState;
}

void DualShock4::readGamepad() {
    if(mode == AUTOMATIC)
        return;
    readLeftAxis();
    readRightAxis();
    readButtons();
    readTriggers();
}

bool DualShock4::newButtonState() {
    if(mode == AUTOMATIC)
        readButtons();
    return ((lastButtons ^ buttons) > 0);
}

bool DualShock4::newButtonState(unsigned int button) {
    if(mode == AUTOMATIC)
        readButtons();
    return (((lastButtons ^ buttons) >> button) & 1);
}

bool DualShock4::button(unsigned int button) {
    if(mode == AUTOMATIC)
        readButtons();
    return ((buttons >> button) & 1);
}
bool DualShock4::buttonPressed(unsigned int button) {
    if(mode == AUTOMATIC)
        readButtons();
    return (newButtonState(button) & this->button(button));
}

bool DualShock4::buttonReleased(unsigned int button) {
    if(mode == AUTOMATIC)
        readButtons();
    return (newButtonState(button) & !this->button(button));
 }

 int DualShock4::axis(unsigned int stick) {
    switch(stick) {
        case LX :
            if(mode == AUTOMATIC)
                readLeftAxis();
            return LX_val;
            break;
        case LY :
            if(mode == AUTOMATIC)
                readLeftAxis();
            return LY_val;
            break;
        case RX :
            if(mode == AUTOMATIC)
                readRightAxis();
            return RX_val;
            break;
        case RY :
            if(mode == AUTOMATIC)
                readRightAxis();
            return RY_val;
            break;
        default :
            return 0;
    }
 }

int DualShock4::trigger(unsigned int trig) {
    if(mode == AUTOMATIC)
        readTriggers();
    switch(trig) {
        case TRIG_L :
            return TRIG_L_val;
            break;
        case TRIG_R :
            return TRIG_R_val;
            break;
        default :
            return 0;
    }
 }