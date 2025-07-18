/**
 * This code contains the inCircle test adapted from Triangle
 */
/*****************************************************************************/
/*                                                                           */
/*      888888888        ,o,                          / 888                  */
/*         888    88o88o  "    o8888o  88o8888o o88888o 888  o88888o         */
/*         888    888    888       88b 888  888 888 888 888 d888  88b        */
/*         888    888    888  o88^o888 888  888 "88888" 888 8888oo888        */
/*         888    888    888 C888  888 888  888  /      888 q888             */
/*         888    888    888  "88o^888 888  888 Cb      888  "88oooo"        */
/*                                              "8oo8D                       */
/*                                                                           */
/*  A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.      */
/*  (triangle.c)                                                             */
/*                                                                           */
/*  Version 1.6                                                              */
/*  July 28, 2005                                                            */
/*                                                                           */
/*  Copyright 1993, 1995, 1997, 1998, 2002, 2005                             */
/*  Jonathan Richard Shewchuk                                                */
/*  2360 Woolsey #H                                                          */
/*  Berkeley, California  94705-1927                                         */
/*  jrs@cs.berkeley.edu                                                      */
/*                                                                           */
/*  This program may be freely redistributed under the condition that the    */
/*    copyright notices (including this entire header and the copyright      */
/*    notice printed when the `-h' switch is selected) are not removed, and  */
/*    no compensation is received.  Private, research, and institutional     */
/*    use is free.  You may distribute modified versions of this code UNDER  */
/*    THE CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE TO IT IN THE   */
/*    SAME FILE REMAIN UNDER COPYRIGHT OF THE ORIGINAL AUTHOR, BOTH SOURCE   */
/*    AND OBJECT CODE ARE MADE FREELY AVAILABLE WITHOUT CHARGE, AND CLEAR    */
/*    NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution of this code as    */
/*    part of a commercial system is permissible ONLY BY DIRECT ARRANGEMENT  */
/*    WITH THE AUTHOR.  (If you are not directly supplying this code to a    */
/*    customer, and you are instead telling them how they can obtain it for  */
/*    free, then you are not required to make any arrangement with me.)      */
/*                                                                           */
/*  Hypertext instructions for Triangle are available on the Web at          */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/triangle.html                           */
/*                                                                           */
/*  Disclaimer:  Neither I nor Carnegie Mellon warrant this code in any way  */
/*    whatsoever.  This code is provided "as-is".  Use at your own risk.     */
/*                                                                           */
/*  Some of the references listed below are marked with an asterisk.  [*]    */
/*    These references are available for downloading from the Web page       */
/*                                                                           */
/*      http://www.cs.cmu.edu/~quake/triangle.research.html                  */
/*                                                                           */
/*  Three papers discussing aspects of Triangle are available.  A short      */
/*    overview appears in "Triangle:  Engineering a 2D Quality Mesh          */
/*    Generator and Delaunay Triangulator," in Applied Computational         */
/*    Geometry:  Towards Geometric Engineering, Ming C. Lin and Dinesh       */
/*    Manocha, editors, Lecture Notes in Computer Science volume 1148,       */
/*    pages 203-222, Springer-Verlag, Berlin, May 1996 (from the First ACM   */
/*    Workshop on Applied Computational Geometry).  [*]                      */
/*                                                                           */
/*    The algorithms are discussed in the greatest detail in "Delaunay       */
/*    Refinement Algorithms for Triangular Mesh Generation," Computational   */
/*    Geometry:  Theory and Applications 22(1-3):21-74, May 2002.  [*]       */
/*                                                                           */
/*    More detail about the data structures may be found in my dissertation: */
/*    "Delaunay Refinement Mesh Generation," Ph.D. thesis, Technical Report  */
/*    CMU-CS-97-137, School of Computer Science, Carnegie Mellon University, */
/*    Pittsburgh, Pennsylvania, 18 May 1997.  [*]                            */
/*                                                                           */
/*  Triangle was created as part of the Quake Project in the School of       */
/*    Computer Science at Carnegie Mellon University.  For further           */
/*    information, see Hesheng Bao, Jacobo Bielak, Omar Ghattas, Loukas F.   */
/*    Kallivokas, David R. O'Hallaron, Jonathan R. Shewchuk, and Jifeng Xu,  */
/*    "Large-scale Simulation of Elastic Wave Propagation in Heterogeneous   */
/*    Media on Parallel Computers," Computer Methods in Applied Mechanics    */
/*    and Engineering 152(1-2):85-102, 22 January 1998.                      */
/*                                                                           */
/*  Triangle's Delaunay refinement algorithm for quality mesh generation is  */
/*    a hybrid of one due to Jim Ruppert, "A Delaunay Refinement Algorithm   */
/*    for Quality 2-Dimensional Mesh Generation," Journal of Algorithms      */
/*    18(3):548-585, May 1995 [*], and one due to L. Paul Chew, "Guaranteed- */
/*    Quality Mesh Generation for Curved Surfaces," Proceedings of the Ninth */
/*    Annual Symposium on Computational Geometry (San Diego, California),    */
/*    pages 274-280, Association for Computing Machinery, May 1993,          */
/*    http://portal.acm.org/citation.cfm?id=161150 .                         */
/*                                                                           */
/*  The Delaunay refinement algorithm has been modified so that it meshes    */
/*    domains with small input angles well, as described in Gary L. Miller,  */
/*    Steven E. Pav, and Noel J. Walkington, "When and Why Ruppert's         */
/*    Algorithm Works," Twelfth International Meshing Roundtable, pages      */
/*    91-102, Sandia National Laboratories, September 2003.  [*]             */
/*                                                                           */
/*  My implementation of the divide-and-conquer and incremental Delaunay     */
/*    triangulation algorithms follows closely the presentation of Guibas    */
/*    and Stolfi, even though I use a triangle-based data structure instead  */
/*    of their quad-edge data structure.  (In fact, I originally implemented */
/*    Triangle using the quad-edge data structure, but the switch to a       */
/*    triangle-based data structure sped Triangle by a factor of two.)  The  */
/*    mesh manipulation primitives and the two aforementioned Delaunay       */
/*    triangulation algorithms are described by Leonidas J. Guibas and Jorge */
/*    Stolfi, "Primitives for the Manipulation of General Subdivisions and   */
/*    the Computation of Voronoi Diagrams," ACM Transactions on Graphics     */
/*    4(2):74-123, April 1985, http://portal.acm.org/citation.cfm?id=282923 .*/
/*                                                                           */
/*  Their O(n log n) divide-and-conquer algorithm is adapted from Der-Tsai   */
/*    Lee and Bruce J. Schachter, "Two Algorithms for Constructing the       */
/*    Delaunay Triangulation," International Journal of Computer and         */
/*    Information Science 9(3):219-242, 1980.  Triangle's improvement of the */
/*    divide-and-conquer algorithm by alternating between vertical and       */
/*    horizontal cuts was introduced by Rex A. Dwyer, "A Faster Divide-and-  */
/*    Conquer Algorithm for Constructing Delaunay Triangulations,"           */
/*    Algorithmica 2(2):137-151, 1987.                                       */
/*                                                                           */
/*  The incremental insertion algorithm was first proposed by C. L. Lawson,  */
/*    "Software for C1 Surface Interpolation," in Mathematical Software III, */
/*    John R. Rice, editor, Academic Press, New York, pp. 161-194, 1977.     */
/*    For point location, I use the algorithm of Ernst P. Mucke, Isaac       */
/*    Saias, and Binhai Zhu, "Fast Randomized Point Location Without         */
/*    Preprocessing in Two- and Three-Dimensional Delaunay Triangulations,"  */
/*    Proceedings of the Twelfth Annual Symposium on Computational Geometry, */
/*    ACM, May 1996.  [*]  If I were to randomize the order of vertex        */
/*    insertion (I currently don't bother), their result combined with the   */
/*    result of Kenneth L. Clarkson and Peter W. Shor, "Applications of      */
/*    Random Sampling in Computational Geometry II," Discrete &              */
/*    Computational Geometry 4(1):387-421, 1989, would yield an expected     */
/*    O(n^{4/3}) bound on running time.                                      */
/*                                                                           */
/*  The O(n log n) sweepline Delaunay triangulation algorithm is taken from  */
/*    Steven Fortune, "A Sweepline Algorithm for Voronoi Diagrams",          */
/*    Algorithmica 2(2):153-174, 1987.  A random sample of edges on the      */
/*    boundary of the triangulation are maintained in a splay tree for the   */
/*    purpose of point location.  Splay trees are described by Daniel        */
/*    Dominic Sleator and Robert Endre Tarjan, "Self-Adjusting Binary Search */
/*    Trees," Journal of the ACM 32(3):652-686, July 1985,                   */
/*    http://portal.acm.org/citation.cfm?id=3835 .                           */
/*                                                                           */
/*  The algorithms for exact computation of the signs of determinants are    */
/*    described in Jonathan Richard Shewchuk, "Adaptive Precision Floating-  */
/*    Point Arithmetic and Fast Robust Geometric Predicates," Discrete &     */
/*    Computational Geometry 18(3):305-363, October 1997.  (Also available   */
/*    as Technical Report CMU-CS-96-140, School of Computer Science,         */
/*    Carnegie Mellon University, Pittsburgh, Pennsylvania, May 1996.)  [*]  */
/*    An abbreviated version appears as Jonathan Richard Shewchuk, "Robust   */
/*    Adaptive Floating-Point Geometric Predicates," Proceedings of the      */
/*    Twelfth Annual Symposium on Computational Geometry, ACM, May 1996. [*] */
/*    Many of the ideas for my exact arithmetic routines originate with      */
/*    Douglas M. Priest, "Algorithms for Arbitrary Precision Floating Point  */
/*    Arithmetic," Tenth Symposium on Computer Arithmetic, pp. 132-143, IEEE */
/*    Computer Society Press, 1991.  [*]  Many of the ideas for the correct  */
/*    evaluation of the signs of determinants are taken from Steven Fortune  */
/*    and Christopher J. Van Wyk, "Efficient Exact Arithmetic for Computa-   */
/*    tional Geometry," Proceedings of the Ninth Annual Symposium on         */
/*    Computational Geometry, ACM, pp. 163-172, May 1993, and from Steven    */
/*    Fortune, "Numerical Stability of Algorithms for 2D Delaunay Triangu-   */
/*    lations," International Journal of Computational Geometry & Applica-   */
/*    tions 5(1-2):193-213, March-June 1995.                                 */
/*                                                                           */
/*  The method of inserting new vertices off-center (not precisely at the    */
/*    circumcenter of every poor-quality triangle) is from Alper Ungor,      */
/*    "Off-centers:  A New Type of Steiner Points for Computing Size-Optimal */
/*    Quality-Guaranteed Delaunay Triangulations," Proceedings of LATIN      */
/*    2004 (Buenos Aires, Argentina), April 2004.                            */
/*                                                                           */
/*  For definitions of and results involving Delaunay triangulations,        */
/*    constrained and conforming versions thereof, and other aspects of      */
/*    triangular mesh generation, see the excellent survey by Marshall Bern  */
/*    and David Eppstein, "Mesh Generation and Optimal Triangulation," in    */
/*    Computing and Euclidean Geometry, Ding-Zhu Du and Frank Hwang,         */
/*    editors, World Scientific, Singapore, pp. 23-90, 1992.  [*]            */
/*                                                                           */
/*  The time for incrementally adding PSLG (planar straight line graph)      */
/*    segments to create a constrained Delaunay triangulation is probably    */
/*    O(t^2) per segment in the worst case and O(t) per segment in the       */
/*    common case, where t is the number of triangles that intersect the     */
/*    segment before it is inserted.  This doesn't count point location,     */
/*    which can be much more expensive.  I could improve this to O(d log d)  */
/*    time, but d is usually quite small, so it's not worth the bother.      */
/*    (This note does not apply when the -s switch is used, invoking a       */
/*    different method is used to insert segments.)                          */
/*                                                                           */
/*  The time for deleting a vertex from a Delaunay triangulation is O(d^2)   */
/*    in the worst case and O(d) in the common case, where d is the degree   */
/*    of the vertex being deleted.  I could improve this to O(d log d) time, */
/*    but d is usually quite small, so it's not worth the bother.            */
/*                                                                           */
/*  Ruppert's Delaunay refinement algorithm typically generates triangles    */
/*    at a linear rate (constant time per triangle) after the initial        */
/*    triangulation is formed.  There may be pathological cases where        */
/*    quadratic time is required, but these never arise in practice.         */
/*                                                                           */
/*  The geometric predicates (circumcenter calculations, segment             */
/*    intersection formulae, etc.) appear in my "Lecture Notes on Geometric  */
/*    Robustness" at http://www.cs.berkeley.edu/~jrs/mesh .                  */
/*                                                                           */
/*  If you make any improvements to this code, please please please let me   */
/*    know, so that I may obtain the improvements.  Even if you don't change */
/*    the code, I'd still love to hear what it's being used for.             */
/*                                                                           */
/*****************************************************************************/

