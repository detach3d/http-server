#include "Network.h"
#include <exception>
#include <iostream>

int main() {
    Network network;
    try {
        network.create_socket();
        network.bind_socket();
        network.listen_socket();
        network.accept_socket();
        network.close_socket();
    }catch(const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

}
