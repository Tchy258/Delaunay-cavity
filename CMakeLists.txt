cmake_minimum_required(VERSION 3.10...3.31.6)



project(Polylla C CXX)

add_compile_options(-fconcepts-diagnostics-depth=2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(src)
include_directories(include)
include_directories(template_implementations)
add_subdirectory(src)
set(CONCEPT_DIR ${PROJECT_SOURCE_DIR}/include/concepts)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEMPLATE_IMPL_DIR ${PROJECT_SOURCE_DIR}/template_implementations)
add_library(concepts INTERFACE 
    ${CONCEPT_DIR}
    ${CONCEPT_DIR}/mesh_details
)

add_library(vertex INTERFACE 
    ${INCLUDE_DIR}/mesh_data/structures
)
add_library(hevertex INTERFACE 
    ${INCLUDE_DIR}/mesh_data/structures
)
target_link_libraries(hevertex INTERFACE vertex)
add_library(halfedge INTERFACE 
    ${INCLUDE_DIR}/mesh_data/structures
)
add_library(hemesh INTERFACE 
    ${INCLUDE_DIR}/mesh_data
    ${TEMPLATE_IMPL_DIR}/mesh_data
)
target_link_libraries(hemesh INTERFACE hevertex concepts halfedge)
add_library(mesh_io INTERFACE 
    ${INCLUDE_DIR}/mesh_io
    ${TEMPLATE_IMPL_DIR}/mesh_io
)
target_link_libraries(mesh_io INTERFACE hemesh)
add_library(criteria INTERFACE 
    ${INCLUDE_DIR}/mesh_refiners/refinement_criteria
    ${INCLUDE_DIR}/mesh_refiners/refinement_criteria/composers
    ${TEMPLATE_IMPL_DIR}/mesh_refiners/refinement_criteria
)


add_library(refiners INTERFACE
    ${INCLUDE_DIR}/mesh_refiners
    ${INCLUDE_DIR}/mesh_refiners/delaunay_cavity
    ${INCLUDE_DIR}/mesh_refiners/polylla
    ${TEMPLATE_IMPL_DIR}/mesh_refiners
    ${TEMPLATE_IMPL_DIR}/mesh_refiners/delaunay_cavity
    ${TEMPLATE_IMPL_DIR}/mesh_refiners/polylla
)

set(Polylla -O3)

add_executable(Polylla main.cpp)

add_executable(Cavity main2.cpp)

add_executable(Polylla2 main3.cpp)

target_link_libraries(Polylla PUBLIC meshfiles)
target_link_libraries(Cavity PUBLIC meshfiles hemesh mesh_io criteria refiners)
target_link_libraries(Polylla2 PUBLIC meshfiles hemesh mesh_io refiners)
set_target_properties(meshfiles PROPERTIES LINKER_LANGUAGE CXX)
