#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <unordered_map>                  
#include <vector>
#include <array>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <chrono>


static bool is_server_running = true;

enum class HttpStatusCode {
    Continue = 100,
    SwitchingProtocols = 101,
    EarlyHints = 103,
    Ok = 200,
    Created = 201,
    Accepted = 202,
    NonAuthoritativeInformation = 203,
    NoContent = 204,
    ResetContent = 205,
    PartialContent = 206,
    MultipleChoices = 300,
    MovedPermanently = 301,
    Found = 302,
    NotModified = 304,
    BadRequest = 400,
    Unauthorized = 401,
    Forbidden = 403,
    NotFound = 404,
    MethodNotAllowed = 405,
    RequestTimeout = 408,
    ImATeapot = 418,
    InternalServerError = 500,
    NotImplemented = 501,
    BadGateway = 502,
    ServiceUnvailable = 503,
    GatewayTimeout = 504,
    HttpVersionNotSupported = 505
};

// Here we only support HTTP/1.1
enum class HttpVersion {
  HTTP_0_9 = 9,
  HTTP_1_0 = 10,
  HTTP_1_1 = 11,
  HTTP_2_0 = 20
};


struct HttpRequest {
   std::string Method;
   std::string Path;
   std::string Version;
   std::unordered_map<std::string, std::string> Headers;
   std::string Body;
};

struct HttpResponse {
    std::string Version;
    int status_code;
    std::string Reason_phrase;
    std::unordered_map <std::string, std::string> Headers;
    std::string Body;
};


// Create a non-blocking sokcet
int create_socket () {
    int socket_fd = socket (AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd < 0) {
        throw std::runtime_error ("Failed to create a socket");
    }
    return socket_fd;
}


// Bind the socket to the Server's IP and PORT
void bind_socket (int socket_fd) {
    sockaddr_in server_config;

    server_config.sin_family = AF_INET;
    server_config.sin_port = htons(8000);
    server_config.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(socket_fd, (struct sockaddr*) &server_config, sizeof(server_config)) < 0) {
        throw std::runtime_error("Failed to bind the socket to the server");
    }
}


void start_listening (int socket_fd) {
    sockaddr_in server_config;
    server_config.sin_family = AF_INET; // We are using IPV4
    server_config.sin_port = htons(8000); // Convert a host byte order into a network byte order (port)
    server_config.sin_addr.s_addr = INADDR_ANY;  // Our server listen on all, of our system's IP

    if (listen (socket_fd, 1000) < 0) {
        throw std::runtime_error ("Failed to listen!!!");
    } else {
        std::cout << "Successfully listening on the port 8000" << std::endl;
    }

}


// Method to parse the request and then store them in a struct
HttpRequest parse_http_request (std::string request) {
    HttpRequest httpRequest;

    std::array <std::string, 7> str_arr;

    int starting_point = 0;
    std::cout << "Parsing the client's Request!!!" << std::endl;
    
    for (int i = 0; i < str_arr.size(); i++) {
        str_arr[i] = "";
        for (int j = starting_point; j < request.size(); j++) {
            if (request[j] == '\n') {
                str_arr[i] += request[j];
                starting_point = j + 1;
                break;
            }
            str_arr[i] += request[j];
        }
    }

    // Extract the content and store it in the struct
    for (int i = 0; i < str_arr.size(); i++) {
        // From first line extract -> Method, Path, Version
        if (i==0) {
            std::array <std::string, 3> first_line;
            std::string content = "";

            int array_num = 0;
            for (int j = 0; j < str_arr[i].size(); j++) {
                if (str_arr[i][j] == ' ') {
                    array_num++;
                    continue;
                }
                first_line[array_num] += str_arr[i][j];
            }

            httpRequest.Method = first_line[0];
            httpRequest.Path = first_line[1];
            httpRequest.Version = first_line[2];

        } 
        else if (i > 0 && i < 7) { // From other lines  extract -> Headerss
            std::string key;
            std::string value;

            std::string content = "";

            for (int j = 0; j < str_arr[i].size(); j++) {
                if (str_arr[i][j] == ':') {
                    key = content; 
                    content = "";
                    continue;
                }
                content += str_arr[i][j];
            }
            value = content;

            httpRequest.Headers[key] = value; // Add the new Headers
        } 
        
        else { // Extract the body

        }
        
    }


    std::cout << "This is the result after the extarction: " << std::endl << std::endl << std::endl;

    std::cout << "This is the Method of the Http request: " << httpRequest.Method << std::endl << std::endl;
    std::cout << "This is the Path of the Http request: " << httpRequest.Path << std::endl << std::endl;
    std::cout << "This is the Version of the Http request: " << httpRequest.Version << std::endl << std::endl;

    std::vector<std::string> Keys;
    Keys.reserve(httpRequest.Headers.size());
    
    std::vector<std::string> Values;
    Keys.reserve(httpRequest.Headers.size());

    for (auto kv: httpRequest.Headers) {
        Keys.push_back(kv.first);
        Values.push_back(kv.second);
    }

    std::cout << "These are the Headers of the Http request: " << std::endl << std::endl;
    for (int i = 0; i < httpRequest.Headers.size(); i++) {
        std::cout << Keys[i] << ":" << Values[i] << std::endl;
    }

    return httpRequest;

}


// Accept new incoming TCP connections
void accept_tcp_connections (int socket_fd) {

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr); 


    // Accept new connections
    bool running = true;
    while (running) {
       int client_fd = accept4(socket_fd, (struct sockaddr*)&client_addr, &client_len, SOCK_NONBLOCK);
       
       if (client_fd < 0) {
           std::cout << "Failed to accept the client connection!!!" << std::endl;
           std::this_thread::sleep_for (std::chrono::milliseconds(100));
           continue;
       }
       else {
           std::cout << "Accepted the request from the client 😊😊😊" << std::endl;

           char buffer[2408];
           int bytes; 
           std::string request;

           while (true) { // Since TCP is stream, meaning we will not get the full request right away so we will have to loop through and get the whole requests
           
               bytes = recv(client_fd, buffer, sizeof(buffer), 0);
               if (bytes <= 0) break;
               request.append (buffer, bytes);

               size_t pos = request.find ("\r\n\r\n"); 
               if (pos != std::string::npos) break;

           }
           
           std::cout << "This is the request received from the User: " << std::endl << request << std::endl;

           HttpRequest httpRequest = parse_http_request(request); // This retuns a HttpRequest struct

           char response[] =
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Length: 13\r\n"
                   "Content-Type: text/plain\r\n"
                   "Connection: close\r\n"
                   "\r\n"
                   "Hello, world!";

           size_t total_sent = 0;
           size_t response_len = strlen(response);

           while (total_sent < response_len)
           {
                size_t sent = send(client_fd, response, strlen(response), 0);
                total_sent += sent;
           }
           
           if (httpRequest.Headers["Connection"] == "Keep-alive") {
                continue;
           }
           else {
                close(client_fd);
           }
       }

       std::cout << "Sent the Response Successfully!!! 😊😊😊" << std::endl;
       break;
 
    }
    
}

// Start the Server
void start () {
    int socket_fd = create_socket();
    bind_socket(socket_fd);
    start_listening(socket_fd);
    accept_tcp_connections (socket_fd);
}

void stop_server () {
    
}

int main () {
    
    start ();

    return 0;   
}


