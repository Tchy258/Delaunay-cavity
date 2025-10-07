#ifndef MIN_AREA_CRITERION_HPP
#define MIN_AREA_CRITERION_HPP
#include <concepts/mesh_data.hpp>

template <MeshData Mesh>
struct MinAreaCriterion {
    double minArea;

    bool operator()(const Mesh* mesh, typename Mesh::FaceIndex polygonIndex) const;

    MinAreaCriterion(double minArea) : minArea(minArea) {}
};

#include <mesh_refiners/refinement_criteria/min_area_criterion.ipp>

#endif // MIN_AREA_CRITERION_HPP