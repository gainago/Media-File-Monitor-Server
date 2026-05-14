
#include <iostream>
#include "output_mutex.h"
#include "Server.h"
std::mutex output_mutex;

int main(int argc, char *argv[]) {

    std::string directory = "/home/goshagaina/"; 
    std::size_t polling_period = 30; //seconds
    if (argc >= 3) {
        directory = argv[1];
        polling_period = std::stoll(argv[2]);
        std::cout << "directory: " << directory << '\n';
        std::cout << "polling period: " << polling_period << '\n';
    }

    startServer(directory, polling_period);

    return 0;
}