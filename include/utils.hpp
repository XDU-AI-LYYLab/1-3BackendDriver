//
// Created by Lhuna on 2024/9/29.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <sys/wait.h>
#include <unistd.h>

std::unordered_map<int, std::pair<std::string, std::vector<std::string>>> config_map;

void PrintHelp() {
    std::cout << "Usage:\n";
    std::cout << "  /h or -h : Print this help message and exit.\n";
    std::cout << "  /f or -f <config_file> : Specify the configuration file.\n";
}

bool LoadConfig(const std::string& file_path) {
    std::ifstream file(file_path);
    std::string line;
    int key = 1;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string path;
        if (!(iss >> path)) { continue; }
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) { args.push_back(arg); }
        config_map[key++] = {path, args};
    }
    return true;
}

std::string GetTimestamp() {
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
        dup

#endif