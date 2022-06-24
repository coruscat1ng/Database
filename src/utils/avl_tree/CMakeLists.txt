add_library(avltree avl_tree.c 
	"${INCLUDE}/utils/avl_tree/avl_tree.h" 
	"${INCLUDE}/utils/binary_tree/binary_tree.h"
	)
target_include_directories(avltree PUBLIC "${INCLUDE}/utils/avl_tree")
target_link_libraries(avltree PUBLIC binarytree PRIVATE stack)

