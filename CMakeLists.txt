cmake_minimum_required(VERSION 3.10...3.31.6)

project(Polylla C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-fconcepts-diagnostics-depth=2)

include_directories(src include template_implementations)

add_subdirectory(src)

set_target_properties(meshfiles PROPERTIES LINKER_LANGUAGE CXX)


set(CONCEPT_DIR ${PROJECT_SOURCE_DIR}/include/concepts)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(TEMPLATE_IMPL_DIR ${PROJECT_SOURCE_DIR}/template_implementations)

# ---------------------------
# Libraries
# ---------------------------
add_library(concepts INTERFACE ${CONCEPT_DIR} ${CONCEPT_DIR}/mesh_details)
add_library(vertex INTERFACE ${INCLUDE_DIR}/mesh_data/structures)
add_library(hevertex INTERFACE ${INCLUDE_DIR}/mesh_data/structures)
target_link_libraries(hevertex INTERFACE vertex)
add_library(halfedge INTERFACE ${INCLUDE_DIR}/mesh_data/structures)
add_library(hemesh INTERFACE ${INCLUDE_DIR}/mesh_data
    ${TEMPLATE_IMPL_DIR}/mesh_data)
target_link_libraries(hemesh INTERFACE hevertex concepts halfedge)
add_library(mesh_io INTERFACE ${INCLUDE_DIR}/mesh_io ${TEMPLATE_IMPL_DIR}/mesh_io)
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

add_subdirectory(extern/CLI11)

# ---------------------------
# Macro options
# ---------------------------
set(MESH_TYPE_OPTIONS HalfEdgeMesh)
set(MESH_REFINER_OPTIONS DelaunayCavityRefiner PolyllaRefiner)
set(TRIANGLE_COMPARATOR_BASE_T_OPTIONS NullComparator EdgeLengthComparator AngleComparator AreaComparator RandomComparator)
set(TRIANGLE_COMPARATOR_SORT_ASCENDING_OPTIONS true false)
set(TRIANGLE_COMPARATOR_SORT_KEY_OPTIONS true false)
set(MERGING_STRATEGY_T_OPTIONS ExcludePreviousCavitiesStrategy MergeTrianglesWithBestConvexityStrategy MergeTrianglesIntoSmallestNeighbor MergeTrianglesIntoBiggestNeighbor)
set(REFINEMENT_CRITERION_T_OPTIONS NullRefinementCriterion MinAngleCriterionRobust MinAngleCriterion MinAreaCriterion MinArea2Criterion)
# ---------------------------
# Map MESH_TYPE to header
# ---------------------------
set(MESH_TYPE_HEADER_HalfEdgeMesh "<mesh_data/half_edge_mesh.hpp>")

# -----------------------------
# Map SORT_KEY_OPTIONS to comparator
# -----------------------------
set(SORT_KEY_true_EdgeLengthComparator max_edge)
set(SORT_KEY_false_EdgeLengthComparator min_edge)
set(SORT_KEY_true_AreaComparator precise)
set(SORT_KEY_false_AreaComparator doubled)
set(SORT_KEY_true_AngleComparator min_angle)
set(SORT_KEY_false_AngleComparator max_angle)


# ---------------------------
# Function to convert CamelCase to snake_case (CMake version)
# ---------------------------
function(camel_to_snake input_var output_var)
    string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1_\\2" tmp "${${input_var}}")
    string(REGEX REPLACE "([A-Z]+)([A-Z][a-z])" "\\1_\\2" tmp "${tmp}")
    string(TOLOWER "${tmp}" result)
    set(${output_var} "${result}" PARENT_SCOPE)
