#ifndef MIN_ANGLE_CRITERION_HPP
#define MIN_ANGLE_CRITERION_HPP
#include <cmath>
#include <concepts/mesh_data.hpp>
#include <concepts/is_half_edge_vertex.hpp>

struct MinAngleCriterion {
    double degrees;

    template <MeshData Mesh>
    bool operator()(const Mesh& mesh, int polygonIndex) const;
    MinAngleCriterion(double deg) : degrees(deg) {}
};
#endif