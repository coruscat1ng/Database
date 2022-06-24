add_library(stack stack.c "${INCLUDE}/utils/stack/stack.h")
target_include_directories(stack PUBLIC "${INCLUDE}/utils/stack")
