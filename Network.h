//
// Created by Rasul Mammadov on 14.04.25.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>

class Network {
public:
    int create_socket();
    int bind_socket();
    int listen_socket();
    int accept_socket();
    int close_socket();
private:
    int sock_fd, cfd;
    int bind_fd;
    sockaddr_in addr, client_addr;
    socklen_t client_addr_len;
};



#endif //NETWORK_H
