add_library(lexer lexer.c "${INCLUDE}/lexer/lexer.h")
target_include_directories(lexer PUBLIC "${INCLUDE}/lexer")
target_link_libraries(lexer PUBLIC list)
