#ifndef NOT_CRITERION_HPP
#define NOT_CRITERION_HPP
#include <concepts/mesh_data.hpp>
#include <concepts/refinement_criterion.hpp>

/**
 * Refinement criteria negator, will return the opposite of the criteria's condition
 */
template<MeshData Mesh, RefinementCriterion<Mesh> Criterion>
struct NotCriterion {
    using FaceIndex = typename Mesh::FaceIndex;
    Criterion negatedCriteria;
    NotCriterion(Criterion criterion) : negatedCriteria(criterion) {}
    bool operator()(const Mesh* mesh, FaceIndex polygonIndex) {
        return !negatedCriteria(mesh,polygonIndex);
    }
};

#endif