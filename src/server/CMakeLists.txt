add_library(tcp_server tcp_server.c "${INCLUDE}/server/tcp_server.h")
target_include_directories(tcp_server PUBLIC "${INCLUDE}/server" PRIVATE "${INCLUDE}/utils" "${INCLUDE}/protocol")
target_link_libraries(tcp_server PRIVATE serializing protocol)
