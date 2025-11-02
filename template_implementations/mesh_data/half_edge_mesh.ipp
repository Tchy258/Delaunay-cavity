#ifndef HALF_EDGE_MESH_HPP
#include <mesh_data/half_edge_mesh.hpp>
#endif

HalfEdgeMesh::VertexIndex HalfEdgeMesh::origin(HalfEdgeMesh::EdgeIndex edge) const {
    return halfEdges.at(edge).origin;
}

std::vector<HalfEdgeMesh::FaceIndex> HalfEdgeMesh::getNeighbors(HalfEdgeMesh::FaceIndex polygon) const {
    EdgeIndex firstEdge = getPolygon(polygon);
    std::vector<FaceIndex> neighbors;
    EdgeIndex currentEdge = firstEdge;
    do {
        EdgeIndex twinIdx = twin(currentEdge);
        if (!isBorderEdge(twinIdx)) {
            neighbors.push_back(halfEdges.at(twinIdx).face);
        }
        currentEdge = next(currentEdge);
    } while (currentEdge != firstEdge);
    return neighbors;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::next(HalfEdgeMesh::EdgeIndex edge) const {
    return halfEdges.at(edge).next;
}

HalfEdgeMesh::VertexIndex HalfEdgeMesh::target(HalfEdgeMesh::EdgeIndex edge) const {
    return origin(halfEdges.at(edge).twin);
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::twin(HalfEdgeMesh::VertexIndex edge) const {
    return halfEdges.at(edge).twin;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::prev(HalfEdgeMesh::VertexIndex edge) const {
    return halfEdges.at(edge).prev;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::CCWEdgeToVertex(HalfEdgeMesh::EdgeIndex edge) const {
    int twinIdx, prevIdx;
    prevIdx = halfEdges.at(edge).prev;
    twinIdx = halfEdges.at(prevIdx).twin;
    return twinIdx;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::CWEdgeToVertex(HalfEdgeMesh::EdgeIndex edge) const {
    int twinIdx, nextIdx;
    twinIdx = halfEdges.at(edge).twin;
    nextIdx = halfEdges.at(twinIdx).next;
    return nextIdx;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::edgeOfVertex(HalfEdgeMesh::VertexIndex vertex) const {
    return vertices.at(vertex).incidentHalfEdge;
}

bool HalfEdgeMesh::isBorderEdge(HalfEdgeMesh::EdgeIndex edge) const {
    return halfEdges.at(edge).isBorder;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::getSharedEdge(HalfEdgeMesh::FaceIndex triangle1, HalfEdgeMesh::FaceIndex triangle2) const {
    EdgeIndex firstEdge = getPolygon(triangle1);
    EdgeIndex currentEdge = firstEdge;

    do {
        if (twin(currentEdge) != -1 && halfEdges[twin(currentEdge)].face == triangle2) {
            return currentEdge;
        }
        currentEdge = next(currentEdge);
    } while (firstEdge != currentEdge);
    return -1;
}

bool HalfEdgeMesh::isBorderVertex(HalfEdgeMesh::VertexIndex vertex) const {
    return vertices.at(vertex).isBorder;
}

unsigned int HalfEdgeMesh::degree(HalfEdgeMesh::VertexIndex vertex) const {
    int currentEdge = edgeOfVertex(vertex);
    int nextEdge = CCWEdgeToVertex(currentEdge);
    unsigned int count = 1;
    while (nextEdge != currentEdge) {
        nextEdge = CCWEdgeToVertex(nextEdge);
        count++;
    }
    return count;
}

double HalfEdgeMesh::edgeLength2(HalfEdgeMesh::EdgeIndex edge) const {
    HalfEdgeMesh::VertexType originVertex = vertices.at(origin(edge));
    HalfEdgeMesh::VertexType targetVertex = vertices.at(target(edge));
    Vertex vectorFromTargetToOrigin = targetVertex - originVertex;
    return vectorFromTargetToOrigin.dot(vectorFromTargetToOrigin); //Same as squared distance
}



HalfEdgeMesh::HalfEdgeMesh(std::vector<HalfEdgeMesh::VertexType> vertices,
                           std::vector<HalfEdgeMesh::EdgeType> edges,
                           std::vector<HalfEdgeMesh::FaceIndex> faces) : vertices(vertices), halfEdges(edges), polygons(faces) {
    
    this->nPolygons = faces.size() / 3;
    constructInteriorHalfEdgesFromFaces(this->polygons);
    constructExteriorHalfEdges();
    this->nHalfEdges = halfEdges.size();
    this->nVertices = vertices.size();
}

HalfEdgeMesh::HalfEdgeMesh(std::vector<HalfEdgeMesh::VertexType> vertices,
    std::vector<HalfEdgeMesh::EdgeType> edges,
    std::vector<int> faces,
    std::vector<int> neighbors):
    vertices(vertices), halfEdges(edges), polygons(faces) {
    this->nPolygons = faces.size() / 3;
    constructInteriorHalfEdgesFromFacesAndNeighs(this->polygons, neighbors);
    constructExteriorHalfEdges();
    this->nHalfEdges = halfEdges.size();
    this->nVertices = vertices.size();
}

inline void HalfEdgeMesh::getVerticesOfTriangle(HalfEdgeMesh::FaceIndex polygonIndex, Vertex& v0, Vertex& v1, Vertex& v2) const {
    // We look for this edge first to guarantee CCW ordering
    EdgeIndex firstEdge = getPolygon(polygonIndex);
    v0 = vertices.at(origin(firstEdge));
    v1 = vertices.at(target(firstEdge));
    v2 = vertices.at(target(next(firstEdge)));
}

inline std::array<HalfEdgeMesh::EdgeIndex, 3> HalfEdgeMesh::getEdgesOfTriangle(FaceIndex triangle) const {
    EdgeIndex edge1 = getPolygon(triangle);
    EdgeIndex edge2 = next(edge1);
    EdgeIndex edge3 = next(edge2);
    return {edge1, edge2, edge3};
}

inline bool HalfEdgeMesh::isPolygonConvex(EdgeIndex firstEdgeOfPolygon) const {
    VertexType v1 = vertices.at(origin(firstEdgeOfPolygon));
    VertexType v2 = vertices.at(target(firstEdgeOfPolygon));
    VertexType v3 = vertices.at(target(next(firstEdgeOfPolygon)));

    bool positive = v1.cross2d(v2, v3) > 0;

    EdgeIndex currentEdge = next(firstEdgeOfPolygon);
    do {
        v1 = v2;
        v2 = v3;
        currentEdge = next(currentEdge);
        v3 = vertices.at(target(currentEdge));

        double cross = v1.cross2d(v2, v3);
        if (positive && cross <= 0) return false;
        if (!positive && cross > 0) return false;

    } while (currentEdge != firstEdgeOfPolygon);

    return true;
}

void HalfEdgeMesh::constructInteriorHalfEdgesFromFacesAndNeighs(std::vector<HalfEdgeMesh::FaceIndex> &faces, std::vector<HalfEdgeMesh::FaceIndex> &neighbors) {
    int neigh, origin, target;
    for(std::size_t i = 0; i < nPolygons; i++){
        for(std::size_t j = 0; j < 3; j++){
            HalfEdge he;
            neigh = neighbors.at(3*i + ((j+2)%3));
            origin = faces[3*i+j];
            target = faces[3*i+((j+1)%3)];

            he.origin = origin;
            // he.target = target;
            he.next = 3*i + ((j+1)%3);
            he.prev = 3*i + ((j+2)%3);
            he.face = i;
            he.isBorder = (neigh == -1);
            if(neigh != -1) {
                for (std::size_t j = 0; j < 3; j++){
                    if(faces.at(3*neigh + j) == target && faces.at(3*neigh + (j + 1)%3) == origin) {
                        he.twin = 3*neigh + j;
                        break;
                    }
                }
            } else {
                he.twin = -1;
            }
            halfEdges.push_back(he);
            vertices[he.origin].incidentHalfEdge = i*3 + j;
        }
    }
}

void HalfEdgeMesh::constructInteriorHalfEdgesFromFaces(std::vector<HalfEdgeMesh::FaceIndex> &faces) {
    using _edge = std::pair<VertexIndex,VertexIndex>;
    size_t n_faces = this->nPolygons;
    //std::cout << "0. aca "<< std::endl;	
    auto hash_for_pair = [n = 3* n_faces](const std::pair<VertexIndex, VertexIndex>& p) {
        return std::hash<VertexIndex>{}(p.first)*n + std::hash<VertexIndex>{}(p.second);
    };
    std::unordered_map<_edge, EdgeIndex, decltype(hash_for_pair)> map_edges(3* n_faces, hash_for_pair); //set of edges to calculate the boundary and twin edges
    //std::cout << "1. aca "<< std::endl;
    for(std::size_t i = 0; i < n_faces; i++){
        for(std::size_t j = 0; j < 3; j++){
            HalfEdge he;
            VertexIndex v_origin = faces.at(3*i+j);
            VertexIndex v_target = faces.at(3*i+(j+1)%3);
            he.origin = v_origin;
            he.next = i*3+(j+1)%3;
            he.prev = i*3+(j+2)%3;
            he.isBorder = false;
            he.twin = -1;
            he.face = i;
            vertices.at(v_origin).incidentHalfEdge = i*3+j;
            map_edges[std::make_pair(v_origin, v_target)] = i*3+j;
            halfEdges.push_back(he);
        }
        //std::cout << i << std::endl;
    }
    //std::cout << "2. aca "<< std::endl;	
    
    //Calculate twin halfedge and boundary halfedges from set_edges
    std::unordered_map<_edge,EdgeIndex, decltype(hash_for_pair)>::iterator it;
    for(std::size_t i = 0; i < halfEdges.size(); i++){
        //if halfedge has no twin
        if(halfEdges.at(i).twin == -1){
            VertexIndex edgeTarget = origin(next(i));
            VertexIndex edgeOrigin = origin(i);
            _edge twin = std::make_pair(edgeTarget, edgeOrigin);
            it=map_edges.find(twin);
            //if twin is found
            if(it!=map_edges.end()){
                EdgeIndex index_twin = it->second;
                halfEdges.at(i).twin = index_twin;
                halfEdges.at(index_twin).twin = i;
            }else{ //if twin is not found and halfedge is on the boundary
                halfEdges.at(i).isBorder = true;
                vertices.at(edgeOrigin).isBorder = true;
                vertices.at(edgeTarget).isBorder = true;
            }
        }
    }
    //std::cout << "3. aca "<< std::endl;	
}

void HalfEdgeMesh::constructExteriorHalfEdges() {
    //search interior edges labed as border, generates exterior edges
        //with the origin and target inverted and add at the of HalfEdges vector
        //std::cout<<"Size vector: "<<HalfEdges.size()<<std::endl;
        unsigned int n_halfedges = halfEdges.size();
        for(std::size_t i = 0; i < n_halfedges; i++){
            if(halfEdges.at(i).isBorder){
                HalfEdge he_aux;
                he_aux.face = -1;
                he_aux.twin = i;
                he_aux.origin = origin(next(i));
                //he_aux.target = HalfEdges.at(i).origin;
                he_aux.isBorder = true;
                halfEdges.at(i).isBorder = false;
                
                halfEdges.push_back(he_aux);
                halfEdges.at(i).twin = halfEdges.size() - 1 ;
            }    
        }
        //traverse the exterior edges and search their next prev halfedge
        EdgeIndex nextCCW, prevCCW;
        for(std::size_t i = n_halfedges; i < halfEdges.size(); i++){
            if(halfEdges.at(i).isBorder){
                nextCCW = CCWEdgeToVertex(halfEdges.at(i).twin);
                while (halfEdges.at(nextCCW).isBorder != true) {
                    nextCCW = this->CCWEdgeToVertex(nextCCW);
                }
                halfEdges.at(i).next = nextCCW;

                prevCCW = this->next(twin(i));
                while (halfEdges.at(halfEdges.at(prevCCW).twin).isBorder != true) {
                    prevCCW = this->CWEdgeToVertex(prevCCW);
                }
                halfEdges.at(i).prev = halfEdges.at(prevCCW).twin;
            }
        }
}