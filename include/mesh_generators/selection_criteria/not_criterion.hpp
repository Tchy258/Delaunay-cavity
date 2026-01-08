#ifndef NOT_CRITERION_HPP
#define NOT_CRITERION_HPP
#include <concepts/mesh_data.hpp>
#include <concepts/selection_criterion.hpp>

/**
 * Selection criteria negator, will return the opposite of the criteria's condition
 */
template<MeshData Mesh, SelectionCriterion<Mesh> Criterion>
struct NotCriterion {
    using FaceIndex = typename Mesh::FaceIndex;
    Criterion negatedCriteria;
    NotCriterion(Criterion criterion) : negatedCriteria(criterion) {}
    bool operator()(const Mesh* mesh, FaceIndex polygonIndex) {
        return !negatedCriteria(mesh,polygonIndex);
    }
};

#endif