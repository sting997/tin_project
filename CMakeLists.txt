cmake_minimum_required(VERSION 3.7)
project(tin)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Client/RequestManager.cpp Client/RequestManager.h Client/AuthManager.cpp Client/AuthManager.h Client/TicketManager.cpp Client/TicketManager.h SN/RequestManager.cpp SN/RequestManager.h SN/TicketCorectnessTester.cpp SN/TicketCorectnessTester.h SN/TicketDecryptor.cpp SN/TicketDecryptor.h TS/RequestManager.cpp TS/RequestManager.h TS/Ticket.cpp TS/Ticket.h TS/PrivilegeManager.cpp TS/PrivilegeManager.h)
add_executable(main ${SOURCE_FILES})