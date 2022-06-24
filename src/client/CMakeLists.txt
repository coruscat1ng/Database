add_library(tcp_client tcp_client.c "${INCLUDE}/client/tcp_client.h")
target_include_directories(tcp_client PUBLIC "${INCLUDE}/client" PRIVATE "${INCLUDE}/utils")
target_link_libraries(tcp_client PRIVATE serializing)
