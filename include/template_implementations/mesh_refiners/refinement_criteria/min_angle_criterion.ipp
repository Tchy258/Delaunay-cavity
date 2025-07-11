#ifndef MIN_ANGLE_CRITERION_HPP
#include<mesh_refiners/refinement_criteria/min_angle_criterion.hpp>
#endif

template <MeshData Mesh>
bool MinAngleCriterion::operator()(const Mesh& mesh, int polygonIndex) const {
    Mesh::EdgeIndex firstEdge = mesh.getPolygon(polygonIndex);
    if constexpr (IsHalfEdgeVertex(typename Mesh::VertexType)) {
        std::array<Mesh::VertexIndex,3> verticesToCheck{-1,-1,-1};
        Mesh::VertexIndex v1 = mesh.origin(firstEdge);
        Mesh::VertexIndex v2 = mesh.target(firstEdge);
        Mesh::VertexIndex v3 = mesh.target(mesh.next(firstEdge));
        // TODO: Calcular el ángulo
    }
}