#include "gtest/gtest.h"
#include "UDPConnection.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <thread>

namespace {
    bool recieved=false;

    void send(){
        char ip[]="127.0.0.1";
        std::uint8_t buffer[1024];
        for(int i=0;i<1024;i++){
            buffer[i]=7U;
        }
        pipert::UDPConnection connection=pipert::UDPConnection(8080,ip);
        connection.openCoccection();
        connection.send(buffer);
        connection.closeConnection();
    }

        void recieve() {

            int sockfd;
            std::uint8_t  buffer[1024];
            struct sockaddr_in servaddr, cliaddr;

            // Creating socket file descriptor
            if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("socket creation failed");
                exit(EXIT_FAILURE);
            }

            memset(&servaddr, 0, sizeof(servaddr));
            memset(&cliaddr, 0, sizeof(cliaddr));

            // Filling server information
            servaddr.sin_family = AF_INET; // IPv4
            servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            servaddr.sin_port = htons(8080);

            // Bind the socket with the server address
            if (bind(sockfd, (const struct sockaddr *) &servaddr,
                     sizeof(servaddr)) < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }


            socklen_t len;

            len = sizeof(cliaddr);


            int timeout=3;
            while (!recieved && timeout>0) {
                send();
                timeout=0;
                 recvfrom(sockfd, buffer, 1024,
                             MSG_WAITALL, (struct sockaddr *) &cliaddr,
                             &len);


                if(buffer[0]==7) recieved=true;


            }
        }



    TEST(UDPConnectionTest, TestSending) {
        recieve();
    EXPECT_EQ(recieved, true);
    }


}