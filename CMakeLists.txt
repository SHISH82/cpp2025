cmake_minimum_required(VERSION 3.16)
project(DynamicCalculator LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")


set(PROJECT_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")


add_executable(calc
        src/main.cpp
        src/tokenizer.cpp
        src/parser.cpp
        src/eval.cpp
        src/plugins.cpp
)

target_include_directories(calc PRIVATE "${PROJECT_INCLUDE_DIR}")


if(UNIX AND NOT APPLE)
    target_link_libraries(calc PRIVATE dl)
endif()


foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set_target_properties(calc PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${cfg} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endforeach()


set(PLUGINS_OUT "${CMAKE_BINARY_DIR}/plugins")
file(MAKE_DIRECTORY "${PLUGINS_OUT}")

function(configure_plugin target source)
    add_library(${target} SHARED ${source})
    target_include_directories(${target} PRIVATE "${PROJECT_INCLUDE_DIR}")


    set_target_properties(${target} PROPERTIES
            PREFIX ""
            RUNTIME_OUTPUT_DIRECTORY "${PLUGINS_OUT}"
            LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_OUT}"
            ARCHIVE_OUTPUT_DIRECTORY "${PLUGINS_OUT}"
    )


    foreach(cfg IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
        set_target_properties(${target} PROPERTIES
                RUNTIME_OUTPUT_DIRECTORY_${cfg} "${PLUGINS_OUT}"
                LIBRARY_OUTPUT_DIRECTORY_${cfg} "${PLUGINS_OUT}"
                ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${PLUGINS_OUT}"
        )
    endforeach()


    if(WIN32)
        set_target_properties(${target} PROPERTIES SUFFIX ".dll")
    elseif(APPLE)
        set_target_properties(${target} PROPERTIES SUFFIX ".dylib")
    else()
        set_target_properties(${target} PROPERTIES SUFFIX ".so")
    endif()


    add_dependencies(calc ${target})
endfunction()


configure_plugin(funcsin  "${CMAKE_SOURCE_DIR}/plugins/funcsin.cpp")
configure_plugin(funccos  "${CMAKE_SOURCE_DIR}/plugins/funccos.cpp")
configure_plugin(funcsqrt "${CMAKE_SOURCE_DIR}/plugins/funcsqrt.cpp")
configure_plugin(funcln   "${CMAKE_SOURCE_DIR}/plugins/funcln.cpp")


option(ENABLE_ASAN "Enable AddressSanitizer for debug builds" OFF)
if(ENABLE_ASAN AND (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU"))
    message(STATUS "AddressSanitizer enabled")
    target_compile_options(calc PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(calc PRIVATE -fsanitize=address)
    foreach(p funcsin funccos funcsqrt funcln)
        target_compile_options(${p} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
        target_link_options(${p} PRIVATE -fsanitize=address)
    endforeach()
endif()
