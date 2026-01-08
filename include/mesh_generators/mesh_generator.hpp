#ifndef MESH_GENERATOR_HPP
#define MESH_GENERATOR_HPP

#include <concepts/mesh_data.hpp>
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <misc/memory_stat.hpp>
#include <cstdint>
template <MeshData Mesh>
class MeshGenerator {
    public:
        /**
         * Refines the `inputMesh` according to this specific generator's algorithm
         * @param inputMesh A mesh to be refined
         * @return A refined mesh
         */
        virtual Mesh* generateMesh(const Mesh* inputMesh) = 0;
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
        virtual const std::unordered_map<MeshStat,int>& getGenerationStats() = 0;
        /**
         * @return A hash map of elapsed time on each step of the refinement process
         */
        virtual const std::unordered_map<TimeStat,double>& getGenerationTimes() = 0;
        /**
         * @return A hash map of memory usage from a run of the generator
         */
        virtual const std::unordered_map<MemoryStat, unsigned long long>& getGenerationMemory() = 0;


        virtual ~MeshGenerator() = default;
};

#endif