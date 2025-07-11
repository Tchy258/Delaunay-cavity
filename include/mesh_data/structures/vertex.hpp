#ifndef VERTEX_HPP
#define VERTEX_HPP
#include<cmath>
#define EPSILON 1e-9
/**
 * Basic vertex structure with x,y coordinates
 */
struct Vertex{
    double x;
    double y;
    /**
     * Dot product of `this` and vertex `v`
     */
    inline double dot(Vertex v) const {
        return x * v.x + y * v.y;
    }
    /**
     * Z coordinate of cross product between (`v1` - `this`) and (`v2` - `this`)
     */
    inline double cross2d(Vertex v1, Vertex v2) const {
        Vertex v1v0 = v1 - *this;
        Vertex v2v0 = v2 - *this;
        return (v1v0.x * v2v0.y) - (v1v0.y * v2v0.x);
    }
     /**
     * Returns true if point `P` lies inside the circumcircle of triangle ABC.
     * Assumes triangle ABC is oriented counter-clockwise (CCW).
     */
    static bool inCircle(const Vertex& A, const Vertex& B, const Vertex& C, const Vertex& P) {
        double ax = A.x - P.x;
        double ay = A.y - P.y;
        double bx = B.x - P.x;
        double by = B.y - P.y;
        double cx = C.x - P.x;
        double cy = C.y - P.y;

        double aLenSq = ax * ax + ay * ay;
        double bLenSq = bx * bx + by * by;
        double cLenSq = cx * cx + cy * cy;

        double det = ax * (by * cLenSq - bLenSq * cy)
                - ay * (bx * cLenSq - bLenSq * cx)
                + aLenSq * (bx * cy - by * cx);

        return det > 0;
    }

    Vertex operator+(const Vertex& other) const {
        return Vertex{x + other.x, y + other.y};
    }
    Vertex operator-(const Vertex& other) const {
        return Vertex{x - other.x, y - other.y};
    }
    Vertex operator*(double scalar) const {
        return Vertex{x*scalar, y*scalar};
    }
    bool operator==(const Vertex& other) const {
        return std::fabs(x - other.x) < EPSILON && std::fabs(y - other.y) < EPSILON;
    }
};

#undef EPSILON
#endif