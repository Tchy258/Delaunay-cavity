#ifndef OR_CRITERIA_HPP
#define OR_CRITERIA_HPP
#include <vector>
#include <concepts/mesh_data.hpp>
#include <concepts/selection_criterion.hpp>
template <MeshData Mesh, SelectionCriterion<Mesh> Criterion1, SelectionCriterion<Mesh> Criterion2>
struct OrCriteria {
    Criterion1 criterion1;
    Criterion2 criterion2;

    bool operator()(Mesh& mesh, int polygonIndex) const {
        return criterion1(mesh, polygonIndex) || criterion2(mesh, polygonIndex);
    }

    OrCriteria(Criterion1 c1, Criterion2 c2) : criterion1(c1), criterion2(c2) {}
};

#endif