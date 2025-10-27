#ifndef MIN_AREA2_CRITERION_HPP
#include <mesh_refiners/refinement_criteria/min_area2_criterion.hpp>
#endif

template <MeshData Mesh>
bool MinArea2Criterion<Mesh>::operator()(const Mesh* mesh, typename Mesh::FaceIndex polygonIndex) const {
    typename Mesh::VertexType v1,v2,v3;

    mesh->getVerticesOfTriangle(polygonIndex, v1, v2, v3);

    double area = v1.cross2d(v2,v3);
    return area >= minArea2;
}