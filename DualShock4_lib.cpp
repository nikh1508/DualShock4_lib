#include <DualShock4_lib.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#if ARDUINO > 22
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <PID_v1.h>

void DualShock4::serialFlush()
{
    while (port.available() > 0)
        char ch = port.read();
}

bool DualShock4::serialBreak()
{
    unsigned long long lastTime = millis();
    while (!port.available() && ((millis() - lastTime) < serialTimeout))
        ;
    return (port.available() > 0);
}

void DualShock4::decodeData(byte toDecode[], byte totalRecvd, byte &decodedByteCount, byte decodedBytes[])
{
    for (int i = 1; i < (totalRecvd - 1); i++)
    {
        byte x = toDecode[i];
        if (x == specialByte)
            x += toDecode[++i];
        decodedBytes[decodedByteCount++] = x;
    }
}

bool DualShock4::readBytes(byte data[], byte toRead, char toSend)
{
    static bool firstCall = true;
    static bool inProgress = false;
    static byte bytesRecvd = 0;
    static byte tempBuffer[20];
    static long long lastRead;
    if (firstCall)
    {
        firstCall = false;
        serialFlush();
        port.print(toSend);
        lastRead = millis();
    }

    while (port.available())
    {
        byte x = port.read();
        lastRead = millis();
        if (x == startMarker)
        {
            inProgress = true;
            bytesRecvd = 0;
        }
        if (inProgress)
            tempBuffer[bytesRecvd++] = x;
        if (x == endMarker)
        {
            inProgress = false;
            firstCall = true;
            byte decodedByteCount = 0;
            decodeData(tempBuffer, bytesRecvd, decodedByteCount, data);
            if (decodedByteCount == toRead)
                return true;
            else
                debug("Received Incorrect Bytes in :: " + String(int(toSend)));
        }
    }
    if ((millis() - lastRead) > 50)
    {
        debug("SERIAL TIMEOUT");
        firstCall = true;
    }
    return false;
}

bool DualShock4::readLeftStick()
{
    // if(mode == AUTOMATIC_UPDATE && (millis() - lastCall[0]) < sampleTime)
    //     return;
    lastCall[0] = millis();
    byte data[4];
    if (readBytes(data, 4, LX))
    {
        LX_val = data[0] << 8 | data[1];
        LY_val = data[2] << 8 | data[3];
        return true;
    }
    return false;
}

bool DualShock4::readRightStick()
{
    // if(mode == AUTOMATIC_UPDATE && (millis() - lastCall[1]) < sampleTime)
    //     return;
    // lastCall[1] = millis();
    byte data[4];
    if (readBytes(data, 4, RX))
    {
        RX_val = data[0] << 8 | data[1];
        RY_val = data[2] << 8 | data[3];
        return true;
    }
    return false;
}

bool DualShock4::readButtons()
{
    // if(mode == AUTOMATIC_UPDATE && (millis() - lastCall[2]) < sampleTime)
    //     return;
    // lastCall[2] = millis();
    byte data[3];
    if (readBytes(data, 3, BUTTONS))
    {
        lastButtons = buttons;
        buttons = 0;
        buttons = (long)data[0] << 16 | (long)data[1] << 8 | data[2];
        return true;
    }
    return false;
}

bool DualShock4::readTriggers()
{
    // if(mode == AUTOMATIC_UPDATE && (millis() - lastCall[3]) < sampleTime)
    //     return;
    // lastCall[3] = millis();
    byte data[4];
    if (readBytes(data, 4, TRIG_L))
    {
        TRIG_L_val = data[0] << 8 | data[1];
        TRIG_R_val = data[2] << 8 | data[3];
        return true;
    }
    return false;
}

void DualShock4::setSampleTime(unsigned int newSampleTime)
{
    sampleTime = constrain(newSampleTime, 0, 500);
}

void DualShock4::setImuState(bool newState)
{
    imuState = newState;
}

bool DualShock4::readGamepad()
{
    static bool leftStick = true;
    static bool rightStick = false;
    static bool buttons = false;
    static bool triggers = false;
    bool transmissionCompleted = false;
    bool newData = false;
    if (leftStick)
    {
        transmissionCompleted = readLeftStick();
        if (transmissionCompleted)
        {
            leftStick = false;
            rightStick = true;
        }
    }
    if (rightStick)
    {
        transmissionCompleted = readRightStick();
        if (transmissionCompleted)
        {
            rightStick = false;
            buttons = true;
        }
    }
    if (buttons)
    {
        transmissionCompleted = readButtons();
        if (transmissionCompleted)
        {
            buttons = false;
            triggers = true;
        }
    }
    if (triggers)
    {
        transmissionCompleted = readTriggers();
        if (transmissionCompleted)
        {
            newData = true;
            triggers = false;
            leftStick = true;
        }
    }
    return newData;
}

bool DualShock4::newButtonState()
{
    if (mode == AUTOMATIC_UPDATE)
        readButtons();
    return ((lastButtons ^ buttons) > 0);
}

bool DualShock4::newButtonState(unsigned int button)
{
    if (mode == AUTOMATIC_UPDATE)
        readButtons();
    return (((lastButtons ^ buttons) >> button) & 1);
}

bool DualShock4::button(unsigned int button)
{
    if (mode == AUTOMATIC_UPDATE)
        readButtons();
    return ((buttons >> button) & 1);
}
bool DualShock4::buttonPressed(unsigned int button)
{
    if (mode == AUTOMATIC_UPDATE)
        readButtons();
    return (newButtonState(button) & this->button(button));
}

bool DualShock4::buttonReleased(unsigned int button)
{
    if (mode == AUTOMATIC_UPDATE)
        readButtons();
    return (newButtonState(button) & !this->button(button));
}

unsigned int DualShock4::axis(unsigned int stick)
{
    switch (stick)
    {
    case LX:
        if (mode == AUTOMATIC_UPDATE)
            readLeftStick();
        return LX_val;
        break;
    case LY:
        if (mode == AUTOMATIC_UPDATE)
            readLeftStick();
        return LY_val;
        break;
    case RX:
        if (mode == AUTOMATIC_UPDATE)
            readRightStick();
        return RX_val;
        break;
    case RY:
        if (mode == AUTOMATIC_UPDATE)
            readRightStick();
        return RY_val;
        break;
    default:
        return 0;
    }
}

unsigned int DualShock4::trigger(unsigned int trig)
{
    if (mode == AUTOMATIC_UPDATE)
        readTriggers();
    switch (trig)
    {
    case TRIG_L:
        return TRIG_L_val;
        break;
    case TRIG_R:
        return TRIG_R_val;
        break;
    default:
        return 0;
    }
}

void DualShock4::debug(String errorMessage)
{
#ifdef DualShock4_lib_DEBUG
    Serial.println(errorMessage);
#endif
}