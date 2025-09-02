#ifndef REFINEMENT_CRITERION_HPP
#define REFINEMENT_CRITERION_HPP
#include<concepts>
#include<concepts/mesh_data.hpp>

/**
 * A refinement criterion must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have an operator() that receives an input mesh, a polygon index, and returns true or false
 */
template <typename Criterion, typename Mesh>
concept RefinementCriterion = requires(Criterion criterion, Mesh inputMesh, int polygonIndex) {
    {criterion(inputMesh,polygonIndex)} -> std::convertible_to<bool>;
} &&
MeshData<Mesh>;


#endif