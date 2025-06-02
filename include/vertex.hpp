#ifndef VERTEX_HPP
#define VERTEX_HPP
#include <vector>

struct HalfEdge;

namespace Vertex {
    /**
     * Vertex structure for use with half edges
     */
    struct Vertex{
        double x;
        double y;
        bool isBorder = false; // if the vertex is on the boundary
        int incidentHalfedge; // halfedge incident to the vertex, vertex is the origin of the halfedge
    };

    /**
     * Returns the index of the edge associated with the vertex `v`
     * @param vertices Vector of vertices
     * @param v Vertex whose edge we want to get
     * @return An integer index to some vector of HalfEdges with the edge of v
     */
    int edgeOfVertex(std::vector<Vertex> &vertices, int v);

    /**
     * @param vertices Vector of vertices
     * @param v Vertex we want to query
     * @return Whether this vertex `v` is part of the border of the polygon
     */
    bool isBorderVertex(std::vector<Vertex> &vertices, int v);

    /**
     * Returns the degree of vertex `v` for the polygon defined by `vertices` and `halfEdges`
     * 
     * The degree of a vertex `v` is equivalent to how many half edges have `v` as its origin
     * 
     * @param vertices Vector of vertices
     * @param halfEdges Vector of halfEdges
     * @param v Vertex whose degree we want to calculate
     * @returns Amount of half edges that have `v` as its origin
     */
    int degree(std::vector<Vertex> &vertices, std::vector<HalfEdge> &halfEdges, int v);
}

#endif