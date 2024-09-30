cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(BackendDriver VERSION 1.0 LANGUAGES CXX)

# 设置C++标准为C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找./source目录下的所有.cpp文件
file(GLOB_RECURSE SOURCES "source/*.cpp")

# 查找./include目录下的所有.hpp文件
file(GLOB_RECURSE HEADERS "include/*.hpp")

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 引入Crow库
include(FetchContent)

FetchContent_Declare(
        crow
        GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
        GIT_TAG v1.2.0  # 或者使用最新的tag或commit hash
)

FetchContent_MakeAvailable(crow)

# 链接Crow库到项目
target_link_libraries(${PROJECT_NAME} PUBLIC Crow::Crow)

# Set the features for the Crow dependency
set(CROW_FEATURES ssl compression)

# Import lib toml11
FetchContent_Declare(
        toml11
        GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
        GIT_TAG        v4.2.0
)

FetchContent_MakeAvailable(toml11)

target_link_libraries(${PROJECT_NAME} PRIVATE toml11::toml11)

# 添加include目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)