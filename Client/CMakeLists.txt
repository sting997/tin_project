cmake_minimum_required(VERSION 3.7)
project(client)

set(CMAKE_CXX_STANDARD 11)
set(EXEC client)

set(GCC_COVERAGE_LINK_FLAGS "-lcryptopp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
SET(SOURCE_FILES main.cpp AuthManager.cpp AuthManager.h TicketManager.cpp TicketManager.h RequestManager.cpp RequestManager.h ../protocol_codes.h ../SN/TicketDecryptor.h ../SN/TicketDecryptor.cpp)
add_executable(${EXEC} ${SOURCE_FILES})

find_library(CRYPTOPP crypto++ /usr/lib)
IF (CRYPTOPP)
    target_link_libraries(${EXEC} cryptopp)
ENDIF (CRYPTOPP)