#ifndef MIN_ANGLE_CRITERION_ROBUST_HPP
#define MIN_ANGLE_CRITERION_ROBUST_HPP
#include <cmath>
#include <algorithm>
#include <concepts/mesh_data.hpp>
#include <mesh_data/structures/vertex.hpp>
#include <concepts/is_half_edge_vertex.hpp>
#define PI 3.14159265358979323846

/**
 * Minimum angle criterion comparator, this criterion will compare
 * compute the arccosine of the cosine of each angle, very expensive to use
 */
template <MeshData Mesh>
struct MinAngleCriterionRobust {
    double angleThreshold;
    /**
     * Checks if the minimum angle of the triangle given by `polygonIndex` in `mesh` 
     * is less than or equal to `angleThreshold`, and if so, must be the start of a cavity
     * @param mesh The input mesh
     * @param polygonIndex The index of the triangle to be checked
     */
    bool operator()(const Mesh* mesh, int polygonIndex) const;
    /**
     * @param angle Angle threshold the user wants to check for
     * @param type Whether the angle is given in degrees (default to true) or radians
     */
    MinAngleCriterionRobust(double angle, bool type = true) : angleThreshold(angle) {
        if (type) {
            angleThreshold = angle * PI / 180.0;
        } else {
            angleThreshold = angle;
        }
    }
};

#include <template_implementations/mesh_refiners/refinement_criteria/min_angle_criterion_robust.ipp>
#undef PI
#endif