add_library(serializing serializing.c "${INCLUDE}/utils/serializing/serializing.h")
target_include_directories(serializing PUBLIC "${INCLUDE}/utils/serializing")
