#ifndef INC_7_SERVER_H
#define INC_7_SERVER_H
#include<sys/socket.h>
#include <netinet/in.h>
#include <stdexcept>
#include <cstring>


class Server {

    int server_socket = -1;
    sockaddr_in si_me;

    int num_of_listeners;

public:

    explicit Server(int n);

    void start();

    void process_client(int new_socket);


};


#endif //INC_7_SERVER_H
