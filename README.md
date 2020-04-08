CS372 Project 2 - File Transfer
Josh Kinzel

BUILD & RUN:

1. Compile ftserver.c with the following command: `gcc ftserver.c -o ftserver -std=gnu99`
2. Start server: `./ftserver <SERVER PORT>`
3. Run ftclient.py program with the following command options:
    -To request list of server files: `python3 ftclient.py <SERVER HOST> <SERVER PORT> -l <CLIENT PORT>`
    -To request file transfer: `python3 ftclient.py <SERVER HOST> <SERVER PORT> -g <FILE NAME> <CLIENT PORT>`