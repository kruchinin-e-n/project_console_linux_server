#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 10

void client_handler(int client_sock) {
    std::cout << "Client connected\n";

    char buffer[2000];
    std::ofstream log_file("log.txt", std::ios_base::app);

    while(recv(client_sock, buffer, 2000, 0) > 0) {
        log_file << buffer << std::endl;
    }

    std::cout << "Client disconnected\n";
    close(client_sock);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        std::cerr << "Usage: ./server [PORT]\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    int server_sock, client_sock, c;
    struct sockaddr_in server, client;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        std::cerr << "Could not create socket\n";
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if(bind(server_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    std::cout << "Listening socket opened\n";
    listen(server_sock, MAX_CLIENTS);
    c = sizeof(struct sockaddr_in);

    while((client_sock = accept(server_sock, (struct sockaddr*)&client, (socklen_t*)&c))) {
        std::thread(client_handler, client_sock).detach();
    }

    if (client_sock < 0) {
        std::cerr << "Accept failed\n";
        return 1;
    }

    return 0;
}
