CS372 Project 2
Josh Kinzel

BUILD & RUN:

1. Unzip files to desired directory.
2. Go to directory where files have been exported.
3. Transfer ftserver.c file to directory location with .txt files that you want to make available to a connecting client.
4. Transfer ftclient.py file to directory where you want to run the client side program. 
3. Compile ftserver.c with the following command:
    gcc ftserver.c -o ftserver -std=gnu99 
4. Start server: ./ftserver <SERVER PORT>
5. Run ftclient.py program with the following command options:
    To request list of server files: python3 ftclient.py <SERVER HOST> <SERVER PORT> -l <CLIENT PORT>
    To request file transfer: python3 ftclient.py <SERVER HOST> <SERVER PORT> -g <FILE NAME> <CLIENT PORT>