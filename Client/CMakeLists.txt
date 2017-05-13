cmake_minimum_required(VERSION 3.7)
project(client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp RequestManager.cpp RequestManager.h AuthManager.cpp AuthManager.h TicketManager.cpp TicketManager.h)
add_executable(client ${SOURCE_FILES})