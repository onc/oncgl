cmake_minimum_required(VERSION 2.8)

if(APPLE)
  set(ENV{PKG_CONFIG_PATH} ${CMAKE_SOURCE_DIR}/pkgconfig)
endif()

PROJECT( oncgl )

if(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -std=c++11")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")
endif()

# adding dir contraining a cmakelists file
add_subdirectory(lib/glfw)
add_subdirectory(lib/soil)
add_subdirectory(lib/glew)
add_subdirectory(lib/freetype)

include_directories(.)
include_directories(src/)
include_directories(lib/glew/include)
include_directories(lib/glm)
include_directories(lib/glfw/include)
include_directories(lib/soil/inc)
include_directories(lib/freetype/include)

include_directories(${Assimp_SOURCE_DIR}/include)
include_directories(${Assimp_SOURCE_DIR}/code)

link_directories(${Assimp_BINARY_DIR})
link_directories(${Assimp_BINARY_DIR}/lib/)

FILE(GLOB_RECURSE oncgl_SRCS

  lib/glew/src/glew.c

  src/*.h
  src/*.cc
  )

ADD_EXECUTABLE(oncgl ${oncgl_SRCS})

target_link_libraries(oncgl glfw ${GLFW_LIBRARIES})
target_link_libraries(oncgl soil)
target_link_libraries(oncgl freetype)
target_link_libraries(oncgl assimp)
