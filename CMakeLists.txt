cmake_minimum_required(VERSION 3.5.1)
project(StaticYamlConfigReader)

set(CMAKE_CXX_STANDARD 17)

function(target_link_libraries_system target)
  set(libs ${ARGN})
  foreach(lib ${libs})
    get_target_property(lib_include_dirs ${lib} INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories(${target} SYSTEM PRIVATE ${lib_include_dirs})
    target_link_libraries(${target} ${lib})
  endforeach(lib)
endfunction(target_link_libraries_system)

add_subdirectory(src)

include_directories(include)
add_library(${PROJECT_NAME} src/StaticYamlConfigReader.cpp)
target_include_directories(${PROJECT_NAME} INTERFACE include)
find_package(yaml-cpp)
target_link_libraries_system(
        ${PROJECT_NAME}
        yaml-cpp
        soraxas_toolbox
)

# add_executable(
#         test-config-reader
#         test.cpp
# )
# target_link_libraries(test-config-reader ${PROJECT_NAME})
