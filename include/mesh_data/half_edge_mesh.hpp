#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP
#include <vector>
#include <mesh_data/structures/he_vertex.hpp>
#include <mesh_data/structures/half_edge.hpp>
#include <cmath>
#include <unordered_map>
#include <mesh_refiners/mesh_refiner.hpp>
#include <concepts/mesh_data.hpp>
#include <concepts/has_adjacency_constructor.hpp>

class HalfEdgeMesh {
    public:
        using VertexIndex = int;
        using EdgeIndex = int;
        using FaceIndex = int;
        using OutputIndex = EdgeIndex;
        using VertexType = HEVertex;
        using EdgeType = HalfEdge;
    private:
        std::vector<VertexType> vertices;
        std::vector<EdgeType> halfEdges;
        std::vector<FaceIndex> polygons;
        size_t nVertices;
        size_t nPolygons;
        size_t nHalfEdges;
        void constructInteriorHalfEdgesFromFacesAndNeighs(std::vector<FaceIndex> &faces, std::vector<FaceIndex> &neighbors);
        void constructInteriorHalfEdgesFromFaces(std::vector<FaceIndex> &faces);
        void constructExteriorHalfEdges();
    public:
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<FaceIndex> faces);
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<FaceIndex> faces, std::vector<FaceIndex> neighbors);
         /**
         * Retrieves the vertices of the triangle stored at `polygonIndex`.
         * @param polygonIndex An index to the triangle whose vertices we need
         * @param vertex0 A reference to a vertex we will set, this can be any vertex.
         * @param vertex1 A reference to another vertex we will set, this vertex must be the next vertex on a CCW orientation to `vertex1`
         * @param vertex2 A reference to the last vertex we will set, this vertex must be the last vertex on a CCW orientation such that `vertex0` -> `vertex1` -> `vertex2`
         */
        void getVerticesOfTriangle(FaceIndex polygonIndex, Vertex& v0, Vertex& v1, Vertex& v2) const;
        /**
         * @param triangle A triangle index
         * @return An array with 3 indices that point to the CCW edges of `triangle`
         */
        std::array<EdgeIndex,3> getEdgesOfTriangle(FaceIndex triangle) const;
        VertexType& getVertex(VertexIndex v) {
            return vertices.at(v);
        }
        EdgeType& getEdge(EdgeIndex e) {
            return halfEdges.at(e);
        }
        std::vector<HalfEdge> getEdges() const {
            return halfEdges;
        }
        void setEdges(std::vector<HalfEdge>& edges) {
            halfEdges = std::move(edges);
        }
        /**
         * Returns the polygon index (face) associated with this half edge
         * @param e An index of an edge whose face we need
         * @return The polygon index associated with edge `e`
         */
        FaceIndex getFaceOfEdge(EdgeIndex e) const {
            return halfEdges.at(e).face;
        }
        std::pair<FaceIndex,FaceIndex> getFacesAssociatedWithEdge(EdgeIndex e) const {
            return {getFaceOfEdge(e), getFaceOfEdge(twin(e))};
        }
        /**
         * Every 3 indices of the polygons vector are a face, so we'll get the half edge that satisfies that
         * the vertex at polygon + 1 and the vertex at polygon + 2 are its next and prev / prev and next
         */
        EdgeIndex getPolygon(FaceIndex polygon) const {
            int timesThree = polygon * 3;
            VertexIndex vertexIdx = polygons.at(timesThree);
            VertexIndex vertexIdx2 = polygons.at(timesThree + 1);
            VertexIndex vertexIdx3 = polygons.at(timesThree + 2);
            const HEVertex& aVertex = vertices.at(vertexIdx);
            EdgeIndex incidentHE = aVertex.incidentHalfEdge;
            while ( halfEdges.at(incidentHE).face == -1 || (!(target(incidentHE) == vertexIdx2) && !(target(next(incidentHE)) == vertexIdx3)) ) {
                // Some edge that goes from the vertex at vertexIdx must satisfy the condition that it is part of an internal triangle
                // and it's target and target of the next are the other vertices
                incidentHE = CCWEdgeToVertex(incidentHE);
            }
            return incidentHE;
        }
        size_t numberOfVertices() const {
            return nVertices;
        }
        size_t numberOfEdges() const {
            return nHalfEdges;
        }
        size_t numberOfPolygons() const {
            return nPolygons;
        }
        void updateVertexCount(size_t newAmount) {
            this->nVertices = newAmount;
        }
        void updateEdgeCount(size_t newAmount) {
            this->nHalfEdges = newAmount;
        }
        void updatePolygonCount(size_t newAmount) {
            this->nPolygons = newAmount;
        }
        /**
         * Updates the `next` edge of edge `edge` to `nextEdge`
         * @param edge The edge that needs its next edge updated
         * @param nextEdge The new next edge
         */
        void setNext(EdgeIndex edge, EdgeIndex nextEdge) {
            halfEdges.at(edge).next = nextEdge;
        }
        /**
         * Updates the `prev` edge of edge `edge` to `previousEdge`
         * @param edge The edge that needs its prev edge updated
         * @param previousEdge The new previous edge
         */
        void setPrev(EdgeIndex edge, EdgeIndex previousEdge) {
            halfEdges.at(edge).prev = previousEdge;
        }
        /**
         * Updates the `twin` edge of edge `edge` to `newTwin`
         * @param edge The edge that needs its twin edge updated
         * @param newTwin The new twin edge
         */
        void setTwin(EdgeIndex edge, EdgeIndex newTwin) {
            halfEdges.at(edge).twin = newTwin;
        }
        /**
         * Updates the `polygons` vector so the face at `polygonIndex` now points
         * to `identifyingEdge`
         * @param polygonIndex The face that needs its starting edge updated
         * @param identifyingEdge The edge that will now identify this face
         */
        void setFaceToEdge(FaceIndex polygonIndex, EdgeIndex identifyingEdge) {
            polygons[polygonIndex] = identifyingEdge;
        }
        /**
         * Updates `edge`'s `.face` attribute to store the value of `newFace`
         * @param edge An edge whose associated face has to change
         * @param newFace The new face that `edge` belongs to
         */
        void setEdgeToFace(EdgeIndex edge, FaceIndex newFace) {
            halfEdges.at(edge).face = newFace;
        }
        std::vector<FaceIndex> getNeighbors(FaceIndex polygon) const;
        /**
         * Calculates the tail vertex of the edge `edge`
         * 
         * @param edge The edge whose origin we want to get
         * @return Index to the tail vertex `v` of the edge `edge`
        */
        VertexIndex origin(EdgeIndex edge) const;
        /**
         * Calculates the next edge of the face incident to edge `edge`
         * 
         * This edge's origin is `edge`'s target
         * 
         * @param edge The edge we want to get the next edge from
         * @return Index to the next edge of the face incident to `edge`
         */
        EdgeIndex next(EdgeIndex edge) const;
        /**
         * Calculates the head vertex of the edge `edge`
         * 
         * @param edge The edge whose target we want to get
         * @return Index to the head vertex `v` of the edge `edge` 
         */
        VertexIndex target(EdgeIndex edge) const;
        /**
         * Returns the twin edge of the edge `edge`
         * @param edge The edge whose twin we want to get
         * @return Index to twin edge of `edge`
         */
        EdgeIndex twin(VertexIndex edge) const;
        /**
         * Returns the previous edge of the face incident to edge `edge`
         * 
         * This edge's target is `edge`'s origin
         * 
         * @param edge The edge whose previous edge we want to get
         * @return Index to the previous edge of the face incident to `edge`
         */
        EdgeIndex prev(VertexIndex edge) const;
        /**
         * Returns the next counterclockwise edge of the origin vertex `v` of edge `edge`
         * 
         * This is the `twin` of the `prev` edge of `edge`
         * 
         * @param edge The edge with an origin `v` whose next ccw edge we need
         * @return Index to next counterclockwise edge
         */
        EdgeIndex CCWEdgeToVertex(EdgeIndex e) const;
        /**
         * Returns the next clockwise edge of the origin vertex `v` of edge `edge`
         * 
         * This is the `next` of the `twin` edge of `edge`
         * 
         * @param edge The edge with an origin `v` whose next cw edge we need
         * @return Index to next clockwise edge
         */
        EdgeIndex CWEdgeToVertex(EdgeIndex edge) const;
        /**
         * Returns the index of the edge associated with the vertex `vertex`
         * @param v Vertex whose edge we want to get
         * @return An index to the mesh's vector of HalfEdges with the edge of `vertex`
         */
        EdgeIndex edgeOfVertex(VertexIndex vertex) const;
        /**
         * @param edge Edge we want to query from
         * @return Whether `edge` is a border edge
         */
        bool isBorderEdge(EdgeIndex edge) const;
        /**
         * @param triangle1 A triangle in the mesh
         * @param triangle2 A possible neighbor of `triangle1`
         * @return The shared edge between `triangle1` and `triangle2` as the index of the
         * half edge in CCW for `triangle1`, or -1 if these triangles aren't neighbors
         */
        EdgeIndex getSharedEdge(FaceIndex triangle1, FaceIndex triangle2) const;
        /**
         * @param vertex Vertex we want to query
         * @return Whether this vertex `vertex` is part of the border of the polygon
         */
        bool isBorderVertex(VertexIndex vertex) const;
        /**
         * Returns the degree of vertex `vertex`
         * 
         * The degree of a vertex `vertex` is equivalent to how many half edges have `vertex` as its origin
         * 
         * @param vertex Vertex whose degree we want to calculate
         * @returns Amount of half edges that have `v` as its origin
         */
        unsigned int degree(VertexIndex vertex) const;
        /**
         * Returns the squared length of the edge `edge`.
         * 
         * This saves the cost of calculating the square root which
         * is not necessary in most cases
         * @param edge The edge whose length is queried
         * @returns The squared length of the edge `edge`
         */
        double edgeLength2(EdgeIndex edge) const;

};

template<> inline constexpr bool isMeshData<HalfEdgeMesh> = true;
/** 
 * These lines must be here after the class is completely defined to make sure the
 * class adheres to the concept, it gets deleted on compilation
 * 
 * This is analogous to Java's "implements"
 * It is only used here due to HalfEdgeMesh not being templated itself, otherwise, it's not applicable
 */
static_assert(MeshData<HalfEdgeMesh>);
static_assert(HasAdjacencyConstructor<HalfEdgeMesh>);

#include<mesh_data/half_edge_mesh.ipp>

#endif
