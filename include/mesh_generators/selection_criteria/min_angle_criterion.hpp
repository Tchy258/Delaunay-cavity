#ifndef MIN_ANGLE_CRITERION_HPP
#define MIN_ANGLE_CRITERION_HPP
#include <cmath>
#include <concepts/mesh_data.hpp>
#include <mesh_data/structures/vertex.hpp>
#include <iostream>
#define PI 3.14159265358979323846

/**
 * Efficient minimum angle criterion comparator, this criterion will compare
 * the squared cosine of the angle provided to the squared cosine of the
 * angle opposite to the smallest side of the triangle, this assumes the triangle
 * is acute, only use if the input mesh is known to be of good quality
 */
template <MeshData Mesh>
struct MinAngleCriterion {
    using FaceIndex = typename Mesh::FaceIndex;
    double angleThreshold;
    /**
     * Checks if the minimum angle of the triangle given by `polygonIndex` in `mesh` 
     * is less than or equal to `angleThreshold`, and if so, must be the start of a cavity
     * @param mesh The input mesh
     * @param polygonIndex The index of the triangle to be checked
     */
    bool operator()(const Mesh* mesh, FaceIndex polygonIndex) const;
    /**
     * @param angle Angle threshold the user wants to check for
     * @param type Whether the angle is given in degrees (default to true) or radians
     */
    MinAngleCriterion(double angle, bool type = true) {
    double radians = type ? angle * PI / 180.0 : angle;

    if (radians >= PI) {
        std::cout << "Warning: Angle >= 180° is invalid, clamping to 179.999°" << std::endl;
        radians = PI - 1e-6; // clamp
    }

    double c = std::cos(radians);
    angleThreshold = (c >= 0.0 ? 1.0 : -1.0) * (c * c);
    }
};

#include<mesh_generators/selection_criteria/min_angle_criterion.ipp>
#undef PI
#endif