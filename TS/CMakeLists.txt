cmake_minimum_required(VERSION 3.7)
project(ts)

set(CMAKE_CXX_STANDARD 11)
SET(GCC_COVERAGE_LINK_FLAGS    "-lcryptopp")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
set(SOURCE_FILES main.cpp RequestManager.cpp RequestManager.h Ticket.cpp Ticket.h PrivilegeManager.cpp PrivilegeManager.h ../protocol_codes.h)
add_executable(ts ${SOURCE_FILES})