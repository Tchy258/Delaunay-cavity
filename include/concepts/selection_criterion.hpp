#ifndef SELECTION_CRITERION_HPP
#define SELECTION_CRITERION_HPP
#include <concepts>
#include <concepts/mesh_data.hpp>

template <typename Criterion, MeshData Mesh>
inline constexpr bool isNullSelectionCriterion = false;

template <typename Criterion, typename Mesh>
concept SelectionCriterionBase = requires(Criterion criterion, const Mesh* inputMesh, typename Mesh::FaceIndex polygonIndex) {
    {criterion(inputMesh,polygonIndex)} -> std::convertible_to<bool>;
} &&
MeshData<Mesh>;


/**
 * A selection criterion must apply to a particular mesh that conforms to the MeshData concept,
 * and it must have an operator() that receives an input mesh, a polygon index, and returns true or false
 * 
 * The NullSelectionCriterion defines a partial specialization of isNullSelectionCriterion to skip
 * refining altogether
 */
template <typename Criterion, typename Mesh>
concept SelectionCriterion = SelectionCriterionBase<Criterion,Mesh> || isNullSelectionCriterion<Criterion,Mesh>;

#endif