#ifndef HALF_EDGE_HPP
#define HALF_EDGE_HPP
struct HalfEdge {
    int origin; //tail of edge
    //int target; //head of edge
    int twin; //opposite halfedge
    int next; //next halfedge of the same face
    int prev; //previous halfedge of the same face
    int face; //face index incident to the halfedge
    int isBorder; //1 if the halfedge is on the boundary, 0 otherwise
    bool operator==(const HalfEdge& other) const {
        return origin == other.origin && 
        next == other.next && 
        twin == other.twin && 
        prev == other.prev && 
        face == other.face && 
        isBorder == other.isBorder;
    }
};

#endif