cmake_minimum_required (VERSION 3.12)

project(hello_render)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/header)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
SET (RENDER_SOURCE
    pbrt_func.cpp
    render.cpp
    pbrt_render.cpp
    main.cpp
)
SET (RENDER_SOURCE_HEADERS
    pbrt_func.h
    render.h
    pbrt_render.h
)

add_executable (main ${RENDER_SOURCE})
target_link_libraries(main ${CMAKE_SOURCE_DIR}/libpbrt_lib.a)