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
    print("sent TCP connection request to: {}:{}".format(serverHost, serverPort))
    return clientSocket

#*************************************************************************************************
# 
#*************************************************************************************************
def makeRequest(socket, message, serverHost, serverPort):
    socket.sendto(message.encode(),(serverHost, serverPort))
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
if (len(sys.argv) >= 5):
    print(sys.argv)
    serverHost = sys.argv[1]
    serverPort = int(sys.argv[2])
    
    #establish control connection with server
    controlConnection = initiateContact(serverHost, serverPort)
    #call makeRequest function to send command to server
    if (controlConnection):
        command = sys.argv[3]
        dataPort = ""
        fileName = ""
        sendRequest = ""
        if (sys.argv[3] == '-g'):
            fileName = sys.argv[4]
            dataPort = sys.argv[5]
            sendRequest = command + str(len(fileName)) + "-" + fileName + str(len(dataPort)) + "-" + dataPort
        else:
            dataPort = sys.argv[4]
            sendRequest = command + str(len(dataPort)) + "-" + dataPort
            
        print(sendRequest)
            
        makeRequest(controlConnection, sendRequest, serverHost, serverPort)
        response = recvResponse(controlConnection)
        print(response)
else:
    print("Error: Invalid program inputs.")






