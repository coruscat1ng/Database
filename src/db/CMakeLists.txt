set(HEADERS core.h "${INCLUDE}/db/dbserial.h" "${INCLUDE}/db/db.h" "${INCLUDE}/db/dbgen.h" "${INCLUDE}/db/data.h")

add_library(db ${HEADERS} core.c db.c dbgen.c dbserial.c data.c)
target_include_directories(db PUBLIC "${INCLUDE}/db" PRIVATE "${INCLUDE}/utils")
target_link_libraries(db PRIVATE avltree serializing)