endfunction()
# ---------------------------
# Function to create target with macro combo
# ---------------------------
function(add_configured_executable main_file refiner mesh tri_base merge is_ascending alt_sort_key refinement_criterion)
    # Convert all parts to snake_case
    camel_to_snake(mesh mesh_snake)
    camel_to_snake(tri_base tri_snake)
    camel_to_snake(merge merge_snake)
    camel_to_snake(refinement_criterion refinement_criterion_snake)
    camel_to_snake(refiner refiner_snake)
    if(NOT tri_base STREQUAL "NullComparator" AND NOT tri_base STREQUAL "RandomComparator" AND NOT tri_base STREQUAL "")
        if (alt_sort_key)
            set(sort_key "${SORT_KEY_true_${tri_base}}")
        else()
            set(sort_key "${SORT_KEY_false_${tri_base}}")
        endif()
        if (is_ascending)
            set(sort_order "ascending")
        else()
            set(sort_order "descending")
        endif()
        set(target_name "${refiner_snake}-${mesh_snake}-${sort_order}_${sort_key}_${tri_snake}-${merge_snake}-${refinement_criterion_snake}")
    else()
        if(refiner STREQUAL "DelaunayCavityRefiner")
            set(target_name "${refiner_snake}-${mesh_snake}-${tri_snake}-${merge_snake}-${refinement_criterion_snake}")
        else()
            set(target_name "${refiner_snake}-${mesh_snake}")
        endif()
    endif()
    add_executable(${target_name} ${main_file})
    target_link_libraries(${target_name} PUBLIC meshfiles hemesh mesh_io criteria refiners CLI11::CLI11)

    # Map mesh type to header
    set(mesh_header "${MESH_TYPE_HEADER_${mesh}}")

    target_compile_definitions(${target_name} PUBLIC
        REFINER_T=${refiner}
        MESH_TYPE=${mesh}
        MESH_TYPE_HEADER=${mesh_header}
        TRIANGLE_COMPARATOR_BASE_T=${tri_base}
        MERGING_STRATEGY_T=${merge}
        REFINEMENT_CRITERION_T=${refinement_criterion}
    )
    if(NOT tri_base STREQUAL "NullComparator" AND NOT tri_base STREQUAL "RandomComparator" AND NOT tri_base STREQUAL "")
        target_compile_definitions(${target_name} PUBLIC
            TRIANGLE_COMPARATOR_ASCENDING=${is_ascending}
            TRIANGLE_COMPARATOR_SORT_PARAM_2=${alt_sort_key}
            TRIANGLE_COMPARATOR_WITH_ARGS=1
        )
    endif()
    if(NOT refinement_criterion STREQUAL "NullRefinementCriterion" AND NOT refinement_criterion STREQUAL "")
        target_compile_definitions(${target_name} PUBLIC
            REFINEMENT_CRITERION_WITH_ARG=1
        )
    endif()
    if(refiner STREQUAL "DelaunayCavityRefiner")
        target_compile_definitions(${target_name} PUBLIC
            DELAUNAY_REFINER=1
        )
    endif()
endfunction()

# ---------------------------
# Optionally build all combinations
# ---------------------------
#[[
if(BUILD_MACRO_COMBO)
    foreach(refiner IN LISTS MESH_REFINER_OPTIONS)
        foreach(mesh IN LISTS MESH_TYPE_OPTIONS)
            if(refiner STREQUAL "PolyllaRefiner")
                add_configured_executable("main2.cpp" ${refiner} ${mesh} "" "" true true "")
            else()
                foreach(tri_base IN LISTS TRIANGLE_COMPARATOR_BASE_T_OPTIONS)
                    foreach(merge IN LISTS MERGING_STRATEGY_T_OPTIONS)
                        foreach(refine IN LISTS REFINEMENT_CRITERION_T_OPTIONS)
                            if(NOT tri_base STREQUAL "NullComparator" AND NOT tri_base STREQUAL "RandomComparator")
                                add_configured_executable("main2.cpp" ${refiner} ${mesh} ${tri_base} ${merge} true true ${refine})
                                add_configured_executable("main2.cpp" ${refiner} ${mesh} ${tri_base} ${merge} true false ${refine})
                                add_configured_executable("main2.cpp" ${refiner} ${mesh} ${tri_base} ${merge} false true ${refine})
                                add_configured_executable("main2.cpp" ${refiner} ${mesh} ${tri_base} ${merge} false false ${refine})
                            else()
                                add_configured_executable("main2.cpp" ${refiner} ${mesh} ${tri_base} ${merge} true true ${refine})
                            endif()
                        endforeach()
                    endforeach()
                endforeach()
            endif()
        endforeach()
    endforeach()
endif()

add_executable(Polylla main.cpp)
]]

add_configured_executable("main2.cpp" "DelaunayCavityRefiner" "HalfEdgeMesh" "EdgeLengthComparator" "MergeTrianglesIntoSmallestNeighbor" true false "NullRefinementCriterion")
#add_configured_executable("main2.cpp" "DelaunayCavityRefiner" "HalfEdgeMesh" "EdgeLengthComparator" "ExcludePreviousCavitiesStrategy" true false "NullRefinementCriterion")