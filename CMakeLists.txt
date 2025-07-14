cmake_minimum_required(VERSION 3.4)



project(Polylla C CXX)

add_compile_options(-fconcepts-diagnostics-depth=2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(src)
include_directories(src)
include_directories(include)
set(CONCEPT_DIR ${PROJECT_SOURCE_DIR}/include/concepts)
add_library(concepts STATIC 
    ${CONCEPT_DIR}/is_half_edge_vertex.hpp 
    ${CONCEPT_DIR}/mesh_data.hpp 
    ${CONCEPT_DIR}/mesh_refiner.hpp 
    ${CONCEPT_DIR}/refinement_criterion.hpp
    ${CONCEPT_DIR}/has_adjacency_constructor.hpp
)
set_target_properties(concepts PROPERTIES LINKER_LANGUAGE CXX)
add_library(vertex STATIC ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/vertex.hpp)
set_target_properties(vertex PROPERTIES LINKER_LANGUAGE CXX)
add_library(hevertex STATIC ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/he_vertex.hpp)
set_target_properties(hevertex PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(hevertex PUBLIC vertex)
add_library(halfedge STATIC ${PROJECT_SOURCE_DIR}/include/mesh_data/structures/half_edge.hpp)
set_target_properties(halfedge PROPERTIES LINKER_LANGUAGE CXX)
add_library(hemesh STATIC ${PROJECT_SOURCE_DIR}/include/mesh_data/half_edge_mesh.hpp)
target_link_libraries(hemesh PUBLIC hevertex concepts halfedge)
set_target_properties(hemesh PROPERTIES LINKER_LANGUAGE CXX)
add_library(mesh_io STATIC 
    ${PROJECT_SOURCE_DIR}/include/mesh_io/mesh_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/node_ele_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/off_reader.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/mesh_writer.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_io/off_writer.hpp
)
target_link_libraries(mesh_io PUBLIC hemesh)
set_target_properties(mesh_io PROPERTIES LINKER_LANGUAGE CXX)
add_library(criteria STATIC 
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/min_angle_criterion.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/min_angle_criterion_robust.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/composers/and_criteria.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/refinement_criteria/composers/or_criteria.hpp
)
set_target_properties(criteria PROPERTIES LINKER_LANGUAGE CXX)

add_library(refiners STATIC
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/mesh_refiner.hpp
    ${PROJECT_SOURCE_DIR}/include/mesh_refiners/delaunay_cavity_refiner.hpp
)
set_target_properties(refiners PROPERTIES LINKER_LANGUAGE CXX)

set(Polylla -O3)

add_executable(Polylla main.cpp)

add_executable(Cavity main2.cpp)

target_link_libraries(Polylla PUBLIC meshfiles)
target_link_libraries(Cavity PUBLIC meshfiles hemesh mesh_io criteria refiners)
set_target_properties(meshfiles PROPERTIES LINKER_LANGUAGE CXX)
