cmake_minimum_required(VERSION 3.5.1)
project(StaticYamlConfigReader)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(
        src/yaml-cpp
)
add_subdirectory(
        src/cpp-soraxas_toolbox
)

include_directories(include)
add_library(${PROJECT_NAME} src/StaticYamlConfigReader.cpp)
target_include_directories(${PROJECT_NAME} INTERFACE include)
target_link_libraries(
        ${PROJECT_NAME}
        INTERFACE
        yaml-cpp soraxas_toolbox
)

add_executable(
        test-config-reader
        test.cpp
)
target_link_libraries(test-config-reader ${PROJECT_NAME})
