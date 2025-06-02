#ifndef HALF_EDGE_HPP
#define HALF_EDGE_HPP
#include <vector>

namespace HalfEdge {

    struct HalfEdge {
        int origin; //tail of edge
        //int target; //head of edge
        int twin; //opposite halfedge
        int next; //next halfedge of the same face
        int prev; //previous halfedge of the same face
    // int face = -1; //face index incident to the halfedge
        int isBorder; //1 if the halfedge is on the boundary, 0 otherwise
    };

    /**
     * Calculates the next edge of the face incident to edge `e`
     * 
     * This edge's origin is `e`'s target
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge we want to get the next edge from
     * @return Index to the next edge of the face incident to e
     */
    int next(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Calculates the tail vertex of the edge `e`
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge whose origin we want to get
     * @return Index to the tail vertex v of the edge e
    */
    int origin(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Calculates the head vertex of the edge `e`
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge whose target we want to get
     * @return Index to the head vertex v of the edge `e` 
     */
    int target(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Returns the twin edge of the edge `e`
     * @param halfEdges Vector of half edges
     * @param e The edge whose twin we want to get
     * @return Index to twin edge of `e`
     */
    int twin(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Calculates the previous edge of the face incident to edge `e`
     * 
     * This edge's target is `e`'s origin
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge whose previous edge we want to get
     * @return Index to the previous edge of the face incident to `e`
     */
    int prev(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Returns the next counterclockwise edge of the origin vertex `v` of edge `e`
     * 
     * This is the `twin` of the `prev` edge of `e`
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge with an origin `v` whose next ccw edge we need
     * @return Index to next counterclockwise edge
     */
    int CCWEdgeToVertex(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * Returns the next clockwise edge of the origin vertex `v` of edge `e`
     * 
     * This is the `next` of the `twin` edge of `e`
     * 
     * @param halfEdges Vector of half edges
     * @param e The edge with an origin `v` whose next cw edge we need
     * @return Index to next clockwise edge
     */
    int CWEdgeToVertex(std::vector<HalfEdge> &halfEdges, int e);

    /**
     * @param halgEdges Vector of half edges
     * @param e Edge we want to query from
     * @return Whether the face of `e` is a border face
     */
    bool isBorderFace(std::vector<HalfEdge> &halfEdges, int e);

        // Input: edge e of compressTriangulation
        // Output: true if the edge is an interior face a
        //         false otherwise
    /**
     * @param halgEdges Vector of half edges
     * @param e Edge we want to query from
     * @return Whether the face of `e` is an interior face
     */
    bool isInteriorFace(std::vector<HalfEdge> &halfEdges, int e);
    
    inline int incidentHalfedge(int f) {
        return 3*f;
    }
}
#endif