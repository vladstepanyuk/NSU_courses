#ifndef INC_7_SERVER_H
#define INC_7_SERVER_H
#include<sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>


class Server {

    int server_socket = -1;
    struct sockaddr_in si_me, si_other;

    int num_of_listeners;

public:

    Server();

    void start();


};


#endif //INC_7_SERVER_H
