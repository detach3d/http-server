//
// Created by Rasul Mammadov on 14.04.25.
//

#include "Network.h"
#include <iostream>
#include <arpa/inet.h>
#include <stdexcept>
#include <unistd.h>
#include <sstream>
#include <fstream>

#define MAX_LEN 1024

int Network::create_socket() {
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        close_socket();
        throw std::invalid_argument("Failed to create socket");
    }
    return sock_fd;
}

int Network::bind_socket() {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8001);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    bind_fd = bind(sock_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (bind_fd == -1) {
        close_socket();
        throw std::invalid_argument("Failed to bind socket");
    }
    return bind_fd;
}

int Network::listen_socket() {
    if (listen(sock_fd, 10) == -1) {
        close_socket();
        throw std::invalid_argument("Failed to listen on socket");
    }
    return sock_fd;
}

int Network::accept_socket() {
    while (true) {
        client_addr_len = sizeof(client_addr);
        cfd = accept(sock_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_addr_len);
        if (cfd == -1) {
            close_socket();
            throw std::invalid_argument("Failed to accept connection");
        }
        //std::cout << "Connection accepted" << std::endl;

        // Reading from client
        int rec_fd;

        char buffer[MAX_LEN];
        rec_fd = recv(cfd, buffer, MAX_LEN, 0);
        std::string buffer_s{buffer};

        if (rec_fd == -1) {
            close_socket();
            throw std::invalid_argument("Failed to receive message");
        }
        //buffer[rec_fd] = '\0';
        // std::cout << "size: " << rec_fd << '\n' << buffer << std::endl;

        // string slicing
        std::string verb, path, protocol;


        auto it = std::find(std::begin(buffer_s), std::end(buffer_s), ' ');
        verb = std::string(std::begin(buffer_s), it);
        buffer_s.erase(std::begin(buffer_s), it + 1);
        it = std::find(std::begin(buffer_s), std::end(buffer_s), ' ');
        path = std::string(std::begin(buffer_s), it);
        buffer_s.erase(std::begin(buffer_s), it + 1);
        protocol = std::string(std::begin(buffer_s), it);

        // std::cout << verb << " " << path << " " << protocol << std::endl;
        path.erase(path.begin());
        std::ifstream infile{path};
        std::string body{std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>()};

        int contentLength = body.length();

        const std::string protocol_c{"HTTP/1.1"};

        std::ostringstream response{};
        response << protocol_c << " 200 OK\n"
                << "Content-Type: text/html\n"
                << "Content-Length: " << contentLength + 1 << "\n\n"
                << body;

        std::string responseStr = response.str();
        //std::cout << protocol;
        std::cout << responseStr << std::endl;

        char *responseBuf = const_cast<char *>(responseStr.c_str());
        // Sending to client
        send(cfd, responseBuf, responseStr.length(), 0);
    }
}

int Network::close_socket() {
    if (close(sock_fd) == -1) {
        throw std::invalid_argument("Failed to close socket");
    }
}
