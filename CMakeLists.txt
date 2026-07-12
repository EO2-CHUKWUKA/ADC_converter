cmake_minimum_required(VERSION 4.2)
project(ADC_ANALYSER)

set(CMAKE_CXX_STANDARD 20)

add_executable(ADC_ANALYSER main.c
        main.c
        io.c
        io.h
        stats.c
        stats.h
        adc.c
        adc.h

)
