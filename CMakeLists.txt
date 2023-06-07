cmake_minimum_required(VERSION 3.23)
project(server)

set(CMAKE_CXX_STANDARD 14)

find_package(Curses REQUIRED)

include_directories(.)
include_directories(client)
include_directories(server)

add_executable(client client/client.cpp
        client/snake.cpp client/snake.h
        client/food.cpp client/food.h
       client/snake_part.cpp client/snake_part.h
        client/server.cpp client/server.h
       client/game.cpp client/game.h)
#add_executable(server server/server_main.cpp server/server.cpp server/server.h
 #               server/snake.cpp server/snake.h
 #               server/food.cpp server/food.h
#                server/snake_part.cpp server/snake_part.h
 #              server/game.cpp server/game.h)
#target_link_libraries(client ${CURSES_LIBRARIES})
target_link_libraries(client ${CURSES_LIBRARIES})