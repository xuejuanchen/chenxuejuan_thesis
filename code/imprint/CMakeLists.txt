cmake_minimum_required(VERSION 3.1)

# 项目信息
project(Imprint_test)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set(CMAKE_BUILD_TYPE "Release")

set(LIBIGL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/libigl/include)
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/libigl/external/glad/include)
set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/libigl/external/glfw/include)
set(TRIANGLE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/libigl/external/triangle)
set(EIGEN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/eigen)
add_definitions(-DNOMINMAX)

include_directories(${LIBIGL_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${GLFW_INCLUDE_DIR} ${EIGEN_INCLUDE_DIR} ${TRIANGLE_INCLUDE_DIR})
add_library(interfaceLib SHARED     
        src/interface.cpp
        src/Imprint.cpp
        src/Parameterization.cpp)
target_include_directories(interfaceLib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(interfaceLib PUBLIC IMPRINTOOL_EXPORTS)
target_compile_options(interfaceLib PUBLIC /bigobj)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
file(GLOB_RECURSE HEAD_FILES include/*.h include/*.hpp)
source_group("src" FILES ${SRC_FILES})
source_group("include" FILES ${HEAD_FILES})
target_sources(interfaceLib PRIVATE ${HEAD_FILES})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/Debug")
else()
    set(LIB_DIR "${CMAKE_SOURCE_DIR}/lib/Release")
endif()
file(GLOB LIB_FILES "${LIB_DIR}/*.lib")
target_link_libraries(interfaceLib ${LIB_FILES})
add_executable(${PROJECT_NAME} test/main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/test)
target_compile_options(${PROJECT_NAME} PUBLIC /bigobj)
target_link_libraries(${PROJECT_NAME} PRIVATE interfaceLib)

