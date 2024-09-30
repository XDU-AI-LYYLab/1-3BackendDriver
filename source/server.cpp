/**
 * @file server.cpp
 * @brief Main entry point for the Crow server application.
 *
 * This application initializes a Crow server that can execute specified programs
 * based on group configurations loaded from a TOML file.
 */

#include <utility>
#include <future>

#include "crow.h"
#include "utils.hpp"

/**
 * @brief Main function to start the Crow server and handle command-line arguments.
 *
 * @param argc Number of arguments.
 * @param argv Array of command-line arguments.
 * @return int Returns 0 on successful server start, 1 on error.
 */
int main(const int argc, char* argv[]) {
    // Display help if insufficient arguments are provided
    if (argc < 2) {
        PrintHelp();
        return 1;
    }

    // Handle help command
    if (const std::string cmd = argv[1]; cmd == "/h" or cmd == "-h") {
        PrintHelp();
        return 0;
    }
    // Handle config file loading
    else if ((cmd == "/f" or cmd == "-f") and  argc > 2) {
        if (not LoadConfig(argv[2])) {
            std::cerr << "Failed to load configuration file." << std::endl;
            return 1;
        }
    }
    // If command is unrecognized, print help
    else {
        PrintHelp();
        return 1;
    }

    crow::SimpleApp app;
    // Define a route to handle requests for running programs based on group ID
    CROW_ROUTE(app, "/group/<int>")([](const int group_id){
        // Check if the group exists in the configuration
        if (not config_map.contains(group_id)) {
            return crow::response(404, "Error: Group does not exist");
        }
        auto future = RunProgram(group_id);

        // Wait for the program to finish and send an HTTP response based on the result
        if (const auto result = future.get(); result == 0) {
            return crow::response("Program for group " + std::to_string(group_id) + " executed successfully.");
        } else {
            return crow::response(500, "Program for group " + std::to_string(group_id) + " failed with error code: " + std::to_string(result));
        }
    });

    // Start the server on port 18080 with multithreading enabled
    app.port(server_port).multithreaded().run();
}
