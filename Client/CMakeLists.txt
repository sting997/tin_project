cmake_minimum_required(VERSION 3.7)
project(client)

set(CMAKE_CXX_STANDARD 11)

SET(GCC_COVERAGE_LINK_FLAGS    "-lcryptopp")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
set(SOURCE_FILES main.cpp AuthManager.cpp AuthManager.h TicketManager.cpp TicketManager.h ../protocol_codes.h ../SN/TicketDecryptor.h ../SN/TicketDecryptor.cpp)
add_executable(client ${SOURCE_FILES})