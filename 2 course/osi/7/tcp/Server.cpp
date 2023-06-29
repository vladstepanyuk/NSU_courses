//
// Created by io on 26.05.2023.
//

#include <iostream>
#include "Server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <sys/wait.h>


namespace {
    int PORT = 8888;
    int BUFFER_SIZE = 1024;
}

Server::Server(int n):num_of_listeners(n) {
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == -1) {
        throw std::runtime_error("can't create socket");
    }

    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = inet_addr("127.0.0.2");


    if (bind(server_socket, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
        throw std::runtime_error("can't bind socket");
    }
    std::cout << "my address: " << inet_ntoa(si_me.sin_addr) << std::endl;


}

void Server::start() {
    int i = 0;
    sockaddr_in si_other;
    std::string buffer(BUFFER_SIZE, 0);
    listen(server_socket, num_of_listeners);
    int new_socket;
    while (true) {
        int slen = sizeof(si_me);

        for (i = 0; i < num_of_listeners; ++i) {

            if ((new_socket = accept(server_socket, (struct sockaddr *) &si_other, reinterpret_cast<socklen_t *>(&slen))) == -1) {
                throw std::runtime_error("can't accept connection");
            }

            int f;

            f = fork();

            if (f == 0) {
                process_client(new_socket);
                exit(0);
            } else if (f < 0) {
                throw std::runtime_error("can't do fork");
            }
        }

        int status;
        wait(&status);
        i--;

    }
}

void Server::process_client(int new_socket) {
    std::string buffer(BUFFER_SIZE, 0);
    try {
        while (true) {
            int slen = sizeof(si_me), recv_len;

            if ((recv_len = recv(new_socket, &buffer[0], BUFFER_SIZE, MSG_CMSG_CLOEXEC)) == 0) {
                throw std::runtime_error("can't receive message");
            }
            buffer[recv_len] = 0;

            if (buffer.substr(0, recv_len) == "exit") {
                close(new_socket);
                return;
            }


            if (send(new_socket, &buffer[0], recv_len, 0) == -1) {
                throw std::runtime_error("can't send message");
            }

        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return;
    }

}

