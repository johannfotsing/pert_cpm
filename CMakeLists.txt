cmake_minimum_required(VERSION 3.10)

# set the project name
project(pert_cpm VERSION 0.1)

add_subdirectory(src)

# add the executable
add_executable(pert_cpm src/test/driver.cpp src/test/pert.cpp)

# list of path to search for include files
target_include_directories(pert_cpm PUBLIC ${PROJECT_BINARY_DIR}/../src/include)

# configure a header to pass the version number to the source code
configure_file(src/include/pert_cpm_config.h.in src/include/pert_cpm_config.h)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)