cmake_minimum_required(VERSION 3.26)
project(WEEK8 C)

set(CMAKE_C_STANDARD 11)

add_executable(WEEK8 main.c
        errors.c
        errors.h
        system.c
        system.h
        message.c
        message.h)
