#ifndef MESH_HELPER_HALF_EDGE_POLYLLA_HPP
#include <mesh_refiners/polylla/mesh_helpers/mesh_helper_half_edge_polylla.hpp>
#endif

namespace refiners::helpers::polylla {
    void MeshHelper<HalfEdgeMesh>::labelMaxEdges(RefinementData& data, HalfEdgeMesh* mesh) {
        for(FaceIndex face = 0; face < mesh->numberOfPolygons(); ++face) {
            data.maxEdges[findMaxEdge(mesh,face)] = true;
        }
    }

    HalfEdgeMesh::EdgeIndex MeshHelper<HalfEdgeMesh>::findMaxEdge(HalfEdgeMesh* mesh, FaceIndex face) {
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
    void MeshHelper<HalfEdgeMesh>::labelFrontierEdges(RefinementData& data, HalfEdgeMesh *mesh) {
        for (EdgeIndex edge = 0; edge < mesh->numberOfEdges(); ++edge){
            if(isFrontierEdge(data, mesh, edge)){
                data.frontierEdges[edge] = true;
                ++data.meshStats[N_FRONTIER_EDGES];
            }
        }
    }

    bool MeshHelper<HalfEdgeMesh>::isFrontierEdge(RefinementData& data, HalfEdgeMesh *mesh, EdgeIndex edge) {
        EdgeIndex twin = mesh->twin(edge);
        bool isBorderEdge = mesh->isBorderEdge(edge) || mesh->isBorderEdge(twin);
        bool isMaxEdge = data.maxEdges[edge] || data.maxEdges[twin];
        return isBorderEdge || !isMaxEdge;
    }
    std::vector<HalfEdgeMesh::OutputIndex> MeshHelper<HalfEdgeMesh>::generateSeedCandidates(RefinementData& data, HalfEdgeMesh *mesh) {
        std::vector<OutputIndex> seedCandidates;
        for (EdgeIndex edge = 0; edge < mesh->numberOfEdges(); ++edge) {
            if(!mesh->isBorderEdge(edge) && isSeedCandidateIndex(data,mesh,edge)) {
                seedCandidates.push_back(edge);
            }
        }
        return seedCandidates;
    }
    bool MeshHelper<HalfEdgeMesh>::isSeedCandidateIndex(RefinementData& data, HalfEdgeMesh *mesh, OutputIndex seedCandidate) {
        EdgeIndex candidateTwin = mesh->twin(seedCandidate);

        bool isTerminalEdge = (!mesh->isBorderEdge(candidateTwin) && (data.maxEdges[seedCandidate] && data.maxEdges[candidateTwin]));
        bool isTerminalBorderEdge = (mesh->isBorderEdge(candidateTwin) && data.maxEdges[seedCandidate]);

        if((isTerminalEdge && seedCandidate < candidateTwin ) || isTerminalBorderEdge) {
            return true;
        }

        return false;
    }
    std::vector<HalfEdgeMesh::OutputIndex> MeshHelper<HalfEdgeMesh>::generateOutputSeeds(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh) {
        //Travel phase: Generate polygon mesh
        std::vector<OutputIndex> outputSeeds;
        EdgeIndex polygonSeed;
        //Foreach seed edge generate polygon
        auto t_start = std::chrono::high_resolution_clock::now();
        for(EdgeIndex seedCandidate : data.seedCandidates){
            polygonSeed = generatePolygonFromSeed(data,inputMesh, outputMesh,seedCandidate);
            //output_seeds.push_back(polygonSeed);
            if(isSimplePolygon(outputMesh,polygonSeed)){ //If the polygon is a simple polygon then is part of the mesh
                outputSeeds.push_back(polygonSeed);
            }else{ //Else, the polygon is send to reparation phase
                auto t_start_repair = std::chrono::high_resolution_clock::now();
                barrierEdgeTipReparation(data, inputMesh, outputMesh, polygonSeed, outputSeeds);
                auto t_end_repair = std::chrono::high_resolution_clock::now();
                data.timeStats[T_REPAIR] += std::chrono::duration<double, std::milli>(t_end_repair-t_start_repair).count();
            }         
        }    
        auto t_end = std::chrono::high_resolution_clock::now();
        data.timeStats[T_TRAVERSAL_AND_REPAIR] = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        data.timeStats[T_TRAVERSAL] = data.timeStats[T_TRAVERSAL_AND_REPAIR] - data.timeStats[T_REPAIR];

        return outputSeeds;
    }

    HalfEdgeMesh::OutputIndex MeshHelper<HalfEdgeMesh>::generatePolygonFromSeed(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seed) {
        //search next frontier-edge
        EdgeIndex firstEdge = getNextFrontierEdge(data, inputMesh,seed);
        EdgeIndex currentEdge = inputMesh->next(firstEdge);
        EdgeIndex currentFrontierEdge = firstEdge;

        //travel inside frontier-edges of polygon
        do {
            currentEdge = getNextFrontierEdge(data,inputMesh,currentEdge);
            //update next of previous frontier-edge
            outputMesh->setNext(currentFrontierEdge, currentEdge);
            //update prev of current frontier-edge
            outputMesh->setPrev(currentEdge, currentFrontierEdge);

            //travel to next half-edge
            currentFrontierEdge = currentEdge;
            currentEdge = inputMesh->next(currentEdge);
        } while(currentFrontierEdge != firstEdge);

        return firstEdge;
    }
    HalfEdgeMesh::EdgeIndex MeshHelper<HalfEdgeMesh>::getNextFrontierEdge(RefinementData& data, const HalfEdgeMesh *mesh, EdgeIndex edge) {
        EdgeIndex nextEdge = edge;
        while(!data.frontierEdges[nextEdge]) {
            nextEdge = mesh->CWEdgeToVertex(nextEdge);
        }
        return nextEdge;
    }
    bool MeshHelper<HalfEdgeMesh>::isSimplePolygon(HalfEdgeMesh *mesh, OutputIndex seed) {
        EdgeIndex currentEdge = mesh->next(seed);
        //travel inside frontier-edges of polygon
        while(currentEdge != seed){   
            //if the twin of the next halfedge is the current halfedge, then the polygon is not simple
            if( mesh->twin(mesh->next(currentEdge)) == currentEdge)
                return false;
            //travel to next half-edge
            currentEdge = mesh->next(currentEdge);
        }
        return true;
    }
    void MeshHelper<HalfEdgeMesh>::barrierEdgeTipReparation(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex nonSimpleSeed, std::vector<OutputIndex>& currentOutputs) {
        ++data.meshStats[N_POLYGONS_TO_REPAIR];
        
        std::vector<EdgeIndex> triangleList;
        EdgeIndex t1, t2;
        EdgeIndex middleEdge;
        VertexIndex barrierEdgeTipVertex;

        EdgeIndex firstEdge = nonSimpleSeed;
        EdgeIndex currentEdge = outputMesh->next(firstEdge);
        //search by barrier-edge tips
        while(currentEdge != firstEdge){   
            //if the twin of the next halfedge is the current halfedge, then the polygon is not simple
            if(outputMesh->twin(outputMesh->next(currentEdge)) == currentEdge){
                //std::cout<<"e_curr "<<e_curr<<" e_next "<<mesh_output->next(e_curr)<<" next del next "<<mesh_output->next(mesh_output->next(e_curr))<<" twin curr "<<mesh_output->twin(e_curr)<<" twin next "<<mesh_output->twin(mesh_output->next(e_curr))<<std::endl;
                ++data.meshStats[N_BARRIER_EDGE_TIPS];
                data.meshStats[N_FRONTIER_EDGES] += 2;

                //select edge with barrier edge tip
                barrierEdgeTipVertex = outputMesh->target(currentEdge);
                middleEdge = calculateMiddleEdge(data, inputMesh,barrierEdgeTipVertex);

                //middle edge that contains barrierEdgeTipVertex
                t1 = middleEdge;
                t2 = outputMesh->twin(middleEdge);
                
                //edges of middle-edge are labeled as frontier-edge
                data.frontierEdges[t1] = true;
                data.frontierEdges[t2] = true;

                //edges are use as seed edges and saves in a list
                triangleList.push_back(t1);
                triangleList.push_back(t2);

                data.seedBarrierEdgeTipMark[t1] = true;
                data.seedBarrierEdgeTipMark[t2] = true;
            }
                
            //travel to next half-edge
            currentEdge = outputMesh->next(currentEdge);
        }

        EdgeIndex t_curr;
        //generate polygons from seeds,
        //two seeds can generate the same polygon
        //so the bit_vector seed_bet_mark is used to label as false the edges that are already used
        EdgeIndex newPolygonSeed;
        while (!triangleList.empty()){
            t_curr = triangleList.back();
            triangleList.pop_back();
            if(data.seedBarrierEdgeTipMark[t_curr]) {
                ++data.meshStats[N_POLYGONS_ADDED_AFTER_REPAIR];
                
                data.seedBarrierEdgeTipMark[t_curr] = false;
                newPolygonSeed = generateRepairedPolygon(data, inputMesh, outputMesh, t_curr);
                //Store the polygon in the as part of the mesh
                currentOutputs.push_back(newPolygonSeed);
            }
        }

    }
    HalfEdgeMesh::EdgeIndex MeshHelper<HalfEdgeMesh>::calculateMiddleEdge(RefinementData& data, const HalfEdgeMesh* inputMesh, VertexIndex barrierEdgeTipVertex) {
        EdgeIndex frontierEdgeWithBarrierEdgeTip = getNextFrontierEdge(data,inputMesh,inputMesh->edgeOfVertex(barrierEdgeTipVertex));
        unsigned int numberOfInternalEdges = inputMesh->degree(barrierEdgeTipVertex) - 1; //internal-edges incident to v
        unsigned int cwStepsUntilMiddleEdge = (numberOfInternalEdges - 1) / 2;
        EdgeIndex nextCWEdge = inputMesh->CWEdgeToVertex(frontierEdgeWithBarrierEdgeTip);
        //back to traversing the edges of v_barrier edge tip until the middle-edge is selected
        while (cwStepsUntilMiddleEdge != 0){
            nextCWEdge = inputMesh->CWEdgeToVertex(nextCWEdge);
            --cwStepsUntilMiddleEdge;
        }
        return nextCWEdge;
    }
    HalfEdgeMesh::OutputIndex MeshHelper<HalfEdgeMesh>::generateRepairedPolygon(RefinementData& data, const HalfEdgeMesh* inputMesh, HalfEdgeMesh* outputMesh, OutputIndex seedToRepair) {
        EdgeIndex firstEdge = seedToRepair;

        //search next frontier-edge
        while(!data.frontierEdges[firstEdge]){
            firstEdge = inputMesh->CWEdgeToVertex(firstEdge);
            data.seedBarrierEdgeTipMark[firstEdge] = false; 
            //refiner.seedBarrierEdgeTipMark[mesh->twin(firstEdge)] = false;
        }   
        EdgeIndex currentEdge = inputMesh->next(firstEdge);    
        data.seedBarrierEdgeTipMark[currentEdge] = false;
    
        EdgeIndex currentFrontierEdge = firstEdge; 

        //travel inside frontier-edges of polygon
        do {   
            while(!data.frontierEdges[currentEdge]) {
                currentEdge = inputMesh->CWEdgeToVertex(currentEdge);
                data.seedBarrierEdgeTipMark[currentEdge] = false;
                //seedBarrierEdgeTipMark[mesh->twin(e_curr)] = false;
            } 
            //update next of previous frontier-edge
            outputMesh->setNext(currentFrontierEdge, currentEdge);  
            //update prev of current frontier-edge
            outputMesh->setPrev(currentEdge, currentFrontierEdge);

            //travel to next half-edge
            currentFrontierEdge = currentEdge;
            currentEdge = inputMesh->next(currentEdge);
            data.seedBarrierEdgeTipMark[currentEdge] = false;

        } while(currentFrontierEdge != firstEdge);

        return firstEdge;
    }
}
