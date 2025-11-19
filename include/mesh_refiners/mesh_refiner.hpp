#ifndef MESH_REFINER_HPP
#define MESH_REFINER_HPP

#include <concepts/mesh_data.hpp>
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <misc/memory_stat.hpp>
#include <cstdint>
template <MeshData Mesh>
class MeshRefiner {
    public:
        /**
         * Refines the `inputMesh` according to this specific refiner's algorithm
         * @param inputMesh A mesh to be refined
         * @return A refined mesh
         */
        virtual Mesh* refineMesh(const Mesh* inputMesh) = 0;
        /**
         * @return A vector of output polygons to write after refinement
         */
        virtual std::vector<typename Mesh::OutputIndex>& getOutputSeeds() = 0;
        /**
         * @return A vector of output polygons in an intermediate step in the refinement process before post processing is made
         */
        virtual std::vector<typename Mesh::OutputIndex>& getOutputSeedsBeforePostProcess() = 0;
        /**
         * @return A refined mesh before post processing
         */
        virtual Mesh* getMeshBeforePostProcess() = 0;
        /**
         * @return A hash map of mesh stats of the resulting mesh
         */
        virtual std::unordered_map<MeshStat,int>& getRefinementStats() = 0;
        /**
         * @return A hash map of elapsed time on each step of the refinement process
         */
        virtual std::unordered_map<TimeStat,double>& getRefinementTimes() = 0;
        /**
         * @return A hash map of memory usage from a run of the refiner
         */
        virtual std::unordered_map<MemoryStat, unsigned long long>& getRefinementMemory() = 0;


        virtual ~MeshRefiner() = default;
};

#endif