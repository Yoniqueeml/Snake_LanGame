cmake_minimum_required(VERSION 3.25)
project(snakes)

set(CMAKE_CXX_STANDARD 14)

find_package(Curses REQUIRED)

include_directories(.)
include_directories(client)
include_directories(server)

add_executable(snakes client/client.cpp
        client/snake.cpp client/snake.h
        client/food.cpp client/food.h
        client/snake_part.cpp client/snake_part.h
        client/server.cpp client/server.h
        client/game.cpp client/game.h)
#add_executable(snakes server/server.cpp server/server.h main.cpp)
#target_link_libraries(snakes ${CURSES_LIBRARIES})
target_link_libraries(snakes ${CURSES_LIBRARIES})