cmake_minimum_required(VERSION 3.14)

# 设置项目名称和版本
project(BackendDriver VERSION 1.0 LANGUAGES CXX)

# 添加可执行文件，明确指定源文件和头文件
add_executable(${PROJECT_NAME}
        source/server.cpp
        include/utils.hpp
)

# 使用现代CMake指令设置C++标准
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

# 为不同的编译器设置静态链接选项
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # 对于 GCC 和 Clang，静态链接标准库和所有其他库
    target_link_options(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
endif()

if(MSVC)
    # 对于 MSVC，使用MT标志代替MD标志进行静态链接
    foreach(flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
    endforeach()
endif()

# 定义一个函数来处理FetchContent的声明和可用性
function(fetch_content_declare_and_make_available name git_repository tag)
    include(FetchContent)
    FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${git_repository}
            GIT_TAG        ${tag}
    )
    FetchContent_MakeAvailable(${name})
endfunction()

# 引入Crow库
fetch_content_declare_and_make_available(
        crow
        https://github.com/CrowCpp/Crow.git
        v1.2.0  # 使用最新的tag或commit hash
)

# 引入toml11库
fetch_content_declare_and_make_available(
        toml11
        https://github.com/ToruNiina/toml11.git
        v4.2.0
)

# 链接外部库到项目
target_link_libraries(${PROJECT_NAME} PUBLIC Crow::Crow PRIVATE toml11::toml11)

# 添加include目录
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
