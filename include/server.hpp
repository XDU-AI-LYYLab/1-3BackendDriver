//
// Created by Lhuna on 2024/9/29.
//

#ifndef SERVER_HPP
#define SERVER_HPP

#include <crow.h>
#include "utils.hpp"

void SetupServer(crow::SimpleApp& app) {
    CROW_ROUTE(app, "/runfile1")([&](const crow::request& req) {
        RunProgram(1);
        return "Program started.";
    });
}

#endif
