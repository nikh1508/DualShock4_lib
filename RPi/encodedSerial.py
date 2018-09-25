import serial

arduino = serial.Serial('/dev/ttyAMA0', 115200)

startMarker = 254
endMarker = 255
specialByte = 253

def encodeToString(data):
    global startMarker, specialByte, specialByte
    outString = ""
    outString = outString + chr(startMarker)
    for val in data:
        if val >= specialByte:
            outString = outString + chr(specialByte)
            outString = outString + chr(val - specialByte)
        else:
            outString = outString + chr(val)
    outString = outString + chr(endMarker)
    return outString

def sendToArduino(toSend):
    arduino.write(encodeToString(toSend).encode('latin_1'))

