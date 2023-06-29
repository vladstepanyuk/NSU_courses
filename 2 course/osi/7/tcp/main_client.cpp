#include "Client.h"
#include <iostream>


int main() {
    try {
        Client client;

        client.start();
    } catch (std::exception &e) {
        std::cerr << e.what();
    }




}
