set(LIB_INCLUDE_PATHS 
    "${CMAKE_CURRENT_SOURCE_DIR}/json"
    "${CMAKE_CURRENT_SOURCE_DIR}/kazmath/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/OpenThreads/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/protobuf/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/spine-c/include"
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY=1" PARENT_SCOPE)

if (EMSCRIPTEN)
    list(APPEND LIB_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/webp/emscripten/include")
    list(APPEND LIB_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/jpeg-turbo/emscripten/include")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_ZLIB=1 -s USE_LIBPNG=1 -DGOOGLE_PROTOBUF_NO_THREAD_SAFETY=1" PARENT_SCOPE)
elseif (APPLE)
    if (BUILD_PLATFORM_APPLE_IOS)
    else()
        list(APPEND LIB_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/png/mac/include")
        list(APPEND LIB_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/webp/mac/include")
    endif()
    list(APPEND LIB_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/jpeg-turbo/darwin/include")
endif()

set(LIB_INCLUDE_PATHS ${LIB_INCLUDE_PATHS} PARENT_SCOPE)

if (NOT EMSCRIPTEN)
    add_subdirectory(OpenThreads)
endif()
add_subdirectory(spine-c)
add_subdirectory(kazmath)
add_subdirectory(protobuf)

if (EMSCRIPTEN)
    add_library(jpeg STATIC IMPORTED GLOBAL)
    set_target_properties(  jpeg
                            PROPERTIES IMPORTED_LOCATION
                            ${CMAKE_CURRENT_SOURCE_DIR}/jpeg-turbo/emscripten/lib/libjpeg.a)
    add_library(webp STATIC IMPORTED GLOBAL)
    set_target_properties(  webp
                            PROPERTIES IMPORTED_LOCATION
                        ${CMAKE_CURRENT_SOURCE_DIR}/webp/emscripten/lib/libwebp.a)
elseif (ANDROID)
    
elseif (APPLE)
    add_library(jpeg STATIC IMPORTED GLOBAL)
    set_target_properties(  jpeg
                            PROPERTIES IMPORTED_LOCATION
                            ${CMAKE_CURRENT_SOURCE_DIR}/jpeg-turbo/darwin/lib/libjpeg.a)
    if (BUILD_PLATFORM_APPLE_IOS)
        add_library(png STATIC IMPORTED GLOBAL)
        set_target_properties(  png
                                PROPERTIES IMPORTED_LOCATION
                                ${CMAKE_CURRENT_SOURCE_DIR}/png/ios/lib/libpng.a)
        add_library(webp STATIC IMPORTED GLOBAL)
        set_target_properties(  webp
                                PROPERTIES IMPORTED_LOCATION
                                ${CMAKE_CURRENT_SOURCE_DIR}/webp/ios/lib/libwebp.a)
    else()
        add_library(png STATIC IMPORTED GLOBAL)
        set_target_properties(  png
                                PROPERTIES IMPORTED_LOCATION
                                ${CMAKE_CURRENT_SOURCE_DIR}/png/mac/lib/libpng.a)
        add_library(webp STATIC IMPORTED GLOBAL)
        set_target_properties(  webp
                                PROPERTIES IMPORTED_LOCATION
                                ${CMAKE_CURRENT_SOURCE_DIR}/webp/mac/lib/libwebp.a)
    endif()
endif()
