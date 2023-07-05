#include <iostream>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

void start_client(const char* server_address, int port) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        throw std::runtime_error("Failed to create client socket");
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(server_address);
    server_address.sin_port = htons(port);

    if (connect(client_socket, reinterpret_cast<const sockaddr*>(&server_address), sizeof(server_address)) == -1) {
        throw std::runtime_error("Failed to connect to server");
    }

    std::cout << "Connected to " << server_address << ":" << port << std::endl;

    while (true) {
        const char* message = "ping";
        send(client_socket, message, strlen(message), 0);
        std::cout << "Sent 'ping' message" << std::endl;

        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break; // Server disconnected
        }

        std::string response(buffer, bytes_received);
        if (response == "pong") {
            std::cout << "Received 'pong' response" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <server_address> <port>" << std::endl;
        return 1;
    }

    const char* server_address = argv[1];
    int port = std::atoi(argv[2]);

    try {
        start_client(server_address, port);
    } catch (const std::exception& e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
