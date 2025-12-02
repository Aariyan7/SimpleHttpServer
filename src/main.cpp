#include <iostream>
#include <sys/socket.h>

#include <netinet/in.h>
#include <cstring>

int create_socket () {
    int socket_fd = socket (AF_INET, SOCK_STREAM, 0); // IPV4 // TCP
    if (socket_fd >= 0) return socket_fd; 
    else {
        throw std::runtime_error ("Failed to create a socket");
    }
}

void listen_my (int socket_fd) {
    sockaddr_in server_config;
    server_config.sin_family = AF_INET; // We are using IPV4
    server_config.sin_port = htons(8000); // Convert a host byte order into a network byte order (port)
    server_config.sin_addr.s_addr = INADDR_ANY;  // Our server listen on all, of our system's IP

    if (bind (socket_fd, (struct sockaddr*)&server_config,sizeof(server_config)) < 0) {
        throw std::runtime_error ("Failed to bind the socket to the server!!!");
    } else {
        std::cout << "Succesfully bound the socket!!!" << std::endl;
    }

    if (listen (socket_fd, 1000) < 0) {
        throw std::runtime_error ("Failed to listen!!!");
    } else {
        std::cout << "Successfully listening on the port 8000" << std::endl;
    }

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr); 

    // Accept new connections
    bool running = true;
    while (running)
    {
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0) {
            std::cout << "Failed to accept the client connection!!!" << std::endl; 
            running = false;
        }
        else {
            std::cout << "Accepted the request from the client" << std::endl;

            char buffer[2408];
            int bytes = recv(client_fd, buffer, sizeof(buffer), 0);

            printf("This the received request \n %s \n", buffer);
            char response[] =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Length: 13\r\n"
                    "Content-Type: text/plain\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Hello, world!";

            send(client_fd, response, strlen(response), 0);

        }
    }

}

int main () {
    int socket_fd = create_socket ();    
    std::cout << "The file descriptor of the socket is: " << socket_fd << std::endl; 
    listen_my (socket_fd);

    return 0;   
}