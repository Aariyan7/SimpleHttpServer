#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include <string>

class HttpServer {
    private:
        
    public:
        std::string host;
        int port;
        HttpServer(std::string host, int port);
        ~HttpServer() = default;

        void start ();
        void stop ();
};


#endif