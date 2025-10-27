#ifndef AREA_COMPARATOR_HPP
#define AREA_COMPARATOR_HPP
#include <concepts/refinement_criterion.hpp>
#include <concepts/mesh_data.hpp>
#include <cmath>

template <MeshData Mesh, bool ByAscendingOrder, bool Precise = false>
struct AreaComparator {
    using MeshVertex = typename Mesh::VertexType;
    using FaceIndex = typename Mesh::FaceIndex;
    static bool compare(const Mesh* meshptr, const FaceIndex& t1, const FaceIndex& t2) {
        MeshVertex v0t1, v1t1, v2t1;
        MeshVertex v0t2, v1t2, v2t2;

        meshptr->getVerticesOfTriangle(t1, v0t1, v1t1, v2t1);
        meshptr->getVerticesOfTriangle(t2, v0t2, v1t2, v2t2);

        double area1 = v0t1.cross2d(v1t1,v2t1);
        double area2 = v0t2.cross2d(v1t2,v2t2);
        if constexpr (Precise) {
            area1 *= 0.5;
            area2 *= 0.5;
        }
        if constexpr (ByAscendingOrder) {
            return std::fabs(area1) < std::fabs(area2);
        } else {
            return std::fabs(area1) > std::fabs(area2);
        }
    }
};

template <MeshData Mesh>
using AscendingAreaComparator = AreaComparator<Mesh,true,false>;

template <MeshData Mesh>
using AscendingAreaComparatorPrecise = AreaComparator<Mesh,true,true>;

template <MeshData Mesh>
using DescendingAreaComparator = AreaComparator<Mesh,false,false>;

template <MeshData Mesh>
using DescendingAreaComparatorPrecise = AreaComparator<Mesh,false,true>;

#endif // AREA_COMPARATOR_HPP