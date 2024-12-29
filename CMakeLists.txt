cmake_minimum_required(VERSION 3.28)
project(123)

set(CMAKE_CXX_STANDARD 17)

add_executable(123 main.cpp
        svg.cpp
        transport_catalogue.cpp
        request_handler.cpp
        map_renderer.cpp
        json_reader.cpp
        json.cpp
        geo.cpp
        domain.cpp)
