add_executable(test_db test_db.c)

target_link_libraries(test_db PRIVATE db)
add_test(NAME test.db COMMAND test_db)

add_executable(test_avltree test_avltree.c)
target_link_libraries(test_avltree PRIVATE avltree)
add_test(NAME test.avltree COMMAND test_avltree)

add_executable(test_serial test_serial.c)
target_link_libraries(test_serial PRIVATE serializing)
add_test(NAME test.serial COMMAND test_serial)

add_executable(test_server test_server.c)
target_link_libraries(test_server PRIVATE tcp_server)
add_test(NAME test.server COMMAND test_server)

add_executable(test_client test_client.c)
target_link_libraries(test_client PRIVATE tcp_client)
add_test(NAME test.client COMMAND test_client)


add_executable(test_lexer test_lexer.c)
target_link_libraries(test_lexer PRIVATE lexer)
add_test(NAME test.lexer COMMAND test_lexer)
