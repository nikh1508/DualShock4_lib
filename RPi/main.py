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
        if x == b'i':
            val = 0
            val = (dataSet[4]["UP"] << 1) | dataSet[4]["DOWN"]
            toSend.append(val)
            val = (dataSet[4]["LEFT"] << 7) | (dataSet[4]["RIGHT"] << 6) |(dataSet[3]["SHARE"][1] << 5) | (dataSet[3]["TOUCHPAD"][1] << 4) | (dataSet[3]["OPTION"][1] << 3) | (dataSet[3]["PS"][1] << 2) | (dataSet[3]["LB"][1] << 1) | dataSet[3]["RB"][1]
            toSend.append(val)
            val = (dataSet[3]["TRIANGLE"][1] << 7) | (dataSet[3]["CROSS"][1] << 6) |(dataSet[3]["SQUARE"][1] << 5) | (dataSet[3]["CIRCLE"][1] << 4) | (dataSet[3]["L1"][1] << 3) | (dataSet[3]["L2"][1] << 2) | (dataSet[3]["R1"][1] << 1) | dataSet[3]["R2"][1]
            toSend.append(val)
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