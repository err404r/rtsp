project(rtsp_emu)
cmake_minimum_required(VERSION 2.8)

file(GLOB HDR_LIST
        *.h
)

file(GLOB SRC_LIST
        *.c
)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HDR_LIST})

