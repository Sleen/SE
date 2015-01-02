project(minizip)

set(Z_DIR ../z)

include_directories(include ${Z_DIR}/include)
aux_source_directory(src SRC)
link_directories(${Z_DIR}/bin)
target_link_libraries(z)
add_library(minizip ${SRC})
set(LIBRARY_OUTPUT_PATH ${minizip_SOURCE_DIR}/bin)
