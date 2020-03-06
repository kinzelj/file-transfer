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
    print("Sent connection request to: {}:{}".format(serverHost, serverPort))
    return clientSocket

#*************************************************************************************************
# 
#*************************************************************************************************
def sendMessage(socket, message):
    socket.send(message)
    return

#*************************************************************************************************
# 
#*************************************************************************************************
def recvResponse(socket, bufferSize):
    returnMessage = socket.recv(bufferSize)
    return returnMessage

#*************************************************************************************************
# 
#*************************************************************************************************
def openSocket(host, port):
    newSocket = socket(AF_INET, SOCK_STREAM)
    newSocket.bind((host, port))
    newSocket.listen(1)
    print ("Ready to receive on port {}").format(port)
    return newSocket

#*************************************************************************************************
#
#*************************************************************************************************
def recvFiles(socket, bufferSize, host, port):
    print("Server directory files:")
    done = 0
    while (done != 1):
        connectionSocket, addr = socket.accept()
        recvMessage = connectionSocket.recv(bufferSize)
        if (recvMessage != "done"):
            print(recvMessage)
            returnMessage = "next file"
            # sendMessage(connectionSocket, returnMessage, host, port)
            sendMessage(connectionSocket, returnMessage)
        else:
            done = 1

#*************************************************************************************************
# 
#*************************************************************************************************
def getServerFiles(controlConnection, sendRequest, serverHost, serverPort):
    # sendMessage(controlConnection, sendRequest, serverHost, serverPort)
    sendMessage(controlConnection, sendRequest)
    response = recvResponse(controlConnection, BUFFER)
    if(response == "connection accepted"):
        dataServerSocket = openSocket("localhost", int(dataPort))
        if (dataServerSocket):
            # sendMessage(controlConnection, "data socket open", serverHost, serverPort)
            sendMessage(controlConnection, "data socket open")
            recvFiles(dataServerSocket, BUFFER, serverHost, serverPort)
        else:
            print("Error: Unable to create data socket")


#*************************************************************************************************
#   Main program body
#*************************************************************************************************
BUFFER = 1024
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
        if (sys.argv[3] == '-l'):
            dataPort = sys.argv[4]
            sendRequest = command + str(len(dataPort)) + "-" + dataPort
            getServerFiles(controlConnection, sendRequest, serverHost, serverPort)
        if (sys.argv[3] == '-g'):
            fileName = sys.argv[4]
            dataPort = sys.argv[5]
            sendRequest = command + str(len(fileName)) + "-" + fileName + str(len(dataPort)) + "-" + dataPort

            

else:
    print("Error: Invalid program inputs.")






