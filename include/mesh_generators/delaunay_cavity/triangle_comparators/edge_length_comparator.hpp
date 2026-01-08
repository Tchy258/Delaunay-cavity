#ifndef EDGE_LENGTH_COMPARATOR_HPP
#define EDGE_LENGTH_COMPARATOR_HPP
#include <concepts/mesh_data.hpp>
#include <array>
#include <algorithm>

template <MeshData Mesh, bool ByAscendingOrder, bool ByMinEdge = true>
struct EdgeLengthComparator {
    using FaceIndex = typename Mesh::FaceIndex;
    using EdgeIndex = typename Mesh::EdgeIndex;
    static bool compare(const Mesh* meshptr, const FaceIndex& t1, const FaceIndex& t2) {

        auto selectEdge = [meshptr](const std::array<EdgeIndex, 3>& edges) {
            return std::transform_reduce(
                edges.begin(), edges.end(),
                ByMinEdge ? std::numeric_limits<double>::max()
                          : std::numeric_limits<double>::lowest(),
                ByMinEdge ? [](double a, double b){ return std::min(a, b); }
                          : [](double a, double b){ return std::max(a, b); },
                [meshptr](EdgeIndex e){ return meshptr->edgeLength2(e); }
            );
        };

        double selectedT1Edge = selectEdge(meshptr->getEdgesOfTriangle(t1));
        double selectedT2Edge = selectEdge(meshptr->getEdgesOfTriangle(t2));

        if constexpr (ByAscendingOrder) {
            return selectedT1Edge < selectedT2Edge;
        } else {
            return selectedT1Edge > selectedT2Edge;
        }
    }
};

template <MeshData Mesh>
using AscendingMinEdgeLengthComparator = EdgeLengthComparator<Mesh, true, true>;

template <MeshData Mesh>
using DescendingMinEdgeLengthComparator = EdgeLengthComparator<Mesh, false, true>;

template <MeshData Mesh>
using AscendingMaxEdgeLengthComparator = EdgeLengthComparator<Mesh, true, false>;

template <MeshData Mesh>
using DescendingMaxEdgeLengthComparator = EdgeLengthComparator<Mesh, false, false>;

#endif // EDGE_LENGTH_COMPARATOR_HPP