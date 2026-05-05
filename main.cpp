#include "Storage.h"
#include "Service.h"
#include "TcpServer.h"
#include <iostream>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>


int main() {
struct addrinfo hints {};
hints.ai_family = AF_UNSPEC;
hints.ai_socktype = SOCK_STREAM;
hints.ai_protocol = 0;

struct addrinfo* result = nullptr;
int status = getaddrinfo(nullptr, "8080", &hints, &result);

if (status != 0) {
    std::cerr << "Ошибка getaddrinfo: " << gai_strerror(status) << std::endl;
} else {
    std::cout << "getaddrinfo успешно отработал" << std::endl;
    
    // Пройти по списку и вывести адреса
    for (struct addrinfo* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
        char ip[INET6_ADDRSTRLEN];
        void* addr;
        
        if (ptr->ai_family == AF_INET) {
            // IPv4
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            // IPv6
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
        }
        
        inet_ntop(ptr->ai_family, addr, ip, sizeof(ip));
        std::cout << "Найден адрес: " << ip << std::endl;
    }
    
freeaddrinfo(result);
}
}
