cmake_minimum_required(VERSION 3.28)

project(TCatalog VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CXX_SOURCE_FILES geo.cpp domain.cpp json.cpp json_reader.cpp map_renderer.cpp request_handler.cpp svg.cpp transport_catalogue.cpp)
set(CXX_HEADER_FILES geo.h domain.h json.h json_reader.h map_renderer.h request_handler.h svg.h transport_catalogue.h)

add_executable(${PROJECT_NAME} main.cpp ${CXX_SOURCE_FILES} ${CXX_HEADER_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ./)
