/*************************************************************************************************
  ftserver.c 
  Programmed by: Josh Kinzel
  CS372 - Project 2
  Last Modified: 2020-03-08
  Resources Sited: Beej's Guide to Network Programming: https://beej.us/guide/bgnet/html/ 
  Description: Server side file transfer program that provides access to files in local directory.
*************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define h_addr h_addr_list[0]

/******************************************************************************************************
 * error handler function receives message and passes to perror function to print a system error message 
******************************************************************************************************/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}
/******************************************************************************************************
 * get_ip_str function determines the ip address of the sockaddr passed to function.
 * Function returns a char * to the ip string
 * source: https://beej.us/guide/bgnet/html/#cb112-15 
******************************************************************************************************/
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }

    return s;
}
/******************************************************************************************************
 * countFile function determines the character count of the text file passed as its input.
 * Function returns an int of the file count
******************************************************************************************************/
int countFile(char * file_name)
{
    //get number of chars in file name
    int file_name_length = strlen(file_name);
    
    //define file path
    char file_open[file_name_length + 1];
    memset(file_open, '\0', sizeof(file_open));

    char current_dir[3] = "./";
    current_dir[3] = '\0';

    strcpy(file_open, current_dir);
    strcat(file_open, file_name);

    //open rooms file and import room info
    int file_descriptor = open(file_open, O_RDONLY);
    if (file_descriptor < 0)
    {
        return -1;
    }

    int num_chars = 0; 
    char readChar[2];
    memset(readChar, '\0', sizeof(readChar));
    int chars_read = read(file_descriptor, &readChar, 1);
    //read and concatanate until end of file
    while(chars_read)
    {
        num_chars++;
        chars_read = read(file_descriptor, &readChar, 1);
    }

    return num_chars;
}

/******************************************************************************************************
 * readFile function reads the contents of the file passed to the function then stores
 * its contents in the return_contents parameter.
******************************************************************************************************/
void readFile(char * file_name, char * return_contents)
{
    //get number of chars in file name
    int file_name_length = strlen(file_name);
    
    //define file path
    char file_open[file_name_length + 1];
    memset(file_open, '\0', sizeof(file_open));

    char current_dir[3] = "./";
    current_dir[3] = '\0';

    strcpy(file_open, current_dir);
    strcat(file_open, file_name);

    //open rooms file and import room info
    int file_descriptor = open(file_open, O_RDONLY);
    if (file_descriptor < 0)
    {
        printf("Could not open %s\n", file_open);
        exit(1);
    }

    size_t file_capacity = 100;
    char * file_contents = calloc(file_capacity, sizeof(char));
    // memset(file_contents, '\0', sizeof(file_contents));
    int num_chars = 0; 

    char readChar[2];
    memset(readChar, '\0', sizeof(readChar));
    int chars_read = read(file_descriptor, &readChar, 1);
    //read and concatanate until end of file 
    while(chars_read)
    {
        if(num_chars == (file_capacity - 2))
        {
            char * temp = calloc(file_capacity, sizeof(char));
            // memset(temp, '\0', sizeof(temp));
            memcpy(temp, file_contents, strlen(file_contents));
            free(file_contents);

            size_t new_capacity = file_capacity * 2;
            file_contents = calloc(new_capacity, sizeof(char));
            // memset(file_contents, '\0', sizeof(file_contents));
            memcpy(file_contents, temp, strlen(temp));

            free(temp);
            file_capacity = new_capacity;
            // file_contents = realloc(file_contents, file_capacity * sizeof(char));
        }
        strcat(file_contents, readChar);
        num_chars++;
        chars_read = read(file_descriptor, &readChar, 1);
    }

    strcpy(return_contents, file_contents);
    free(file_contents);
}

/******************************************************************************************************
 * openSocket function creates a server side socket to listen for incoming TCP connections
******************************************************************************************************/
int openSocket(char *host, char *port)
{
    int socketFD, portNumber;
    struct sockaddr_in serverAddress;

    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0)
        error("ERROR creating socket");

    // Set up the address struct
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(port);                                     // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET;                          // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber);                  // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddress.sin_zero, '\0', sizeof serverAddress);

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
 * initiateContact function establishes TCP connection with a client socket.
 * Input parameters are the connecting clients host url and port number
 * Function returns integer value of socket file descriptor
