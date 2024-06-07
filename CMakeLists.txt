cmake_minimum_required(VERSION 3.29)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0042 NEW)

project(native)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-DNAPI_VERSION=7)
ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)

include_directories(${CMAKE_JS_INC})
set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/native/dependencies/lib)

find_library(VBCORE_LIB NAMES VBCore PATHS ${LIBRARY_DIR})

add_library(${PROJECT_NAME} SHARED)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} ${VBCORE_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/native/dependencies/include)

target_sources(${PROJECT_NAME}
  PRIVATE 
    native/native.cpp
    native/GPUApplication.hpp
    native/GPUApplication.cpp
    ${CMAKE_JS_SRC})


if(MSVC AND CMAKE_JS_NODELIB_DEF AND CMAKE_JS_NODELIB_TARGET)
  execute_process(COMMAND ${CMAKE_AR} /def:${CMAKE_JS_NODELIB_DEF} /out:${CMAKE_JS_NODELIB_TARGET} ${CMAKE_STATIC_LINKER_FLAGS})
endif()