//
// Created by io on 26.05.2023.
//

#include <iostream>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>


namespace {
    int PORT = 8888;
    int BUFFER_SIZE = 1024;
}

Server::Server() {
    server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_socket == -1) {
        throw std::runtime_error("can't create socket");
    }

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = inet_addr("127.0.0.1");;


    if (bind(server_socket, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
        throw std::runtime_error("can't bind socket");
    }
    std::cout << "my address: " << inet_ntoa(si_me.sin_addr) << std::endl;


}

void Server::start() {
    int i = 0;
    std::string buffer(BUFFER_SIZE, 0);
    while (true) {
        int slen = sizeof(si_me), recv_len;

        printf("Waiting for data...");
        fflush(stdout);

        if ((recv_len = recvfrom(server_socket, &buffer[0], BUFFER_SIZE, 0, (struct sockaddr *) &si_other,
                                 reinterpret_cast<socklen_t *>(&slen))) == -1) {
            throw std::runtime_error("can't receive message");
        }
        buffer[recv_len] = 0;

        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n", &buffer[0]);

        if (sendto(server_socket, &buffer[0], recv_len, 0, (struct sockaddr *) &si_other, slen) == -1) {
            throw std::runtime_error("can't send message");
        }

    }
}

