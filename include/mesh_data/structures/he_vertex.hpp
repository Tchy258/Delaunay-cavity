#ifndef HE_VERTEX_HPP
#define HE_VERTEX_HPP
#include <mesh_data/structures/vertex.hpp>
#include <concepts/is_half_edge_vertex.hpp>
struct HEVertex : public Vertex {
    int incidentHalfEdge;
    bool isBorder;

};
static_assert(IsHalfEdgeVertex<HEVertex>);

#endif