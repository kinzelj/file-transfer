#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
/******************************************************************************************************
 * error handler function receives message and passes to perror function to print a system error message 
******************************************************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
} // Error function used for reporting issue

/******************************************************************************************************
 * openSocket function
******************************************************************************************************/
int openSocket(char *host, char *port)
{
    int socketFD, portNumber;

    struct sockaddr_in serverAddress;
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct

    // Set up the server address struct
    portNumber = atoi(port);                    // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;         // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        error("ERROR opening socket");

    // Enable the socket to begin listening
    if (bind(socketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
    {
        error("ERROR on binding");
    }

    //set server to listen on socket
    if (listen(socketFD, 1) < 0)
    {
        error("ERROR on listen");
    }
    else
    {
        printf("Server now listening on port %d\n", portNumber);
    }

    return socketFD;
}

/******************************************************************************************************
 * recvNewConnection function
******************************************************************************************************/
int recvNewConnection(int socketFD)
{
    struct sockaddr_in clientAddress;
    socklen_t clientAddressSize;

    // Accept a connection, blocking if one is not available until one connects
    clientAddressSize = sizeof(clientAddress); // Get the size of the address for the client that will connect
    int connectionFD = accept(socketFD, (struct sockaddr *)&clientAddress, &clientAddressSize);
    if (connectionFD < 0)
    {
        error("ERROR on accept");
    }

    printf("SERVER: Connection made with %s:%d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
    return connectionFD;
}

/******************************************************************************************************
 * recvNewInput function
******************************************************************************************************/
void recvNewInput(int socketFD, char *input)
{
    recv(socketFD, input, sizeof(input) - 1, 0); // Read data from the socket, leaving \0 at end
}

int validateInput(char *input)
{
    char checkInput[256];
    strcpy(checkInput, input);
    checkInput[strcspn(checkInput, "\n")] = '\0'; // Remove the trailing \n
    checkInput[2] = '\0';

    printf("checkInput: %s\n", checkInput);
    printf("input: %s\n", input);

    if (strcmp(checkInput, "-l") == 0 || strcmp(checkInput, "-g") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void sendInputError(int socketFD)
{
    char *errorMessage = "Invalid input error, please try again";
    int charsWritten = send(socketFD, errorMessage, strlen(errorMessage), 0);
    if (charsWritten < 0)
        error("SERVER: ERROR writing to socket");
}

void handleRequest()
{
}

/******************************************************************************************************
 * Main function
******************************************************************************************************/
int main(int argc, char *argv[])
{
    int charsRead;
    char *host = "localhost";
    char *port = argv[1];

    //start server on local host with port defined by user
    int controlSocketFD = openSocket(host, port);

    while (1)
    {
        //establish control connection with client
        int newConnectionFD = recvNewConnection(controlSocketFD);
        //wait for input from client
        char input[256];
        memset(input, '\0', sizeof(input)); // Clear out the buffer
        recvNewInput(newConnectionFD, input);
        int inputValidated = validateInput(input);
        if (!inputValidated)
        {
            printf("Invalid input\n");
            sendInputError(newConnectionFD);
            continue;
        }
        else
        {
            handleRequest();
        }
    }
}