cmake_minimum_required(VERSION 3.7)
project(ts)

set(CMAKE_CXX_STANDARD 11)
set(EXEC ts)

set(GCC_COVERAGE_LINK_FLAGS "-lcryptopp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(SOURCE_FILES main.cpp RequestManager.cpp RequestManager.h Ticket.cpp Ticket.h PrivilegeManager.cpp PrivilegeManager.h ../protocol_codes.h config.h DBManager.cpp DBManager.h)
add_executable(${EXEC} ${SOURCE_FILES})

find_library(CRYPTOPP crypto++ /usr/include)
find_library(CRYPTOPP crypto++ /usr/lib)
IF (CRYPTOPP)
    target_link_libraries(${EXEC} cryptopp)
ENDIF (CRYPTOPP)