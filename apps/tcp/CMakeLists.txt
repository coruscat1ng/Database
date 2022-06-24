add_executable(server server.c server_core.h server_core.c messages.h messages.c)
target_link_libraries(server PRIVATE tcp_server db serializing protocol queue avltree)

add_executable(client client.c client_core.h client_core.c messages.h messages.c)
target_link_libraries(client PRIVATE tcp_client db serializing protocol queue avltree)
