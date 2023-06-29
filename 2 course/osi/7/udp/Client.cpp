//
// Created by io on 29.05.2023.
//

#include <iostream>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>

#include "Client.h"

namespace {
    int PORT = 8888;
    std::string standard_address = "127.0.0.1";
    int BUFFER_SIZE = 1024;
}

Client::Client() {
    my_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (my_socket == -1) {
        throw std::runtime_error("can't create socket");
    }

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);


    std::cout << "my address: " << inet_ntoa(si_me.sin_addr) << std::endl;


}

void Client::start() {

    memset((char *) &si_other, 0, sizeof(si_other));

    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORT);

    if (!inet_aton(&standard_address[0], &si_other.sin_addr))
        throw std::runtime_error("wrong ip address");


    sockaddr_in si_other_another;

    std::string buffer(BUFFER_SIZE, 0);

    std::string data;
    int slen = sizeof(si_me);
    int recv_len;
    while (true) {
        std::cin >> data;

        if (data == "exit")
            break;


        if (sendto(my_socket, &data[0], data.size(), 0, (sockaddr*)&si_other, slen) == -1) {
            throw std::runtime_error("can't send message");
        }

        if ((recv_len = recvfrom(my_socket, &buffer[0], BUFFER_SIZE, 0, (struct sockaddr *) &si_other_another,
                                 reinterpret_cast<socklen_t *>(&slen))) == -1) {
            throw std::runtime_error("can't receive message");
        }
        buffer[recv_len] = 0;

        std::cout << buffer.substr(0, recv_len);
    }
}
