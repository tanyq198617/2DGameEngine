# message("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
set(ENGINE_INCLUDE_PATHS 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/application"
    "${CMAKE_CURRENT_SOURCE_DIR}/base"
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/bmfont"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/HLFont"
    "${CMAKE_CURRENT_SOURCE_DIR}/core"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/components"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton/spine"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/eventdispatcher"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/glhelpers"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui"
    "${CMAKE_CURRENT_SOURCE_DIR}/gui/RichLabel"
    "${CMAKE_CURRENT_SOURCE_DIR}/textures"
)

if (NOT EMSCRIPTEN)
    list(APPEND ENGINE_INCLUDE_PATHS
        "${CMAKE_CURRENT_SOURCE_DIR}/network"
        "${CMAKE_CURRENT_SOURCE_DIR}/jsscripting"
        "${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jsbindings"
        "${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jswrappers"
    )
endif()

if (EMSCRIPTEN)
    list(APPEND ENGINE_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/emscripten")
    file(GLOB_RECURSE PLATFORM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/emscripten/*.cpp)
    source_group(emscripten FILES ${PLATFORM_SRC_FILES})
elseif (ANDROID)
    list(APPEND ENGINE_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/android")
    list(APPEND ENGINE_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/android/jni")
    file(GLOB_RECURSE PLATFORM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/android/*.cpp)
    source_group(android FILES ${PLATFORM_SRC_FILES})
elseif (APPLE)
    if (BUILD_PLATFORM_APPLE_IOS)
        list(APPEND ENGINE_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/ios")
        file(GLOB_RECURSE PLATFORM_SRC_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR}/ios/*.h
            ${CMAKE_CURRENT_SOURCE_DIR}/ios/*.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/ios/*.m 
            ${CMAKE_CURRENT_SOURCE_DIR}/ios/*.mm
        )
        source_group(ios FILES ${PLATFORM_SRC_FILES})
    else()
        list(APPEND ENGINE_INCLUDE_PATHS "${CMAKE_CURRENT_SOURCE_DIR}/mac")
        file(GLOB_RECURSE PLATFORM_SRC_FILES 
            ${CMAKE_CURRENT_SOURCE_DIR}/mac/*.h
            ${CMAKE_CURRENT_SOURCE_DIR}/mac/*.cpp 
            ${CMAKE_CURRENT_SOURCE_DIR}/mac/*.m 
            ${CMAKE_CURRENT_SOURCE_DIR}/mac/*.mm
        )
        list(APPEND PLATFORM_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ios/HLUserDefaults.mm)
        source_group(mac FILES ${PLATFORM_SRC_FILES})
    endif()
endif()

set(ENGINE_INCLUDE_PATHS ${ENGINE_INCLUDE_PATHS} PARENT_SCOPE)

set(SRC_FILES ${PLATFORM_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/application/*.h ${CMAKE_CURRENT_SOURCE_DIR}/application/*.cpp)
source_group(application FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/base/*.h ${CMAKE_CURRENT_SOURCE_DIR}/base/*.cpp)
source_group(base FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/common/*.h ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp)
source_group(common FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/common/bmfont/*.h ${CMAKE_CURRENT_SOURCE_DIR}/common/bmfont/*.cpp)
source_group(common\\bmfont FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/common/HLFont/*.h ${CMAKE_CURRENT_SOURCE_DIR}/common/HLFont/*.cpp)
source_group(common\\HLFont FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp)
source_group(core FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/components/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/components/*.cpp)
source_group(core\\components FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton/*.cpp)
source_group(core\\components\\skeleton FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton/spine/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/components/skeleton/spine/*.cpp)
source_group(core\\components\\skeleton\\spine FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/eventdispatcher/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/eventdispatcher/*.cpp)
source_group(core\\eventdispatcher FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/*.h ${CMAKE_CURRENT_SOURCE_DIR}/core/utils/*.cpp)
source_group(core\\utils FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/glhelpers/*.h ${CMAKE_CURRENT_SOURCE_DIR}/glhelpers/*.cpp)
source_group(glhelpers FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.h ${CMAKE_CURRENT_SOURCE_DIR}/gui/*.cpp)
source_group(gui FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/gui/RichLabel/*.h ${CMAKE_CURRENT_SOURCE_DIR}/gui/RichLabel/*.cpp)
source_group(gui\\RichLabel FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/textures/*.h ${CMAKE_CURRENT_SOURCE_DIR}/textures/*.cpp)
source_group(textures FILES ${TMP_SRC_FILES})
list(APPEND SRC_FILES ${TMP_SRC_FILES})

if (NOT EMSCRIPTEN)
    if (APPLE)
        file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/network/*.h ${CMAKE_CURRENT_SOURCE_DIR}/network/*.mm)
        list(APPEND TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/network/HLHttpRequest.cpp)
        source_group(network FILES ${TMP_SRC_FILES})
        list(APPEND SRC_FILES ${TMP_SRC_FILES})
    else()
        file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/network/*.h ${CMAKE_CURRENT_SOURCE_DIR}/network/*.cpp)
        source_group(network FILES ${TMP_SRC_FILES})
        list(APPEND SRC_FILES ${TMP_SRC_FILES})
    endif()
    file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/*.h ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/*.cpp)
    source_group(jsscripting FILES ${TMP_SRC_FILES})
    list(APPEND SRC_FILES ${TMP_SRC_FILES})
    file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jsbindings/*.h ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jsbindings/*.cpp)
    source_group(jsscripting\\jsbindings FILES ${TMP_SRC_FILES})
    list(APPEND SRC_FILES ${TMP_SRC_FILES})
    file(GLOB TMP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jswrappers/*.h ${CMAKE_CURRENT_SOURCE_DIR}/jsscripting/jswrappers/*.cpp)
    source_group(jsscripting\\jswrappers FILES ${TMP_SRC_FILES})
    list(APPEND SRC_FILES ${TMP_SRC_FILES})
endif()

include_directories(${LIB_INCLUDE_PATHS} ${ENGINE_INCLUDE_PATHS})

add_library(HoolaiEngine STATIC ${SRC_FILES})

set_target_properties(
    HoolaiEngine
    PROPERTIES
    XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Debug]
    YES
)

if (APPLE)
    if (BUILD_PLATFORM_APPLE_IOS)
    else()
        add_framework(JavaScriptCore HoolaiEngine)
        add_framework(OpenGL HoolaiEngine)
        add_framework(Cocoa HoolaiEngine)
    endif()
endif()

# target_link_libraries(HoolaiEngine kazmath spine-c OpenThreads protobuf-lite)
