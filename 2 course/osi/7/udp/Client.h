#ifndef INC_7_CLIENT_H
#define INC_7_CLIENT_H
#include<sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>


class Client {
    int my_socket;
    struct sockaddr_in si_me, si_other;
public:
    Client();

    void start();
};


#endif //INC_7_CLIENT_H
