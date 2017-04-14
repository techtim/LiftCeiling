import RPi.GPIO as GPIO
import time
import socket
import time

UDP_IP = "192.168.2.10"
UDP_PORT = 1313


GPIO.setmode(GPIO.BCM)

GPIO.setup(17, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

GPIO.setup(18, GPIO.IN, pull_up_down = GPIO.PUD_DOWN) #PUD_UP

GPIO.setup(22, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

GPIO.setup(27, GPIO.IN, pull_up_down = GPIO.PUD_DOWN)

MESSAGE = "0"

while True:
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	

	if(GPIO.input(17) ==1):
		# print("UP Button 17 pressed")
		MESSAGE = "2"

	elif(GPIO.input(18) == 1):
		# print("DOWN Button 18 pressed")
		MESSAGE = "1"
	else :
		MESSAGE = "0"
	
	if(GPIO.input(27) ==1):
		# print("RIDING Button 27 pressed")
		MESSAGE += ",3"
	
	if(GPIO.input(22) ==1):
		# print("CONTENT Button 27 pressed")
		MESSAGE += ",4"
	if (!sock.herror && !sock.gaierror):
		sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))

	time.sleep(.1)

GPIO.cleanup()