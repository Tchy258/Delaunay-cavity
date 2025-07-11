#ifndef OR_CRITERION_HPP
#define OR_CRITERION_HPP
#include <vector>
#include <concepts/mesh_data.hpp>

template <MeshData Mesh, typename Criterion1, typename Criterion2>
struct OrCriteria {
    Criterion1 criterion1;
    Criterion2 criterion2;

    bool operator()(const Mesh& mesh, int polygonIndex) const {
        return criterion1(mesh, polygonIndex) || criterion2(mesh, polygonIndex);
    }
};

#endif