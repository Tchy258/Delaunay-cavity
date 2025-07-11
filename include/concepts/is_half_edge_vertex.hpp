#ifndef IS_HALF_EDGE_VERTEX_HPP
#define IS_HALF_EDGE_VERTEX_HPP

template<typename VertexType>
concept IsHalfEdgeVertex = requires(VertexType v) {
    {v.incidentHalfEdge};
    {v.isBorder};
};

#endif