cmake_minimum_required(VERSION 3.10)

project(execucaoCG)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)

find_package(glfw3 REQUIRED)

include_directories(include)

set(SOURCES 
    main.cpp 
    glad.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL 
    glfw
    ${CMAKE_DL_LIBS}
)