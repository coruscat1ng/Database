add_library(binarytree binary_tree.c "${INCLUDE}/utils/binary_tree/binary_tree.h")
target_include_directories(binarytree PUBLIC "${INCLUDE}/utils/binary_tree")
target_link_libraries(binarytree PRIVATE stack)
