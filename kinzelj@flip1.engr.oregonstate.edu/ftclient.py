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
def sendMessage(socket, message, host, port):
    socket.sendto(message.encode(),(host, port))
    return

#*************************************************************************************************
# 
#*************************************************************************************************
def recvResponse(socket, bufferSize):
    returnMessage = socket.recv(bufferSize)
    return returnMessage.decode()

#*************************************************************************************************
# 
#*************************************************************************************************
def openSocket(host, port):
    newSocket = socket(AF_INET, SOCK_STREAM)
    newSocket.bind((host, port))
    newSocket.listen(1)
    print ("Ready to receive on port {}".format(port))
    return newSocket

#*************************************************************************************************
#
#*************************************************************************************************
def recvFiles(socket, bufferSize, host, port):
    print("Server directory files:")
    done = 0
    connectionSocket, addr = socket.accept()
    while (done != 1):
        recvMessage = connectionSocket.recv(bufferSize)
        if (recvMessage.decode() != "done"):
            print(recvMessage.decode())
            returnMessage = "next file"
            sendMessage(connectionSocket, returnMessage, host, port)
            # sendMessage(connectionSocket, returnMessage)
        else:
            done = 1
    connectionSocket.close() 
    
#*************************************************************************************************
# 
#*************************************************************************************************   
def importTextFile(fileName, socket, bufferSize, host, port):
    connectionSocket, addr = socket.accept()
    fileSize = connectionSocket.recv(bufferSize)

    if (fileSize.decode() == "file not found"):
        print("Error: File not found on server")
        connectionSocket.close()
        return

    fileSizeInt = int(fileSize.decode())
    returnMessage = "file size received"
    sendMessage(connectionSocket, returnMessage, host, port)
    
    #receive file contents from server
    file_content = ""
    charsRead = 0
    while(charsRead < fileSizeInt):
        buffer_message = connectionSocket.recv(bufferSize) 
        file_content += buffer_message.decode('utf-8', 'ignore')
        charsRead += len(buffer_message)
    
    #send file acknowledgement back to server
    returnMessage = "file received"
    sendMessage(connectionSocket, returnMessage, host, port)
    
    #check if file exists, sorce: https://linuxize.com/post/python-check-if-file-exists/
    try:
        f = open(fileName)
        f.close()
        overwrite = input(fileName + " already exists, do you want to overwrite? (y/n): ")
        if(overwrite == "y"):
            file = open(fileName, "w")
            file.write(file_content)
            file.close()
            print(fileName + " imported to local directory.")

    except IOError:
        #if file does not exist create and write contents from imported file
        file = open(fileName, "w")
        file.write(file_content)
        file.close()
        print(fileName + " imported to local directory.")

    connectionSocket.close()

#*************************************************************************************************
# 
#*************************************************************************************************
def handleRequest(cmd, fileName, controlConnection, sendRequest, serverHost, serverPort, bufferSize):
    sendMessage(controlConnection, sendRequest, serverHost, serverPort)
    response = recvResponse(controlConnection, bufferSize)
    if(response == "connection accepted"):
        dataServerSocket = openSocket("localhost", int(dataPort))
        if (dataServerSocket):
            sendMessage(controlConnection, "data socket open", serverHost, serverPort)
            if(cmd == "-l"):
                recvFiles(dataServerSocket, bufferSize, serverHost, serverPort)
            if(cmd == "-g"):
                importTextFile(fileName, dataServerSocket, bufferSize, serverHost, serverPort)
        else:
            print("Error: Unable to create data socket")
    else:
        print(response)

#*************************************************************************************************
#   Main program body
#*************************************************************************************************
BUFFER = 1024
if (len(sys.argv) >= 5):
    if(sys.argv[1] == "flip1"):
        severHost = "flip1.engr.oregonstate.edu"
    elif(sys.argv[1] == "flip2"):
        severHost = "flip2.engr.oregonstate.edu"
    elif(sys.argv[1] == "flip3"):
        severHost = "flip3.engr.oregonstate.edu"
    else:
        serverHost = sys.argv[1]

    serverPort = int(sys.argv[2])
    fileName = ""    

    #establish control connection with server
    controlConnection = initiateContact(serverHost, serverPort)
    #call makeRequest function to send command to server
    if (controlConnection):
        command = sys.argv[3]
        dataPort = ""
        fileName = ""
        sendRequest = "empty"
        if (sys.argv[3] == '-l'):
            dataPort = sys.argv[4]
            sendRequest = command + str(len(dataPort)) + "-" + dataPort
        if (sys.argv[3] == '-g'):
            fileName = sys.argv[4]
            dataPort = sys.argv[5]
            sendRequest = command + str(len(fileName)) + "-" + fileName + str(len(dataPort)) + "-" + dataPort
            
        handleRequest(sys.argv[3], fileName, controlConnection, sendRequest, serverHost, serverPort, BUFFER)

    controlConnection.close()

else:
    print("Error: Invalid program inputs.")





