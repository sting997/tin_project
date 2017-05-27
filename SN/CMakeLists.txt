cmake_minimum_required(VERSION 3.7)
project(sn)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES RequestManager.cpp RequestManager.h TicketCorrectnessTester.cpp TicketCorrectnessTester.h TicketDecryptor.cpp TicketDecryptor.h ../protocol_codes.h main.cpp)
add_executable(sn ${SOURCE_FILES})