#ifndef VERTEX_HPP
#define VERTEX_HPP
#include<cmath>
#include<stdexcept>
#include<format>
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

    static Vertex findCircumcenter(const Vertex& vertex0, const Vertex& vertex1, const Vertex& vertex2) {
        Vertex origin;
        origin.x = 0;
        origin.y = 0;
        Vertex vertex1Displaced = vertex1 - vertex0;
        Vertex vertex2Displaced = vertex2 - vertex0;
        double determinant = 2 * origin.cross2d(vertex1Displaced,vertex2Displaced);
        if (std::fabs(determinant) < EPSILON) {
            throw std::runtime_error(std::format(
                "Degenerate triangle found\n Vertices: \n{},{}\n {},{}\n {},{}",
                vertex0.x, vertex0.y,
                vertex1.x, vertex1.y,
                vertex2.x, vertex2.y
            ));
        }
        // 1/D [ C_y * (B_x^2 + B_y^2) - B_y * (C_x^2 + C_y^2)]
        double circumcenterX = (vertex2Displaced.y * vertex1Displaced.dot(vertex1Displaced) - vertex1Displaced.y * vertex2Displaced.dot(vertex2Displaced)) / determinant;
        double circumcenterY = (vertex1Displaced.x * vertex2Displaced.dot(vertex2Displaced) - vertex2Displaced.x * vertex1Displaced.dot(vertex1Displaced)) / determinant;
        circumcenterX += vertex0.x;
        circumcenterY += vertex0.y;
        Vertex circumcenter;
        circumcenter.x = circumcenterX;
        circumcenter.y = circumcenterY;
        return circumcenter;
    }
     /**
     * Returns true if point `P` lies inside the circumcircle of triangle ABC or is cocircular.
     * Assumes triangle ABC is oriented counter-clockwise (CCW).
     */
    static bool inCircle(const Vertex& A, const Vertex& B, const Vertex& C, const Vertex& P) {
        Vertex pa = A - P;
        Vertex pb = B - P;
        Vertex pc = C - P;

        double paLenSq = pa.dot(pa);
        double pbLenSq = pb.dot(pb);
        double pcLenSq = pc.dot(pc);

        double det = paLenSq * P.cross2d(B,C)
                   + pbLenSq * P.cross2d(C,A)
                   + pcLenSq * P.cross2d(A,B);

        return det >= 0 || std::fabs(det) < EPSILON;
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