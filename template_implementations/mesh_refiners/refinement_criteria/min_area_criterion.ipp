#ifndef MIN_AREA_CRITERION_HPP
#include <mesh_refiners/refinement_criteria/min_area_criterion.hpp>
#endif

template <MeshData Mesh>
bool MinAreaCriterion<Mesh>::operator()(Mesh& mesh, typename Mesh::FaceIndex polygonIndex) const {
    typename Mesh::VertexType v1;
    typename Mesh::VertexType v2;
    typename Mesh::VertexType v3;

    mesh.getVerticesOfTriangle(polygonIndex, v1, v2, v3);

    double area = 0.5 * v1.cross2d(v2,v3);
    return area <= minArea;
}