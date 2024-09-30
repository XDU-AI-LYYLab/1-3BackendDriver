//
// Created by Lhuna on 2024/9/29.
//

// main.cpp
#include <utility>

#include "crow.h"
#include "utils.hpp"

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
    CROW_ROUTE(app, "/group/<int>")([](int group_id){
        if (!config_map.contains(group_id)) {
            return crow::response(404, "Error: Group does not exist"); // 返回404错误
        }
        RunProgram(group_id);
        return crow::response("Program for group " + std::to_string(group_id) + " started.");
    });
    app.port(18080).multithreaded().run();
}
