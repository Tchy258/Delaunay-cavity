#ifndef HALF_EDGE_MESH_HPP
#include <mesh_data/half_edge_mesh.hpp>
#include "half_edge_mesh.hpp"
#endif

HalfEdgeMesh::VertexIndex HalfEdgeMesh::origin(HalfEdgeMesh::EdgeIndex edge) {

    return halfEdges.at(edge).origin;
}

std::vector<int> HalfEdgeMesh::getNeighbors(int polygon) {
    EdgeIndex firstEdge = polygons.at(polygon);
    std::vector<int> neighbors;
    neighbors.push_back(twin(firstEdge));
    EdgeIndex currentEdge = next(firstEdge);
    while (currentEdge != firstEdge) {
        if (!isBorderFace(currentEdge)) {
            neighbors.push_back(twin(currentEdge));
        }
        currentEdge = next(currentEdge);
    }
    return neighbors;
}
HalfEdgeMesh::EdgeIndex HalfEdgeMesh::next(HalfEdgeMesh::EdgeIndex edge) {
    return halfEdges.at(edge).next;
}

HalfEdgeMesh::VertexIndex HalfEdgeMesh::target(HalfEdgeMesh::EdgeIndex edge) {
    return origin(halfEdges.at(edge).twin);
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::twin(HalfEdgeMesh::VertexIndex edge) {
    return halfEdges.at(edge).twin;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::prev(HalfEdgeMesh::VertexIndex edge) {
    return halfEdges.at(edge).prev;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::CCWEdgeToVertex(HalfEdgeMesh::EdgeIndex edge) {
    int twinIdx, prevIdx;
    prevIdx = halfEdges.at(edge).prev;
    twinIdx = halfEdges.at(prevIdx).twin;
    return twinIdx;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::CWEdgeToVertex(HalfEdgeMesh::EdgeIndex edge) {
    int twinIdx, nextIdx;
    twinIdx = halfEdges.at(edge).twin;
    nextIdx = halfEdges.at(twinIdx).next;
    return nextIdx;
}

HalfEdgeMesh::EdgeIndex HalfEdgeMesh::edgeOfVertex(HalfEdgeMesh::VertexIndex vertex) {
    return vertices.at(vertex).incidentHalfEdge;
}

bool HalfEdgeMesh::isBorderFace(HalfEdgeMesh::EdgeIndex edge) {
    return halfEdges.at(edge).isBorder;
}

bool HalfEdgeMesh::isBorderVertex(HalfEdgeMesh::VertexIndex vertex) {
    return vertices.at(vertex).isBorder;
}

unsigned int HalfEdgeMesh::degree(HalfEdgeMesh::VertexIndex vertex) {
    int currentEdge = edgeOfVertex(vertex);
    int nextEdge = CCWEdgeToVertex(currentEdge);
    unsigned int count = 1;
    while (nextEdge != currentEdge) {
        nextEdge = CCWEdgeToVertex(nextEdge);
        count++;
    }
    return count;
}

double HalfEdgeMesh::edgeLength2(HalfEdgeMesh::EdgeIndex edge) {
    HalfEdgeMesh::VertexType originVertex = vertices.at(origin(edge));
    HalfEdgeMesh::VertexType targetVertex = vertices.at(target(edge));
    Vertex vectorFromTargetToOrigin = targetVertex - originVertex;
    return vectorFromTargetToOrigin.dot(vectorFromTargetToOrigin); //Same as squared distance
}

HalfEdgeMesh::HalfEdgeMesh(std::vector<VertexType> vertices, std::vector<EdgeType> edges, std::vector<int> faces) : HalfEdgeMesh::HalfEdgeMesh(vertices, edges, faces, false) {}


HalfEdgeMesh::HalfEdgeMesh(std::vector<HalfEdgeMesh::VertexType> vertices,
                           std::vector<HalfEdgeMesh::EdgeType> edges,
                           std::vector<int> faces,
                           bool alreadyRefined) : vertices(vertices), halfEdges(edges), polygons(faces)
{
    if (!alreadyRefined) {
        constructInteriorHalfEdgesFromFaces(this->polygons);
        constructExteriorHalfEdges();
    }
}

HalfEdgeMesh::HalfEdgeMesh(std::vector<HalfEdgeMesh::VertexType> vertices,
    std::vector<HalfEdgeMesh::EdgeType> edges,
    std::vector<int> faces,
    std::vector<int> neighbors):
    vertices(vertices), halfEdges(edges), polygons(faces) {
    constructInteriorHalfEdgesFromFacesAndNeighs(this->polygons, neighbors);
    constructExteriorHalfEdges();
}

void HalfEdgeMesh::constructInteriorHalfEdgesFromFacesAndNeighs(std::vector<int> &faces, std::vector<int> &neighbors)
{
    int neigh, origin, target;
    for(std::size_t i = 0; i < faces.size(); i++){
        for(std::size_t j = 0; j < 3; j++){
            HalfEdge he;
            neigh = neighbors.at(3*i + ((j+2)%3));
            origin = faces[3*i+j];
            target = faces[3*i+((j+1)%3)];

            he.origin = origin;
            // he.target = target;
            he.next = 3*i + ((j+1)%3);
            he.prev = 3*i + ((j+2)%3);
            // he.face = i;
            he.isBorder = (neigh == -1);
            if(neigh != -1){
                for (std::size_t j = 0; j < 3; j++){
                    if(faces.at(3*neigh + j) == target && faces.at(3*neigh + (j + 1)%3) == origin){
                        he.twin = 3*neigh + j;
                        break;
                    }
                }
            }else
                he.twin = -1;
            halfEdges.push_back(he);
            vertices[he.origin].incidentHalfEdge = i*3 + j;
        }
    }
}

void HalfEdgeMesh::constructInteriorHalfEdgesFromFaces(std::vector<int> &faces) {
    using _edge = std::pair<int,int>;
    size_t n_faces = polygons.size();
    //std::cout << "0. aca "<< std::endl;	
    auto hash_for_pair = [n = 3* n_faces](const std::pair<int, int>& p) {
        return std::hash<int>{}(p.first)*n + std::hash<int>{}(p.second);
    };
    std::unordered_map<_edge, int, decltype(hash_for_pair)> map_edges(3* n_faces, hash_for_pair); //set of edges to calculate the boundary and twin edges
    //std::cout << "1. aca "<< std::endl;
    for(std::size_t i = 0; i < n_faces; i++){
        for(std::size_t j = 0; j < 3; j++){
            HalfEdge he;
            int v_origin = faces.at(3*i+j);
            int v_target = faces.at(3*i+(j+1)%3);
            he.origin = v_origin;
            he.next = i*3+(j+1)%3;
            he.prev = i*3+(j+2)%3;
            he.isBorder = false;
            he.twin = -1;
            vertices.at(v_origin).incidentHalfEdge = i*3+j;
            map_edges[std::make_pair(v_origin, v_target)] = i*3+j;
            halfEdges.push_back(he);
        }
        //std::cout << i << std::endl;
    }
    //std::cout << "2. aca "<< std::endl;	
    
    //Calculate twin halfedge and boundary halfedges from set_edges
    std::unordered_map<_edge,int, decltype(hash_for_pair)>::iterator it;
    for(std::size_t i = 0; i < halfEdges.size(); i++){
        //if halfedge has no twin
        if(halfEdges.at(i).twin == -1){
            int edgeTarget = origin(next(i));
            int edgeOrigin = origin(i);
            _edge twin = std::make_pair(edgeTarget, edgeOrigin);
            it=map_edges.find(twin);
            //if twin is found
            if(it!=map_edges.end()){
                int index_twin = it->second;
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
        int nxtCCW, prvCCW;
        for(std::size_t i = n_halfedges; i < halfEdges.size(); i++){
            if(halfEdges.at(i).isBorder){
                nxtCCW = CCWEdgeToVertex(halfEdges.at(i).twin);
                while (halfEdges.at(nxtCCW).isBorder != true)
                    nxtCCW = this->CCWEdgeToVertex(nxtCCW);
                halfEdges.at(i).next = nxtCCW;

                prvCCW = this->next(twin(i));
                while (halfEdges.at(halfEdges.at(prvCCW).twin).isBorder != true)
                    prvCCW = this->CWEdgeToVertex(prvCCW);
                halfEdges.at(i).prev = halfEdges.at(prvCCW).twin;
            }
        }
}