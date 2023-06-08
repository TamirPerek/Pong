cmake_minimum_required(VERSION 3.21.0)
project(Pong VERSION 0.1.0 LANGUAGES C)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package(SDL REQUIRED)
include_directories(${SDL_INCLUDE_DIRS})

add_executable(
    ${PROJECT_NAME}
    pong.c
)

target_link_libraries(
    ${PROJECT_NAME}
    ${SDL_LIBRARIES}
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/img/ $<TARGET_FILE_DIR:${PROJECT_NAME}>/img/)
