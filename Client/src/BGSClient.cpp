#include <stdlib.h>
#include <connectionHandler.h>
#include <iostream>
#include <thread>

/**
* This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
*/
//read from console and sends a message to the server
bool readKeyBoard(ConnectionHandler *connectionHandler) {
    while (true) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        //test if this works
        std::cout
                << "the next row is to check if moving the char array to string worked. we should see a string of a command from client"
                << std::endl;
        std::cout << line << std::endl;
//        int len = line.length();
        std::string operation;

        //get operation from line
        for (int i = 0; line[i] != ' '; i++) {
            operation.push_back(line[i]);
        }

        bool delivered;


        if (operation == ("REGISTER")) {
            delivered = connectionHandler->sendLine(line, 01);
        } else if (operation==("LOGIN")) {
            delivered = connectionHandler->sendLine(line, 02);
        } else if (operation==("LOGOUT")) {
            delivered = connectionHandler->sendLine(line, 03);
        } else if (operation==("FOLLOW") || operation.compare("UNFOLLOW")) {
            delivered = connectionHandler->sendLine(line, 04);
        } else if (operation==("POST")) {
            delivered = connectionHandler->sendLine(line, 05);
        } else if (operation==("PM")) {
            delivered = connectionHandler->sendLine(line, 06);
        } else if (operation==("LOGSTAT")) {
            delivered = connectionHandler->sendLine(line, 07);
        } else if (operation==("STAT")) {
            delivered = connectionHandler->sendLine(line,8);
        } else if (operation==("BLOCK")) {
            delivered = connectionHandler->sendLine(line, 12);
        }

        if (!delivered) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }
    }
    return false;
}

bool readFromSocket(ConnectionHandler* connectionHandler) {
    while (true) {
        std::string response;
        if (!connectionHandler->getLine(response)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        std::cout << response << std::endl;

        if (response==("ACK 3")) {
            break;
        }
    }
    return false;
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    
    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    std::thread write(readKeyBoard,&connectionHandler);
    readFromSocket(&connectionHandler);
//    read.join();
    write.join();

    return 0;
}