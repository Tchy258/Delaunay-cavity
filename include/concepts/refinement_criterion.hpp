#ifndef REFINEMENT_CRITERION_HPP
#define REFINEMENT_CRITERION_HPP
#include <concepts>
#include <concepts/mesh_data.hpp>

template <typename Criterion, MeshData Mesh>
inline constexpr bool isNullRefinementCriterion = false;

template <typename Criterion, typename Mesh>
concept RefinementCriterionBase = requires(Criterion criterion, const Mesh* inputMesh, typename Mesh::FaceIndex polygonIndex) {
    {criterion(inputMesh,polygonIndex)} -> std::convertible_to<bool>;
} &&
MeshData<Mesh>;


/**
 * A refinement criterion must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have an operator() that receives an input mesh, a polygon index, and returns true or false
 * 
 * The NullRefinementCriterion defines a partial specialization of isNullRefinementCriterion to skip
 * refining altogether
 */
template <typename Criterion, typename Mesh>
concept RefinementCriterion = RefinementCriterionBase<Criterion,Mesh> || isNullRefinementCriterion<Criterion,Mesh>;

#endif