/**
 * @file utils.hpp
 * @brief Utility functions and configurations for the Crow server application.
 *
 * This file contains functions for loading configurations, running processes,
 * and managing log files. It uses asynchronous operations to handle processes
 * and leverages filesystem operations to manage log directories.
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <unordered_map>
#include <iostream>
#include <vector>
#include <ctime>
#include <sys/wait.h>
#include <unistd.h>
#include <future>
#include <atomic>
#include <fstream>
#include <filesystem>
#include "toml.hpp"

namespace fs = std::filesystem;

// Globally accessible configuration map where key is the group ID.
inline std::unordered_map<int, std::pair<std::string, std::vector<std::string>>> config_map;

// Default directory for storing log files, configurable via TOML file.
inline std::string log_directory = "./log";

// Default server port
inline int server_port = 18080;

/**
 * @brief Prints the usage information for the application.
 */
inline void PrintHelp() {
    std::cout << "Usage:\n";
    std::cout << "  /h or -h : Print this help message and exit.\n";
    std::cout << "  /f or -f <config_file> : Specify the configuration file.\n";
}

/**
 * @brief Loads the configuration from a TOML file.
 *
 * @param file_path Path to the TOML configuration file.
 * @return bool Returns true if the configuration was loaded successfully, false otherwise.
 */
bool LoadConfig(const std::string& file_path) {
    try {
        auto config = toml::parse(file_path);

        // Load log directory configuration and ensure its existence.
        log_directory = toml::find_or<std::string>(config, "config.log_path", log_directory);
        if (const fs::path log_dir_path{log_directory};
            not fs::exists(log_dir_path) && not fs::create_directories(log_dir_path)) {
            std::cerr << "Failed to create log directory: " << log_directory << std::endl;
            return false;
        }

        // Load server port configuration
        server_port = toml::find_or<int>(config, "config.port", server_port);

        auto groups = toml::find<std::vector<toml::value>>(config, "group");
        for (const auto& group : groups) {
            int id = toml::find<int>(group, "id");
            std::string path = toml::find<std::string>(group, "path");
            auto args = toml::find<std::vector<std::string>>(group, "args");
            config_map[id] = std::make_pair(path, args);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

/**
 * @brief Generates a timestamp string from the current time.
 *
 * @return std::string Returns the current time formatted as a string.
 */
inline std::string GetTimestamp() {
    const auto t = std::time(nullptr);
    const auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
    return oss.str();
}

/**
 * @brief Runs a program corresponding to a specific group ID in a separate process.
 *
 * @param key The group ID for the program to run.
 * @return std::future<int> Future that will hold the exit status of the program.
 */
std::future<int> RunProgram(int key) {
    return std::async(std::launch::async, [key]() -> int {
        if (!config_map.contains(key)) {
            std::cerr << "No program configured for key: " << key << std::endl;
            return 1; // Error indication.
        }

        auto [path, args] = config_map[key];

        if (const pid_t pid = fork(); pid == 0) { // Child process
            const fs::path log_filename = fs::path(log_directory) / (GetTimestamp() + ".log");
            const int fd = open(log_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Failed to open log file");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);

            std::vector<char*> cargs;
            cargs.push_back(const_cast<char*>(path.c_str()));
            for (auto& arg : args) {
                cargs.push_back(const_cast<char*>(arg.c_str()));
            }
            cargs.push_back(nullptr);

            execvp(path.c_str(), cargs.data());
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // Parent process
            int status;
            if (waitpid(pid, &status, WNOHANG) == 0) {
                return 0; // Indicate successful start.
            } else {
                return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
            }
        } else {
            perror("fork failed");
            return 1; // Error indication.
        }
    });
}

#endif // UTILS_HPP
