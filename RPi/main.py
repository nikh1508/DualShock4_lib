import ds4
import multiprocessing
from encodedSerial import sendToArduino, arduino
from time import sleep

manager = multiprocessing.Manager()
Process = multiprocessing.Process
dataSet = manager.list()

def map(val, low_in, high_in, low_op, high_op):
	return (((val-low_in)/(high_in - low_in)) * (high_op - low_op)) + low_op


def sendData():
    while(len(dataSet) == 0):
        pass
    while(True):
        x = arduino.read()
        toSend = []
        if x == b'e':
            switcher = 1.0
            for axis in ("LX", "LY"):
                val = int(map(dataSet[0][axis][1], -1.0 * switcher, 1.0 * switcher, 0, 65535))
                switcher = switcher * -1.0
                toSend.append(val >> 8 & 0b11111111)
                toSend.append(val & 0b11111111)
            sendToArduino(toSend)
        
        elif x == b'g':
            switcher = 1.0
            for axis in ("RX", "RY"):
                val = int(map(dataSet[0][axis][1], -1.0 * switcher, 1.0 * switcher, 0, 65535))
                switcher = switcher * -1.0
                toSend.append(val >> 8 & 0b11111111)
                toSend.append(val & 0b11111111)
            sendToArduino(toSend)

        elif x == b'i':
            val = 0
            val = (dataSet[4]["UP"] << 1) | dataSet[4]["DOWN"]
            toSend.append(val)
            val = (dataSet[4]["LEFT"] << 7) | (dataSet[4]["RIGHT"] << 6) |(dataSet[3]["SHARE"][1] << 5) | (dataSet[3]["TOUCHPAD"][1] << 4) | (dataSet[3]["OPTIONS"][1] << 3) | (dataSet[3]["PS"][1] << 2) | (dataSet[3]["LB"][1] << 1) | dataSet[3]["RB"][1]
            toSend.append(val)
            val = (dataSet[3]["TRIANGLE"][1] << 7) | (dataSet[3]["CROSS"][1] << 6) |(dataSet[3]["SQUARE"][1] << 5) | (dataSet[3]["CIRCLE"][1] << 4) | (dataSet[3]["L1"][1] << 3) | (dataSet[3]["L2"][1] << 2) | (dataSet[3]["R1"][1] << 1) | dataSet[3]["R2"][1]
            toSend.append(val)
            sendToArduino(toSend)
        
        elif x == b'j':
            for trig in ("TRIG_L", "TRIG_R"):   
                val = int(map(dataSet[1][trig][1], -1.0, 1.0, 0, 65535))
                toSend.append(val >> 8 & 0b11111111)
                toSend.append(val & 0b11111111)
            sendToArduino(toSend)

def Main():
	process1 = Process(target=ds4.read_ds4, args=(True, dataSet))
	process2 = Process(target=sendData, args = ())
	process1.start()
	process2.start()
	process1.join()
	process2.join()
	print("Strated sending data")

if __name__ == "__main__":
	Main()