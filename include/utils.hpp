// utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <unordered_map>
#include <iostream>
#include <vector>
#include <ctime>
#include <sys/wait.h>
#include <unistd.h>
#include "toml.hpp"

// 将配置存储在一个哈希表中，其中键是组号
inline std::unordered_map<int, std::pair<std::string, std::vector<std::string>>> config_map;

inline void PrintHelp() {
    std::cout << "Usage:\n";
    std::cout << "  /h or -h : Print this help message and exit.\n";
    std::cout << "  /f or -f <config_file> : Specify the configuration file.\n";
}

bool LoadConfig(const std::string& file_path) {
    try {
        auto config = toml::parse(file_path);
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

inline std::string GetTimestamp() {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
    return oss.str();
}

void RunProgram(int key) {
    if (config_map.find(key) == config_map.end()) {
        std::cerr << "No program configured for key: " << key << std::endl;
        return;
    }

    auto [path, args] = config_map[key];
    pid_t pid = fork();

    if (pid == 0) { // Child process
        std::string log_filename = GetTimestamp() + ".log";
        int fd = open(log_filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
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
        waitpid(pid, nullptr, 0); // Wait for the child process to finish
    } else {
        perror("fork failed");
    }
}

#endif // UTILS_HPP
