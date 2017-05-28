cmake_minimum_required(VERSION 3.7)
project(sn)

set(CMAKE_CXX_STANDARD 11)
set(EXEC sn)

set(GCC_COVERAGE_LINK_FLAGS "-lcryptopp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
set(SOURCE_FILES RequestManager.cpp RequestManager.h TicketCorrectnessTester.cpp TicketCorrectnessTester.h TicketDecryptor.cpp TicketDecryptor.h ../protocol_codes.h main.cpp)
add_executable(${EXEC} ${SOURCE_FILES})

find_library(CRYPTOPP crypto++ /usr/lib)
IF (CRYPTOPP)
    target_link_libraries(${EXEC} cryptopp)
ENDIF (CRYPTOPP)