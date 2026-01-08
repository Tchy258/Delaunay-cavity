#ifndef MIN_AREA_CRITERION_HPP
#define MIN_AREA_CRITERION_HPP
#include <concepts/mesh_data.hpp>

template <MeshData Mesh>
struct MinAreaCriterion {
    using FaceIndex = typename Mesh::FaceIndex;
    double minArea;

    bool operator()(const Mesh* mesh, FaceIndex polygonIndex) const;

    MinAreaCriterion(double minArea) : minArea(minArea) {}
};

#include <mesh_generators/selection_criteria/min_area_criterion.ipp>

#endif // MIN_AREA_CRITERION_HPP