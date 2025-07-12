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
        void constructInteriorHalfEdgesFromFacesAndNeighs(std::vector<int> &faces, std::vector<int> &neighbors);
        void constructInteriorHalfEdgesFromFaces(std::vector<int> &faces);
        void constructExteriorHalfEdges();
    public:
        using VertexIndex = int;
        using EdgeIndex = int;
        using VertexType = HEVertex;
        using EdgeType = HalfEdge;
        
        HalfEdgeMesh(const HalfEdgeMesh& other) {
            this->vertices = other.vertices;
            this->halfEdges = other.halfEdges;
            this->polygons = other.polygons;
        }
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces);
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces, bool alreadyRefined);
        HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces, std::vector<int> neighbors);

        VertexType& getVertex(VertexIndex v) {
            return vertices.at(v);
        };
        EdgeType& getEdge(EdgeIndex e) {
            return halfEdges.at(e);
        };
        EdgeIndex getPolygon(int polygon) {
            return polygons.at(polygon);
        }
        size_t numberOfVertices() {
            return vertices.size();
        }
        size_t numberOfEdges() {
            return halfEdges.size();
        }
        size_t numberOfPolygons() {
            return polygons.size();
        }
        /**
         * Given a map of pairs `(face, edgesToKeep)`, deletes the edges not present in the map.
         * 
         * This inserts the "cavity"
         */
        void cavityInsertion(std::unordered_map<int,std::vector<EdgeIndex>> cavityMap);
        std::vector<int> getNeighbors(int polygon);
        /**
         * Calculates the tail vertex of the edge `edge`
         * 
         * @param edge The edge whose origin we want to get
         * @return Index to the tail vertex `v` of the edge `edge`
        */
        VertexIndex origin(EdgeIndex edge);
        /**
         * Calculates the next edge of the face incident to edge `edge`
         * 
         * This edge's origin is `edge`'s target
         * 
         * @param edge The edge we want to get the next edge from
         * @return Index to the next edge of the face incident to `edge`
         */
        EdgeIndex next(EdgeIndex edge);
        /**
         * Calculates the head vertex of the edge `edge`
         * 
         * @param edge The edge whose target we want to get
         * @return Index to the head vertex `v` of the edge `edge` 
         */
        VertexIndex target(EdgeIndex edge);
        /**
         * Returns the twin edge of the edge `edge`
         * @param edge The edge whose twin we want to get
         * @return Index to twin edge of `edge`
         */
        EdgeIndex twin(VertexIndex edge);
        /**
         * Returns the previous edge of the face incident to edge `edge`
         * 
         * This edge's target is `edge`'s origin
         * 
         * @param edge The edge whose previous edge we want to get
         * @return Index to the previous edge of the face incident to `edge`
         */
        EdgeIndex prev(VertexIndex edge);
        /**
         * Returns the next counterclockwise edge of the origin vertex `v` of edge `edge`
         * 
         * This is the `twin` of the `prev` edge of `edge`
         * 
         * @param edge The edge with an origin `v` whose next ccw edge we need
         * @return Index to next counterclockwise edge
         */
        EdgeIndex CCWEdgeToVertex(EdgeIndex e);
        /**
         * Returns the next clockwise edge of the origin vertex `v` of edge `edge`
         * 
         * This is the `next` of the `twin` edge of `edge`
         * 
         * @param edge The edge with an origin `v` whose next cw edge we need
         * @return Index to next clockwise edge
         */
        EdgeIndex CWEdgeToVertex(EdgeIndex edge);
        /**
         * Returns the index of the edge associated with the vertex `vertex`
         * @param v Vertex whose edge we want to get
         * @return An index to the mesh's vector of HalfEdges with the edge of `vertex`
         */
        EdgeIndex edgeOfVertex(VertexIndex vertex);
        /**
         * @param edge Edge we want to query from
         * @return Whether the face of `edge` is a border face
         */
        bool isBorderFace(EdgeIndex edge);
        /**
         * @param vertex Vertex we want to query
         * @return Whether this vertex `vertex` is part of the border of the polygon
         */
        bool isBorderVertex(VertexIndex vertex);
        /**
         * Returns the degree of vertex `vertex`
         * 
         * The degree of a vertex `vertex` is equivalent to how many half edges have `vertex` as its origin
         * 
         * @param vertex Vertex whose degree we want to calculate
         * @returns Amount of half edges that have `v` as its origin
         */
        unsigned int degree(VertexIndex vertex);
        /**
         * Returns the squared length of the edge `edge`.
         * 
         * This saves the cost of calculating the square root which
         * is not necessary in most cases
         * @param edge The edge whose length is queried
         * @returns The squared length of the edge `edge`
         */
        double edgeLength2(EdgeIndex edge);
};

#include<template_implementations/mesh_data/half_edge_mesh.ipp>

#endif
