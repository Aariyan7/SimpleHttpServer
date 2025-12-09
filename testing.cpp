#include <iostream> 
#include <array>
#include <unordered_map>
#include <vector>


struct HttpRequest {
    std::string Method;
    std::string Path;
    std::string Version;
    std::unordered_map <std::string, std::string> Header;
    std::string Body;
};

int main () {
    std::string request = "GET / HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "User-Agent: Mozilla/5.0\r\n"
    "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n"
    "Accept-Language: en-GB,en;q=0.5\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Connection: keep-alive\r\n"
    "\r\n";   

    std::array <std::string, 7> str_arr;

    int starting_point = 0;

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
        std::cout << "This was the element that was added recently bro: " << std::endl << str_arr[i] << std::endl;
    }

    HttpRequest httpRequest; // HttpRequest Structure

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
        else if (i > 0 && i < 7) { // From other lines  extract -> Headers
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

            httpRequest.Header[key] = value; // Add the new Header
        } 
        
        else { // Extract the body

        }
        
    }


    std::cout << "This is the result after the extarction: " << std::endl << std::endl << std::endl;

    std::cout << "This is the Method of the Http request: " << httpRequest.Method << std::endl << std::endl;
    std::cout << "This is the Path of the Http request: " << httpRequest.Path << std::endl << std::endl;
    std::cout << "This is the Version of the Http request: " << httpRequest.Version << std::endl << std::endl;

    std::vector<std::string> Keys;
    Keys.reserve(httpRequest.Header.size());
    
    std::vector<std::string> Values;
    Keys.reserve(httpRequest.Header.size());

    for (auto kv: httpRequest.Header) {
        Keys.push_back(kv.first);
        Values.push_back(kv.second);
    }

    std::cout << "These are the headers of the Http request: " << std::endl << std::endl;
    for (int i = 0; i < httpRequest.Header.size(); i++) {
        std::cout << Keys[i] << ":" << Values[i] << std::endl;
    }

    return 0;
}