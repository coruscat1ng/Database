add_library(list "${INCLUDE}/utils/list/list.h" list.c)
target_include_directories(list PUBLIC "${INCLUDE}/utils/list")
target_link_libraries(list PUBLIC binary_node)
