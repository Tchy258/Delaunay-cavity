#ifndef MIN_AREA2_CRITERION_HPP
#define MIN_AREA2_CRITERION_HPP
#include <concepts/mesh_data.hpp>

template <MeshData Mesh>
struct MinArea2Criterion {
    double minArea2;
    
    MinArea2Criterion(double minArea2) : minArea2(minArea2) {}

    bool operator()(const Mesh* mesh, typename Mesh::FaceIndex polygonIndex) const;
};

#include <mesh_refiners/refinement_criteria/min_area2_criterion.ipp>

#endif // MIN_AREA2_CRITERION_HPP