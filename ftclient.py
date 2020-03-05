#*************************************************************************************************
#   ftclient.py 
#   Programmed by: Josh Kinzel
#   CS372 - Project 2
#   Last Modified: 2020-02-27
#   Resources Sited: Python Internet Protocols and Support: 
#       https://docs.python.org/release/2.6.5/library/internet.html
#   Description: Client side program
#*************************************************************************************************
from socket import *
import sys
import signal
#*************************************************************************************************
#  initiateContact function attempt to creates a TCP connection with a server host 
#*************************************************************************************************
def initiateContact(serverHost, serverPort):
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((serverHost, serverPort))
    print("sent TCP connection request to: {}:{}").format(serverHost, serverPort)
    return clientSocket

#*************************************************************************************************
# 
#*************************************************************************************************
def makeRequest(socket, command):
    socket.send(command)
    return

#*************************************************************************************************
# 
#*************************************************************************************************
def recvResponse(socket):
    returnMessage = socket.recv(1024)
    return returnMessage


#*************************************************************************************************
#   Main program body
#*************************************************************************************************
serverHost = sys.argv[1]
serverPort = int(sys.argv[2])
command = sys.argv[3]
dataPort = ""
fileName = ""
if (sys.argv[3] == '-g'):
    fileName = sys.argv[4]
    dataPort = sys.argv[5]
else:
    dataPort = sys.argv[4]
sendRequest = dataPort + command + fileName

#establish control connection with server
controlConnection = initiateContact(serverHost, serverPort)
#call makeRequest function to send command to server
if (controlConnection):
    makeRequest(controlConnection, sendRequest)
    response = recvResponse(controlConnection)
    print(response)







