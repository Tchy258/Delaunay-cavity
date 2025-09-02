#ifndef HALF_EDGE_MESH_HPP
#define HALF_EDGE_MESH_HPP
#include <vector>
#include <mesh_data/structures/he_vertex.hpp>
#include <mesh_data/structures/half_edge.hpp>
#include <cmath>
#include <unordered_map>
#include <mesh_refiners/mesh_refiner.hpp>

class HalfEdgeMesh {
    private:
        std::vector<HEVertex> vertices;
        std::vector<HalfEdge> halfEdges;
        std::vector<int> polygons;
        size_t nVertices;
        size_t nPolygons;
        size_t nHalfEdges;
        void constructInteriorHalfEdgesFromFacesAndNeighs(std::vector<int> &faces, std::vector<int> &neighbors);
        void constructInteriorHalfEdgesFromFaces(std::vector<int> &faces);
        void constructExteriorHalfEdges();
    public:
        using VertexIndex = int;
        using EdgeIndex = int;
        using VertexType = HEVertex;
        using EdgeType = HalfEdge;
        
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces);
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces, std::vector<int> neighbors);
         /**
         * Retrieves the vertices of the triangle stored at `polygonIndex`.
         * @param polygonIndex An index to the triangle whose vertices we need
         * @param vertex0 A reference to a vertex we will set, this can be any vertex.
         * @param vertex1 A reference to another vertex we will set, this vertex must be the next vertex on a CCW orientation to `vertex1`
         * @param vertex2 A reference to the last vertex we will set, this vertex must be the last vertex on a CCW orientation such that `vertex0` -> `vertex1` -> `vertex2`
         */
        void getVerticesOfTriangle(int polygonIndex, Vertex& v0, Vertex& v1, Vertex& v2);
        VertexType& getVertex(VertexIndex v) {
            return vertices.at(v);
        }
        EdgeType& getEdge(EdgeIndex e) {
            return halfEdges.at(e);
        }
        /**
         * Returns the polygon index (face) associated with this half edge
         * @param e An index of an edge whose face we need
         * @return The polygon index associated with edge `e`
         */
        int getFaceOfEdge(EdgeIndex e) const {
            return halfEdges.at(e).face;
        }
        /**
         * Every 3 indices of the polygons vector are a face, so we'll get the half edge that satisfies that
         * the vertex at polygon + 1 and the vertex at polygon + 2 are its next and prev / prev and next
         */
        EdgeIndex getPolygon(int polygon) const {
            int timesThree = polygon * 3;
            int vertexIdx = polygons.at(timesThree);
            int vertexIdx2 = polygons.at(timesThree + 1);
            int vertexIdx3 = polygons.at(timesThree + 2);
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
        void setFace(int polygonIndex, EdgeIndex identifyingEdge) {
            polygons[polygonIndex] = identifyingEdge;
        }
        std::vector<int> getNeighbors(int polygon);
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
         * @return Whether the face of `edge` is a border face
         */
        bool isBorderFace(EdgeIndex edge) const;
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

#include<template_implementations/mesh_data/half_edge_mesh.ipp>

#endif
