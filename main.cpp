#include <iostream>
#include "Storage.h"
#include "Service.h"
#include "RESPParser.h"


int main() {
    RESPParser parser;
    std::string input = "*3\r\n$3\r\nSET\r\n$4\r\nuser\r\n$5\r\nAlice\r\n";
    std::vector<std::string> res = parser.parseRESP(input);
    for (auto& el : res) {
        std::cout << el << std::endl;
    }
    //std::cout << res.size();
    //std::cout << "kv-store has started\n";
}