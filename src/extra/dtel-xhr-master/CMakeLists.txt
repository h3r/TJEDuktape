cmake_minimum_required(VERSION 2.8)
project(dtel-xhr)

#enable_testing()

set(CMAKE_CXX_STANDARD 11)
add_definitions(-DDUK_OPT_CPP_EXCEPTIONS)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

find_package (Threads)
FIND_PACKAGE(Poco REQUIRED Zip Net Foundation)
#IF(Poco_FOUND)
  INCLUDE_DIRECTORIES(${Poco_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Poco_LIBRARY_DIR})
  add_definitions(-DPOCO_STATIC -DPOCO_NO_AUTOMATIC_LIBS)
#ENDIF(Poco_FOUND)


include_directories(include)
include_directories(external/dtel/include)
include_directories(external/dtel/duktape)
file(GLOB_RECURSE headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} include/*.h)

add_executable(example_runner ${CMAKE_CURRENT_SOURCE_DIR}/example/Example.cpp ${headers}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/dtel/duktape/duktape.c
    ${CMAKE_CURRENT_SOURCE_DIR}/external/dtel/duktape/duktape.h
    ${CMAKE_CURRENT_SOURCE_DIR}/external/dtel/duktape/duk_config.h
)

TARGET_LINK_LIBRARIES ( example_runner
  ${Poco_LIBRARIES}

  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_DL_LIBS}
)

SET_SOURCE_FILES_PROPERTIES( ${CMAKE_CURRENT_SOURCE_DIR}/external/dtel/duktape/duktape.c PROPERTIES LANGUAGE CXX )
