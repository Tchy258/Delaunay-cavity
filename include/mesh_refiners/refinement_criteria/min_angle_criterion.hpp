#ifndef MIN_ANGLE_CRITERION_HPP
#define MIN_ANGLE_CRITERION_HPP
#include <cmath>
#include <concepts/mesh_data.hpp>
#include <mesh_data/structures/vertex.hpp>
#include <concepts/is_half_edge_vertex.hpp>
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
    double angleThreshold;
    /**
     * Checks if the minimum angle of the triangle given by `polygonIndex` in `mesh` 
     * is less than or equal to `angleThreshold`, and if so, must be the start of a cavity
     * @param mesh The input mesh
     * @param polygonIndex The index of the triangle to be checked
     */
    bool operator()(const Mesh& mesh, int polygonIndex) const;
    /**
     * @param angle Angle threshold the user wants to check for
     * @param type Whether the angle is given in degrees (default to true) or radians
     */
    MinAngleCriterion(double angle, bool type = true) : angleThreshold(angle) {
        double finalAngle = angle;
        if (type) {
            if (angle >= 90.0) {
                std::cout << "Warning: Non-acute angle provided for min angle criterion, the criterion will look for the supplement instead" << std::endl;
                finalAngle = 180.0 - angle;
            }
            angleThreshold = std::cos(finalAngle * PI / 180.0);
        } else {
            if (angle >= (PI / 2)) {
                std::cout << "Warning: Non-acute angle provided for min angle criterion, the criterion will look for the supplement instead" << std::endl;
                finalAngle = PI - angle;
            }
            angleThreshold = std::cos(finalAngle);
        }
        angleThreshold *= angleThreshold;
    }
};

#include<template_implementations/mesh_refiners/refinement_criteria/min_angle_criterion.ipp>
#undef PI
#endif