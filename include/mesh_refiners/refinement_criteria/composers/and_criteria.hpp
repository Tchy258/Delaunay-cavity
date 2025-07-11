#ifndef AND_CRITERIA_HPP
#define AND_CRITERIA_HPP
#include <vector>
#include <concepts/mesh_data.hpp>

template <MeshData Mesh, typename Criterion1, typename Criterion2>
struct AndCriteria {
    Criterion1 criterion1;
    Criterion2 criterion2;

    bool operator()(const Mesh& mesh, int polygonIndex) const {
        return criterion1(mesh, polygonIndex) && criterion2(mesh, polygonIndex);
    }
};

#endif