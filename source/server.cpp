//
// Created by Lhuna on 2024/9/29.
//

// main.cpp
#include <crow.h>
#include "utils.hpp"
#include "server.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        PrintHelp();
        return 1;
    }

    std::string cmd = argv[1];
    if (cmd == "/h" || cmd == "-h") {
        PrintHelp();
        return 0;
    } else if ((cmd == "/f" || cmd == "-f") && argc > 2) {
        if (!LoadConfig(argv[2])) {
            std::cerr << "Failed to load configuration file." << std::endl;
            return 1;
        }
    } else {
        PrintHelp();
        return 1;
    }

    crow::SimpleApp app;
    SetupServer(app);
    app.port(18080).multithreaded().run();
}
