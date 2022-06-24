cmake_minimum_required(VERSION 3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(INCLUDE "${CMAKE_SOURCE_DIR}/include")

project(
	Tortiks
	VERSION 0.1
	DESCRIPTION "Tortiks database and server"
	LANGUAGES C)

include(CTest)

add_subdirectory(apps)

add_subdirectory(src/db)
add_subdirectory(src/utils)
add_subdirectory(src/server)
add_subdirectory(src/client)
add_subdirectory(src/protocol)
add_subdirectory(src/lexer)

add_subdirectory(tests)
