set(HEADERS "${INCLUDE}/protocol/protocol.h")

add_library(protocol ${HEADERS} protocol.c)
target_include_directories(protocol PUBLIC "${INCLUDE}/protocol" PRIVATE "${INCLUDE}/utils")
target_link_libraries(protocol PRIVATE serializing)
