#ifndef HE_VERTEX_HPP
#define HE_VERTEX_HPP
#include <mesh_data/structures/vertex.hpp>

struct HEVertex : public Vertex {
    int incidentHalfEdge;
    bool isBorder;
};

#endif