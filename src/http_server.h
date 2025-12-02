
#ifndef HTTP_SERVER_H_
#define HTTP_SERVER_H_

#include <sys/socket.h>


namespace simple_http_server {
    // This defines the maximum number of bytes our socket can handle each time
    constexpr size_t kMaxBufferSize = 4096;

    struct EventData { // This defines the 
        EventData () : fd (0), length(0), cursor(0), buffer() {};
        int fd;
        size_t length;
        size_t cursor;
        char buffer[kMaxBufferSize];
    };


}


#endif