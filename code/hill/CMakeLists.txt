cmake_minimum_required(VERSION 3.1)

# 项目信息
project(hillMesh)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/tools)
set(EXTERN_DIR ${CMAKE_SOURCE_DIR}/extern)
include_directories(${EXTERN_DIR}/eigen)
include_directories(${EXTERN_DIR}/libigl/include)
# 查找所有源文件
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# 将源文件添加到项目中
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 动态库路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)


# 根据构建类型链接相应的动态库
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    link_directories(${LIBRARY_OUTPUT_PATH}/Debug)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/Debug/AFT_Tri.lib)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib/Debug/DT_Tetra.lib)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    link_directories(${LIBRARY_OUTPUT_PATH}/Release)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/Release/AFT_Tri.lib)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/Release/DT_Tetra.lib)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/extern/eigen)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/extern/libigl)