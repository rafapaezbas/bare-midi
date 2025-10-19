cmake_minimum_required(VERSION 3.31)

find_package(cmake-bare REQUIRED PATHS node_modules/cmake-bare)
find_package(cmake-ports REQUIRED PATHS node_modules/cmake-ports)

project(bare_midi C)

find_port(portmidi)

add_bare_module(bare_midi)

target_sources(
    ${bare_midi}
    PRIVATE
        binding.c
)

target_link_libraries(
    ${bare_midi}
    PUBLIC
        portmidi
)

if(APPLE)
    target_link_libraries(
        ${bare_midi}
        PRIVATE
            "-framework CoreMIDI"
            "-framework CoreFoundation"
    )
endif()

if(NOT APPLE AND NOT WIN32)
    find_library(ALSA_LIB NAMES asound)
    if(ALSA_LIB)
        target_link_libraries(
            ${bare_midi}
            PRIVATE
                ${ALSA_LIB}
        )
    endif()
endif()
