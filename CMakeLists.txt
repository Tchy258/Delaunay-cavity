cmake_minimum_required(VERSION 3.10...3.31.6)



project(Polylla C CXX)

add_compile_options(-fconcepts-diagnostics-depth=2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(src)
include_directories(include)
add_subdirectory(src)
set(CONCEPT_DIR ${PROJECT_SOURCE_DIR}/include/concepts)
add_library(concepts INTERFACE 
    ${CONCEPT_DIR}/is_half_edge_vertex.hpp 
    ${CONCEPT_DIR}/mesh_data.hpp 
    ${CONCEPT_DIR}/refinement_criterion.hpp
    ${CONCEPT_DIR}/has_adjacency_constructor.hpp
)

add_library(vertex INTERFACE ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/vertex.hpp)
add_library(hevertex INTERFACE ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/he_vertex.hpp)
target_link_libraries(hevertex INTERFACE vertex)
add_library(halfedge INTERFACE ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/half_edge.hpp)
add_library(hemesh INTERFACE ${PROJECT_SOURCE_DIR}/include/mesh_data/half_edge_mesh.hpp)
target_link_libraries(hemesh INTERFACE hevertex concepts halfedge)
add_library(mesh_io INTERFACE 
    ${PROJECT_SOURCE_DIR}/include/mesh_io/mesh_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/node_ele_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/off_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/mesh_writer.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/off_writer.hpp
)
target_link_libraries(mesh_io INTERFACE hemesh)
add_library(criteria INTERFACE 
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/min_angle_criterion.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/min_angle_criterion_robust.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/composers/and_criteria.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/composers/or_criteria.hpp
)

add_library(refiners INTERFACE
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/mesh_refiner.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/delaunay_cavity_refiner.hpp
)

set(Polylla -O3)

add_executable(Polylla main.cpp)

add_executable(Cavity main2.cpp)

target_link_libraries(Polylla PUBLIC meshfiles)
target_link_libraries(Cavity PUBLIC meshfiles hemesh mesh_io criteria refiners)
set_target_properties(meshfiles PROPERTIES LINKER_LANGUAGE CXX)
