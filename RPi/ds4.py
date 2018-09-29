import os
import pygame
import pygame.display
from time import sleep
import threading
###################################################################################

analogStick = {"LX" : [0, 0.0], "LY" : [1, 0.0], "RX" : [2, 0.0], "RY" : [5, 0.0]}
triggers = {"TRIG_L" : [3, -1.0], "TRIG_R" : [4, -1.0]}
imuData = {"AX" : [7, 0.0], "AY" : [6, 0.0], "AZ" : [8, 0.0], "GX" : [11, 0.0], "GY" : [9, 0.0], "GZ" : [10, 0.0]}
buttons = {"TRIANGLE" : [3, 0], "CROSS" : [1, 0], "SQUARE" : [0, 0], "CIRCLE" : [2, 0], "L1" : [4, 0], "L2" : [6, 0], "R1" : [5, 0],
			"R2" : [7, 0], "LB" : [10, 0], "RB" : [11, 0], "SHARE" : [8, 0], "OPTIONS" : [9, 0], "PS" : [12, 0], "TOUCHPAD" : [13, 0]}
hatButtons = {"UP" : 0, "DOWN" : 0, "LEFT" : 0, "RIGHT" : 0}

############################### Initialize ##########################################

pygame.init()
os.putenv('DISPLAY', ':0.0') 
pygame.display.set_mode((1, 1)) 
pygame.display.init()
pygame.joystick.init()
if pygame.joystick.get_count() == 0:
	print("No Controllers Found")
	print("Waiting...")
	while pygame.joystick.get_count() == 0:
		pygame.joystick.quit()
		pygame.joystick.init()
controller_name = 'Sony Computer Entertainment Wireless Controller'
notFound = True
lastControllerCount = pygame.joystick.get_count()
while notFound:
	for i in range(lastControllerCount):
		ds4 = pygame.joystick.Joystick(i)
		ds4.init()
		if not(ds4.get_name() == controller_name):
			print("Controller %d: "%(i+1), ds4.get_name())
		else:
			notFound = False
			break
	if notFound:
		print("Right Contoller not Found")
		print("Waiting for any new Controller...")
		while(pygame.joystick.get_count() == lastControllerCount):
			pygame.joystick.quit()
			pygame.joystick.init()
		lastControllerCount = pygame.joystick.get_count()

print("Sony DualShock 4 Connected")
sleep(.3) 

######################################################################################


def grab_data(device, dataHandler):
	for key in device.keys():
				device[key][1] = dataHandler( device[key][0] )
 
def calc_hat(dict0):
	tup = ds4.get_hat(0)
	dict0["UP"] = tup[1]
	dict0["DOWN"] = tup[1] * -1
	dict0["LEFT"] = tup[0] * -1
	dict0["RIGHT"] = tup[0]
	for key in dict0.keys():
		if dict0[key] < 0:
			dict0[key] = 0

def read_ds4(externalDataSet = False, dataSet = []):
	if externalDataSet:
		dataSet.extend((analogStick, triggers, imuData, buttons, hatButtons))
	while(True):
		event = len(pygame.event.get())
		if  event > 1:
			grab_data(analogStick, ds4.get_axis)
			grab_data(imuData, ds4.get_axis)
			grab_data(triggers, ds4.get_axis)
			grab_data(buttons, ds4.get_button)
			calc_hat(hatButtons)
			if externalDataSet:
				dataSet[0] = analogStick
				dataSet[1] = triggers
				dataSet[2] = imuData
				dataSet[3] = buttons
				dataSet[4] = hatButtons

def print_data(device, hat = False):
	for key in device.keys():
		if hat:
			val = device[key]
		else:
			val = device[key][1]
		if isinstance(val, float):
			print("%.3f   " % val, end='')
		elif isinstance(val, int):
			print("%d  " % val, end='')

def print_all():
	while True:
		print_data(analogStick)
		print_data(triggers)
		print_data(hatButtons, True)
		print_data(buttons)
		print_data(imuData)
		print()

def Main():
	Thread1 = threading.Thread(target=read_ds4)
	Thread2 = threading.Thread(target=print_all)
	Thread1.start()
	Thread2.start()

grab_data(triggers, ds4.get_axis)
if triggers["TRIG_L"][1] != -1.0 or triggers["TRIG_R"][1] != -1.0:
	print("Press Left and Right Triggers once to start.")
	while(not(triggers["TRIG_L"][1] == -1.0 and triggers["TRIG_R"][1] == -1.0)):
		event = len(pygame.event.get())
		if  event > 1:
			grab_data(triggers, ds4.get_axis)

if __name__ == "__main__":
	Main()