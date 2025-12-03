#ifndef MIN_ANGLE_CRITERION_ROBUST_HPP
#include <mesh_refiners/refinement_criteria/min_angle_criterion_robust.hpp>
#endif

template <MeshData Mesh>
bool MinAngleCriterionRobust<Mesh>::operator()(const Mesh* mesh, FaceIndex polygonIndex) const {
    typename Mesh::EdgeIndex firstEdge = mesh->getPolygon(polygonIndex);
    typename Mesh::VertexType v1, v2, v3;

    mesh->getVerticesOfTriangle(polygonIndex,v1,v2,v3);
    Vertex edgeV1V2 = v2 - v1;
    Vertex edgeV2V3 = v3 - v2;
    Vertex edgeV3V1 = v1 - v3;
    
    // Equality of dot product: cos theta = (A . B) / (||A|| * ||B||)
    // So we take the arc cosine of that
    double angle1 = std::acos(std::clamp((edgeV3V1.dot(edgeV1V2)) / (std::sqrt(edgeV3V1.dot(edgeV3V1)) * std::sqrt(edgeV1V2.dot(edgeV1V2))), -1.0, 1.0));
    double angle2 = std::acos(std::clamp((-edgeV1V2.dot(edgeV2V3)) / (std::sqrt(edgeV1V2.dot(edgeV1V2)) * std::sqrt(edgeV2V3.dot(edgeV2V3))), -1.0, 1.0));
    double angle3 = std::acos(std::clamp((-edgeV2V3.dot(edgeV3V1)) / (std::sqrt(edgeV2V3.dot(edgeV2V3)) * std::sqrt(edgeV3V1.dot(edgeV3V1))), -1.0, 1.0));

    return std::min({angle1, angle2, angle3}) < angleThreshold;
}