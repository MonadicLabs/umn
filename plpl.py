import socket
import sys

import time
usleep = lambda x: time.sleep(x/1000000.0)

sock = socket.socket(socket.AF_INET,
        socket.SOCK_DGRAM)     

UDP_IP = "127.0.0.1" #Target IP Address
UDP_PORT = 12345

#Create a socket    
def sendPacket(MESSAGE):
    sock.sendto(MESSAGE, (UDP_IP, UDP_PORT)) #Send message to UDP port

popo = 0
if __name__ == '__main__':
	while True:
		#           S0  S1  SY  VE  TY  FL SND 
		# papacket='\x00\x01\x55\x01\x00\x00\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x01\x00\x0A\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x12\x34'
		papacket = '\x00\x01\x55\x01\x01\x10\x00\x00\x00\x01\x00\x00\x00\x02\x7a\x3d\x00\x07\x00\x0a\x00\x00\xed\xed\x6e\xe7\xed\xd5\xaf\x99\x1f\xe7\x00\x00'		
		#papacket='ALLO'
		text = papacket[:3] + str(popo) + papacket[4:]
		print( text )
		popo = (popo + 1) % 256
		sendPacket(papacket)
		# sendPacket('STOP_SERVICES')
		# sendPacket('\x4e\x55\x00\x02\x04\x3f\x02\x01\x02\x30\x54')
		usleep(1)
		# break
