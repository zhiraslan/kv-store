#include "TcpServer.h"
#include <sys/socket.h>   // socket, bind, listen, accept
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY, htons, htonl
#include <unistd.h>       // close, read, write
#include <signal.h>



volatile std::sig_atomic_t TcpServer::running = 1;

TcpServer::TcpServer(int port, Service& service)
    : port_{port}
    , service_{service}
    , server_fd_{-1} {}


void TcpServer::start() {
    // socket creation
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP could be instead of 0
    if (server_fd_ == -1) {
        perror("--server socket failed--");
	return;
    }
    std::cout << "socket successfully created. FD = "
		<< server_fd_ << ".\n";

    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080); // Host to Network (into big-endian)
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == -1) {
	perror("--bind failed--");
	close(server_fd_);
	return;
    }

    if (listen(server_fd_, 10) == -1) {
	perror("--listen failed--");
	close(server_fd_);
	return;
    }

    while (running) {

        std::cout << "server is listening...\n";

        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

	int client_fd = accept(server_fd_, reinterpret_cast<sockaddr*>(&client_addr), &client_len);

	if (client_fd == -1) {
	    perror("--accept failed--");
    	    continue;
        }

        std::cout << "client is successfully connected. FD = "
                << client_fd << ".\n";
// ...
    }
}


void TcpServer::stop() {
    running = false;

    if (server_fd_ != -1) {
	close(server_fd_);
	server_fd_ = -1;
    }
}

void TcpServer::handleClient(int client_fd) {



}

#include <vector>

using ValueRESP = std::variant<std::string, int64_t, 



class ValueRESP {
public:
    Type getType() const {
	return type_;
    }

    // specific constructors
    ValueRESP(int64_t num)
        : t_{Type::Integer}
        , num_{num} {}

    ValueRESP(std::string s)
        : t_{Type::SimpleString}
        , s_{s} {}

private:
    enum class Type {
        SimpleString,
        Integer,
        Array,
        BulkString,
        Error
    };

    Type t_ = Type::SimpleString;
    std::string s_ = "";
    int64_t num_ = 0;

};

void parseRESP(const std::vector<std::string>& buffer) {
    std::vector<std::string> args;
    size_t pos = 0;
 
while (pos < buffer.size()) {
    switch (buffer[pos]) {
	case ':':  // integer
	    
	case '*': // array
	    readUntilCRLF(buffer, pos);
	case '$': // bulk string

	case '+': // simple string
	



}

void readUntilCRLF(const std::vector<string>& buffer, size_t& pos) { 
    std::string s = "";
    while (buffer[pos] != '\r') {
	s.push_back(buffer[pos];
	++pos;
    }
	
    pos += 2;
    return;
}
