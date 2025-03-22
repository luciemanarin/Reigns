cmake_minimum_required(VERSION 3.27)
project(src C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(src
        audio.c
        card.c
        config_init.c
        event.c
        game.c
        init.c
        main.c
        player.c
        resource.c)
