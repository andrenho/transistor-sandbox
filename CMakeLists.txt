#
# project definitions
#

cmake_minimum_required(VERSION 3.28)

project(transistor-sandbox
        VERSION 0.1.0
        DESCRIPTION "Sandbox engine for transistor project"
        LANGUAGES C CXX
)

#
# configuration
#

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# warnings / flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(warnings -Wall -Wextra -Wformat-nonliteral -Wshadow -Wwrite-strings -Wmissing-format-attribute -Wswitch-enum -Wmissing-noreturn -Wno-unused-parameter -Wno-unused)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(warnings ${warnings} -Wsuggest-attribute=pure -Wsuggest-attribute=const -Wsuggest-attribute=noreturn -Wsuggest-attribute=malloc -Wsuggest-attribute=format -Wsuggest-attribute=cold)
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb -O0)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(DEF B_PRODUCTION_MODE=ON)
    add_compile_options(-Ofast -flto)
endif()

add_compile_definitions(
        PROJECT_VERSION=\"${PROJECT_VERSION}\"
        PROJECT_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        PROJECT_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        PROJECT_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

#
# external libraries
#

find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED luajit)

#
# library
#

add_library(${PROJECT_NAME} STATIC
        contrib/stb/stb_ds.h
        src/sandbox/sandbox.h
        src/sandbox/sandbox.c
        src/board/board.c
        src/board/board.h
        src/util/response.h
        src/util/serialize.h
        src/board/wire.h
        src/board/position.c
        src/board/position.h
        src/transistor-sandbox.h
        src/board/wire.c
        src/util/minmax.h
        src/component/componentdb.c
        src/component/componentdb.h
        src/component/componentdef.h
        src/component/defaultcomponents.h
        src/component/defaultcomponents.c
)

target_compile_options(${PROJECT_NAME} PRIVATE ${warnings})
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/contrib/stb
        ${LUA_INCLUDE_DIRS})

#
# executable
#

add_executable(${PROJECT_NAME}-cli EXCLUDE_FROM_ALL
        exe/main.c)

target_compile_options(${PROJECT_NAME}-cli PRIVATE ${warnings})
target_link_directories(${PROJECT_NAME}-cli PRIVATE ${LUA_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME}-cli PRIVATE ${PROJECT_NAME} ${LUA_LIBRARIES})

#
# testing
#

add_executable(${PROJECT_NAME}-tests EXCLUDE_FROM_ALL
        contrib/doctest/doctest.h
        tests/serialization.cc
        tests/position.cc
)
target_compile_options(${PROJECT_NAME}-tests PRIVATE ${warnings} )
target_link_directories(${PROJECT_NAME}-tests PRIVATE ${LUA_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME}-tests PRIVATE ${PROJECT_NAME} ${LUA_LIBRARIES})
target_include_directories(${PROJECT_NAME}-tests PRIVATE contrib/doctest)

enable_testing()
add_test(${PROJECT_NAME}-tests ${PROJECT_NAME}-tests)
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS ${PROJECT_NAME}-tests)

#
# leaks
#

add_custom_target(${PROJECT_NAME}-test-leaks)
add_custom_command(TARGET ${PROJECT_NAME}-test-leaks
        POST_BUILD
        COMMENT "Check for leaks using valgrind."
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS ${PROJECT_NAME}-tests
        COMMAND valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=${CMAKE_SOURCE_DIR}/valgrind.supp ./${PROJECT_NAME}-tests
)
add_dependencies(${PROJECT_NAME}-test-leaks ${PROJECT_NAME}-tests)
