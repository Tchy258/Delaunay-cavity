#ifndef MESH_REFINEMENT_STRATEGY_HPP
#define MESH_REFINEMENT_STRATEGY_HPP
#include <string>

class PolygonalMesh;

class MeshRefinementStrategy {
    public:
        virtual ~MeshRefinementStrategy() = default;
        virtual void updateMesh(PolygonalMesh &mesh) = 0;
        virtual void writeStatsToFile(PolygonalMesh &mesh, std::string filename) = 0;
        virtual void writeALEFile(PolygonalMesh &mesh, std::string filename) = 0;
        virtual void writeOFFFile(PolygonalMesh &mesh, std::string filename) = 0;
};

#endif