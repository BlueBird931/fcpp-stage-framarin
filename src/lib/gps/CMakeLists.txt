cmake_minimum_required(VERSION 3.10)
project(gps_trace_test)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find RapidXML path - adjust this to where you actually have RapidXML
set(RAPIDXML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/rapidxml-1.13")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}  # For local headers
    ${RAPIDXML_DIR}              # For RapidXML headers
)

# Add source files
add_executable(gps_trace_test
    gps_trace.cpp
    test_main.cpp
)

# Copy test data if it exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_data")
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_data DESTINATION ${CMAKE_BINARY_DIR})
endif()