#ifndef ANGLE_COMPARATOR_HPP
#define ANGLE_COMPARATOR_HPP
#include <cmath>
#include <cmath>
#include <algorithm>
#include <concepts/mesh_data.hpp>

template <MeshData Mesh, bool ByAscendingOrder, bool ByMinAngle = true>
struct AngleComparator {
    using MeshVertex = typename Mesh::VertexType;
    using FaceIndex = typename Mesh::FaceIndex;
    static bool compare(const Mesh* meshptr, const FaceIndex& t1, const FaceIndex& t2) {
        
        auto getAngle = [&](const FaceIndex polygonIndex) {
            MeshVertex v1;
            MeshVertex v2;
            MeshVertex v3;
            meshptr->getVerticesOfTriangle(polygonIndex,v1,v2,v3);
            Vertex edgeV1V2 = v2 - v1;
            Vertex edgeV2V3 = v3 - v2;
            Vertex edgeV3V1 = v1 - v3;
            
            // Equality of dot product: cos theta = (A . B) / (||A|| * ||B||)
            // So we take the arc cosine of that
            double angle1 = std::acos(std::clamp((edgeV3V1.dot(edgeV1V2)) / (std::sqrt(edgeV3V1.dot(edgeV3V1)) * std::sqrt(edgeV1V2.dot(edgeV1V2))), -1.0, 1.0));
            double angle2 = std::acos(std::clamp((-edgeV1V2.dot(edgeV2V3)) / (std::sqrt(edgeV1V2.dot(edgeV1V2)) * std::sqrt(edgeV2V3.dot(edgeV2V3))), -1.0, 1.0));
            double angle3 = std::acos(std::clamp((-edgeV2V3.dot(edgeV3V1)) / (std::sqrt(edgeV2V3.dot(edgeV2V3)) * std::sqrt(edgeV3V1.dot(edgeV3V1))), -1.0, 1.0));

            if constexpr (ByMinAngle) {
                return std::min({angle1, angle2, angle3});
            } else {
                return std::max({angle1, angle2, angle3});
            }
        };
        if constexpr (ByAscendingOrder) {
            return getAngle(t1) < getAngle(t2);
        } else {
            return getAngle(t1) > getAngle(t2);
        }
    }
};

template <MeshData Mesh>
using DescendingMinAngleComparator = AngleComparator<Mesh,false,true>;

template <MeshData Mesh>
using AscendingMinAngleComparator = AngleComparator<Mesh,true,true>;

template <MeshData Mesh>
using DescendingMaxAngleComparator = AngleComparator<Mesh,false,false>;

template <MeshData Mesh>
using AscendingMaxAngleComparator = AngleComparator<Mesh,true,false>;

#endif // ANGLE_COMPARATOR_HPP