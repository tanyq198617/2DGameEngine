project(spine-c)

file(GLOB HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/spine/*.h)
source_group(include\\spine FILES ${HEADER_FILES})
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/spine/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/spine/*.c)
source_group(src\\spine FILES ${SRC_FILES})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
add_library(spine-c STATIC ${HEADER_FILES} ${SRC_FILES})
