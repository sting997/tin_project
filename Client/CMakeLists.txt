cmake_minimum_required(VERSION 3.7)
project(client)

set(CMAKE_CXX_STANDARD 11)
set(EXEC client)

set(GCC_COVERAGE_LINK_FLAGS "-lcryptopp -pthread")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(SOURCE_FILES main.cpp AuthManager.cpp AuthManager.h TicketManager.cpp TicketManager.h RequestManager.cpp RequestManager.h config.h ../protocol_codes.h ../SN/TicketDecryptor.h ../SN/TicketDecryptor.cpp ConsoleMenu.cpp ConsoleMenu.h logger.properties)
add_executable(${EXEC} ${SOURCE_FILES})

find_library(LOG4CPP log4cpp /usr/include)
find_library(LOG4CPP log4cpp /usr/lib)
find_library(LOG4CPP log4cpp /usr/local/lib)
find_library(LOG4CPP log4cpp /usr/local/include)
IF (LOG4CPP)
    target_link_libraries(${EXEC} log4cpp)
ENDIF (LOG4CPP)

find_library(CRYPTOPP crypto++ /usr/include)
find_library(CRYPTOPP crypto++ /usr/lib)
IF (CRYPTOPP)
    target_link_libraries(${EXEC} cryptopp)
ENDIF (CRYPTOPP)