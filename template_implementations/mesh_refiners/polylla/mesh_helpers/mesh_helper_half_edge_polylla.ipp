#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_half_edge_polylla.hpp>
#include "mesh_helper_half_edge_polylla.hpp"
#endif 

namespace refiners::helpers::polylla {
    void MeshHelper<HalfEdgeMesh>::labelMaxEdges(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh) {
        auto t_start = std::chrono::high_resolution_clock::now();
        for(int i = 0; i < mesh->numberOfPolygons(); ++i) {
            refiner.maxEdges[findMaxEdge(refiner,mesh,i)] = true;
        }
        auto t_end = std::chrono::high_resolution_clock::now();
        refiner.timeStats[T_LABEL_MAX_EDGES] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    }

    HalfEdgeMesh::EdgeIndex MeshHelper<HalfEdgeMesh>::findMaxEdge(PolyllaRefiner<HalfEdgeMesh>& refiner, HalfEdgeMesh* mesh, FaceIndex face) {
        EdgeIndex incidentHalfEdge = mesh->getPolygon(face);
        EdgeIndex nextEdge = mesh->next(incidentHalfEdge);
        EdgeIndex prevEdge = mesh->prev(incidentHalfEdge);
        double dist0 = mesh->edgeLength2(incidentHalfEdge);
        double dist1 = mesh->edgeLength2(nextEdge);
        double dist2 = mesh->edgeLength2(prevEdge);
        double maxLength = std::max({dist0, dist1, dist2});
        //Find the longest edge of the triangle
        if(maxLength == dist0) {
            return incidentHalfEdge;
        } else if(maxLength == dist1) {
            return nextEdge;
        } else {
            return prevEdge;
        }
    }
    void MeshHelper<HalfEdgeMesh>::labelFrontierEdges(PolyllaRefiner<HalfEdgeMesh> &refiner, HalfEdgeMesh *mesh) {
        auto t_start = std::chrono::high_resolution_clock::now();
        for (EdgeIndex edge = 0; edge < mesh->numberOfEdges(); ++edge){
            if(isFrontierEdge(refiner,mesh,edge)){
                refiner.frontierEdges[edge] = true;
                ++refiner.meshStats[N_FRONTIER_EDGES];
            }
        }

        auto t_end = std::chrono::high_resolution_clock::now();
        refiner.timeStats[T_LABEL_FRONTIER_EDGES] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    }

    bool MeshHelper<HalfEdgeMesh>::isFrontierEdge(PolyllaRefiner<HalfEdgeMesh> &refiner, HalfEdgeMesh *mesh, EdgeIndex edge) {
        EdgeIndex twin = mesh->twin(edge);
        bool isBorderEdge = mesh->isBorderEdge(edge) || mesh->isBorderEdge(twin);
        bool isMaxEdge = refiner.maxEdges[edge] || refiner.maxEdges[twin];
        return isBorderEdge || !isMaxEdge;
    }
}
