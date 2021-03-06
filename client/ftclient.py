#*************************************************************************************************
#   ftclient.py 
#   Programmed by: Josh Kinzel
#   CS372 - Project 2
#   Last Modified: 2020-03-08
#   Resources Sited: Python Internet Protocols and Support: 
#       https://docs.python.org/release/2.6.5/library/internet.html
#   Description: Client side  program connects with ftserver and request either the list of files
#                in the servers local directory, or export the contents from one of the files.
#*************************************************************************************************
from socket import *
import sys
import signal

#*************************************************************************************************
# initiateContact function attempt to creates a TCP connection with a server host 
# Function returns the socket file descriptor
#*************************************************************************************************
def initiateContact(serverHost, serverPort):
    host = gethostbyname(serverHost)
    clientSocket = socket(AF_INET, SOCK_STREAM)
    clientSocket.connect((host, serverPort))
    print("Sent connection request to: {}:{}".format(serverHost, serverPort))
    return clientSocket

#*************************************************************************************************
# sendMessage function sends message data to connected server socket 
#*************************************************************************************************
def sendMessage(socket, message, host, port):
    socket.sendto(message.encode(),(host, port))
    return

#*************************************************************************************************
# recvResponse function waits on connected socket for return message from server  
# Function return the decoded message string
#*************************************************************************************************
def recvResponse(socket, bufferSize):
    returnMessage = socket.recv(bufferSize)
    return returnMessage.decode()

#*************************************************************************************************
# openSocket establishes a server socket to receive message from data connection from server
# side program. This is opened after the connection is made and the request is processed by the
# server.
#*************************************************************************************************
def openSocket(port):
    host = ''
    newSocket = socket(AF_INET, SOCK_STREAM)
    newSocket.bind((host, port))
    newSocket.listen(1)
    print ("Ready to receive on port {}".format(port))
    return newSocket

#*************************************************************************************************
# recvFiles function processes the list files request (-l) by the client. 
# Function will receive a file name from the server one at a time until all file names have been
# receive. 
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
        else:
            done = 1
    connectionSocket.close() 
    
#*************************************************************************************************
# importTextFile function processes the get file contents request (-g) by the client.
# Function will continue to open TCP connections until the complete file contents have been received.
# The function will check if the file name already exists in the current directory then prompt the
# user to overwrite the file if one exists.
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
    
    #check if file exists, resorce: https://linuxize.com/post/python-check-if-file-exists/
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
# handleRequest function processes the client input.
# If the connection is accepted by the server, the function will call either the recvFiles or 
# importTextFile functions depending on the input command. 
#*************************************************************************************************
def handleRequest(cmd, fileName, controlConnection, sendRequest, serverHost, serverPort, bufferSize):
    #send user command to server to determine if request is valid
    sendMessage(controlConnection, sendRequest, serverHost, serverPort)
    response = recvResponse(controlConnection, bufferSize)

    #if valid user input, open data socket to receive incoming transmission
    if(response == "connection accepted"):
        dataServerSocket = openSocket(int(dataPort))
        if (dataServerSocket):
            sendMessage(controlConnection, "data socket open", serverHost, serverPort)
            #if list files command, call recvFiles function
            if(cmd == "-l"):
                print("Requesting directory files from server...")
                recvFiles(dataServerSocket, bufferSize, serverHost, serverPort)
            #if export file contents command, call importTextFile function
            if(cmd == "-g"):
                print("Requesting file transfer from server...")
                importTextFile(fileName, dataServerSocket, bufferSize, serverHost, serverPort)
        else:
            print("Error: Unable to create data socket")
    else:
        print(response)

#*************************************************************************************************
# Main program body
#*************************************************************************************************
BUFFER = 1024
if (len(sys.argv) >= 5):
    #determine server host name and port from user input
    serverHost = ""
    if(sys.argv[1] == "flip1"):
        serverHost = "flip1.engr.oregonstate.edu"
    elif(sys.argv[1] == "flip2"):
        serverHost = "flip2.engr.oregonstate.edu"
    elif(sys.argv[1] == "flip3"):
        serverHost = "flip3.engr.oregonstate.edu"
    else:
        serverHost = sys.argv[1]
    serverPort = int(sys.argv[2])

    #establish control connection with server
    controlConnection = initiateContact(serverHost, serverPort)

    #define request command and variables to send to server, then handle user request
    fileName = ""    
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

        #call handleRequest function to send request to server and handle response    
        handleRequest(sys.argv[3], fileName, controlConnection, sendRequest, serverHost, serverPort, BUFFER)

    controlConnection.close()

else:
    print("Error: Invalid program inputs.")