******************************************************************************************************/
int initiateContact(char *host, char *port)
{
    struct addrinfo clientAddr, *res;
    int socketFD;

    memset(&clientAddr, 0, sizeof clientAddr);
    clientAddr.ai_family = AF_INET;
    clientAddr.ai_socktype = SOCK_STREAM;

    getaddrinfo(host, port, &clientAddr, &res);

    socketFD = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    // Connect to server
    if (connect(socketFD, res->ai_addr, res->ai_addrlen) < 0) // Connect socket to address
    {
        fprintf(stderr, "Error: could not contact client on port %s\n", port);
        exit(2);
    }

    in_port_t portNum = ((struct sockaddr_in*)((struct sockaddr *)res->ai_addr))->sin_port;
    char hostName[INET_ADDRSTRLEN];
    get_ip_str((struct sockaddr *)(res->ai_addr), hostName, INET_ADDRSTRLEN);
    
    printf("SERVER: Connection made with %s:%d\n", hostName, ntohs(portNum));

    //return socket file descriptor
    freeaddrinfo(res);
    return socketFD;
}

/******************************************************************************************************
 * recvNewConnection function waits for incoming TCP connection on socket file descriptor passed
 * as an input. 
 * Function return the newly created connection file descriptor.
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
 * getSocketInfo function determines sockaddr name of connected socket from socket file descriptor
 * passed to function by calling getpeername function
******************************************************************************************************/
void getSocketInfo(struct sockaddr_in *addr, int socketFD)
{
    socklen_t addrSize = sizeof(*addr);
    int addrNameSize = getpeername(socketFD, (struct sockaddr *)addr, &addrSize);
    return;
}

/******************************************************************************************************
 * recvNewInput function waits on connected socket for incoming message from client
******************************************************************************************************/
void recvNewMessage(int socketFD, char *message)
{
    memset(message, '\0', MAX_INPUT_SIZE);         // Clear out the buffer
    recv(socketFD, message, MAX_INPUT_SIZE - 1, 0); // Read data from the socket, leaving \0 at end
    return;
}

