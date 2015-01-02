project(png)

set(Z_DIR ../z)

include_directories(include ${Z_DIR}/include)
aux_source_directory(src SRC)
link_directories(${Z_DIR}/bin)
target_link_libraries(z)
add_library(png ${SRC})
set(LIBRARY_OUTPUT_PATH ${png_SOURCE_DIR}/bin)