/******************************************************************************************************
 * validateInput function determines if client input is valid (either -l or -g)
******************************************************************************************************/
int validateInput(char *input)
{
    char checkInput[MAX_INPUT_SIZE];
    strcpy(checkInput, input);
    checkInput[2] = '\0';

    if (strcmp(checkInput, "-l") == 0 || strcmp(checkInput, "-g") == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************************************
 * sendMessage function sends message through connected client socket
******************************************************************************************************/
void sendMessage(int socketFD, char *message)
{
    int charsWritten = send(socketFD, message, strlen(message), 0);
    if (charsWritten < 0)
        error("SERVER: ERROR writing to socket");
    return;
}

/******************************************************************************************************
 * handleRequest function handles the client request to either pass the current server directory file 
 * names, or export the contents of a specified file.
******************************************************************************************************/
void handleRequest(char *input, int connectionFD)
{
    printf("Processing client request...\n");
    char command[3];
    strncpy(command, input, 2);
    command[2] = '\0';

    int inputIndex = 2;
    if (strcmp(command, "-l") == 0)
    {
        //get char length of data port
        int portSize = 0;
        for (int i = inputIndex; input[i] != '-'; ++i) {
            portSize = portSize * 10 + input[i] - '0';
            inputIndex++;
        }
        inputIndex++;

        //get data port number from client input
        char dataPort[portSize + 1];
        strncpy(dataPort, input + inputIndex, portSize);
        dataPort[portSize] = '\0';

        //get client host from control connection
        struct sockaddr_in clientAddress;
        getSocketInfo(&clientAddress, connectionFD);
        char *clientHost = inet_ntoa(clientAddress.sin_addr);

        //initiate data connection with clinet
        int dataFD = initiateContact(clientHost, dataPort);

        //get directory files, source: https://faq.cprogramming.com/cgi-bin/smartfaq.cgi?answer=1046380353&id=1044780608
        DIR *d;
        struct dirent *dir;
        d = opendir(".");
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                char file[MAX_INPUT_SIZE];
                memset(file, '\0', MAX_INPUT_SIZE); // Clear out the buffer
                strcpy(file, dir->d_name);

                if (strcmp(file, "..") != 0 && strcmp(file, ".") != 0)
                {
                    sendMessage(dataFD, file);
                    printf("File name sent to client: %s\n", file);
                    char returnMessage[MAX_INPUT_SIZE];
                    recvNewMessage(dataFD, returnMessage);
                    if (strcmp(returnMessage, "next file") != 0)
                    {
                        error("ERROR with data transfer");
                    }
                }
            }
            sendMessage(dataFD, "done");
            closedir(d);
        }
        close(dataFD);
    }

    if (strcmp(command, "-g") == 0)
    {
        //get char length of file name
        int fileNameSize = 0;
        for (int i = inputIndex; input[i] != '-'; ++i) {
            fileNameSize = fileNameSize * 10 + input[i] - '0';
            inputIndex++;
        }
        inputIndex++;
        
        //get file name
        char fileName[fileNameSize + 1];
        strncpy(fileName, input + inputIndex, fileNameSize);
        fileName[fileNameSize] = '\0';
        inputIndex += fileNameSize;
        
        //get char length of data port
        int portSize = 0;
        for (int i = inputIndex; input[i] != '-'; ++i) {
            portSize = portSize * 10 + input[i] - '0';
            inputIndex++;
        }
        inputIndex++;
        
        //get data port number from client input
        char dataPort[portSize + 1];
        strncpy(dataPort, input + inputIndex, portSize);
        dataPort[portSize] = '\0';

        //get client host from control connection
        struct sockaddr_in clientAddress;
        getSocketInfo(&clientAddress, connectionFD);
        char *clientHost = inet_ntoa(clientAddress.sin_addr);

        //initiate data connection with clinet
        int dataFD = initiateContact(clientHost, dataPort);

        //call countFile function to get char length of text file
        int fileSize = countFile(fileName);
        if (fileSize < 0) {
            printf("File requested not found\n");
            char * fileError = "file not found";
            sendMessage(dataFD, fileError);
        }
        else {
            //call readFile function to get contents of file
            char file_contents[fileSize + 1];
            memset(file_contents, '\0', sizeof(file_contents));
            readFile(fileName, file_contents);
            
            //set file input size to string format
        	char file_size_string[256];
            memset(file_size_string, '\0', sizeof(file_size_string));
        	sprintf(file_size_string, "%d", fileSize);
	
            //send file size to client
            sendMessage(dataFD, file_size_string);
            char returnMessage[MAX_INPUT_SIZE];
            recvNewMessage(dataFD, returnMessage);
            if (strcmp(returnMessage, "file size received") != 0)
            {
                error("ERROR with data transfer");
            }
            
            // send file data to client
        	sendMessage(dataFD, file_contents); // Write to the server
            recvNewMessage(dataFD, returnMessage);
            if (strcmp(returnMessage, "file received") != 0)
            {
                error("ERROR with data transfer");
            }
            printf("%s exported to client.\n", fileName);
        }
        close(dataFD);
    }
}

/******************************************************************************************************
 * Main function opens TCP socket to the network and waits for a client connection. Once a connection
 * is made the program will validate the inputs then handle the client request. The program will allow
 * two commands from the client: list file name (-l) and export file contents (-g <FILE NAME>). Once
 * request is complete the connection is closed and the program will wait until another client 
 * request is made or program is terminated.
******************************************************************************************************/
int main(int argc, char *argv[])
{
    int charsRead;
    char *host = "localhost";
    char *port = argv[1];

    //start server on local host with port defined by user
    int controlSocketFD = openSocket(host, port);

    //loop until server is terminated 
    while (1)
    {
        //establish control connection with client
        int newConnectionFD = recvNewConnection(controlSocketFD);
        //wait for input from client
        char input[MAX_INPUT_SIZE];
        recvNewMessage(newConnectionFD, input);
        int inputValidated = validateInput(input);

        //if input is valid, send connection accepted message to client the process request
        if (!inputValidated)
        {
            printf("Invalid input\n");

            char *errorMessage = "Invalid input error, please try again";
            sendMessage(newConnectionFD, errorMessage);
            continue;
        }
        else
        {
            //send connection accepted message to client
            char *acceptMessage = "connection accepted";
            sendMessage(newConnectionFD, acceptMessage);

            //wait for client socket to open
            char message[MAX_INPUT_SIZE];
            recvNewMessage(newConnectionFD, message);

            if (strcmp(message, "data socket open") == 0)
            {
                //handle client request by calling handleRequest function
                handleRequest(input, newConnectionFD);
            }
            else
            {
                printf("Error connecting with client\n");
            }
        }
    }
}