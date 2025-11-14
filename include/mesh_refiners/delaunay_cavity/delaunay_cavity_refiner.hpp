#ifndef DELAUNAY_CAVITY_REFINER_HPP
#define DELAUNAY_CAVITY_REFINER_HPP
#include <unordered_map>
#include <unordered_set>
#include <cstdint>
#include <queue>
#include <array>
#include <misc/mesh_stat.hpp>
#include <misc/time_stat.hpp>
#include <concepts/mesh_data.hpp>
#include <concepts/cavity_merging_strategy.hpp>
#include <concepts/refinement_criterion.hpp>
#include <concepts/triangle_comparator.hpp>
#include <concepts/is_half_edge_vertex.hpp>
#include <mesh_data/structures/vertex.hpp>
#include <mesh_data/half_edge_mesh.hpp>
#include <mesh_refiners/mesh_refiner.hpp>
#include <mesh_refiners/delaunay_cavity/mesh_helpers/mesh_helper_delaunay_cavity.hpp>
#include <mesh_refiners/delaunay_cavity/helper_structs/cavity.hpp>
#include <mesh_refiners/refinement_criteria/null_refinement_criterion.hpp>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <cmath>

#define DELAUNAY_CAVITY_REFINER_TEMPLATE \
template < \
    MeshData MeshType, \
    RefinementCriterion<MeshType> Criterion, \
    TriangleComparator<MeshType> Comparator, \
    CavityMergingStrategy<MeshType> MergingStrategy \
>

DELAUNAY_CAVITY_REFINER_TEMPLATE
class DelaunayCavityRefiner : public MeshRefiner<MeshType> {
    public:
        using MeshVertex = typename MeshType::VertexType;
        using MeshEdge = typename MeshType::EdgeType;
        using VertexIndex = typename MeshType::VertexIndex;
        using EdgeIndex = typename MeshType::EdgeIndex;
        using FaceIndex = typename MeshType::FaceIndex;
        using OutputIndex = typename MeshType::OutputIndex;
    private:
        bool storeMeshBeforePostProcess = false;

        MeshType* meshBeforePostProcess = nullptr;
        
        struct _empty {};
        using maybe_output_vector = std::conditional_t<HasPostInsertionMethod<MergingStrategy,MeshType>, std::vector<OutputIndex>, _empty>;
        [[no_unique_address]] maybe_output_vector outputSeedsBeforePostProcess = maybe_output_vector{};

        Criterion refinementCriterion;
        std::vector<OutputIndex> outputSeeds;
        std::vector<uint8_t> inCavity;
        using _MeshHelper = refiners::helpers::delaunay_cavity::MeshHelper<MeshType>;
        using Cavity = refiners::helpers::delaunay_cavity::Cavity<MeshType>;
        std::unordered_map<MeshStat, int> meshStats;
        std::unordered_map<TimeStat, double> timeStats;

        /**
         * Sorts the triangles before computing the cavities using the provided `TriangleComparator` template type
         * 
         * Note that this method could also shuffle the triangles or do a noop depending on the comparator
         * @param outputMesh The mesh whose triangles will be sorted
         * @return A vector of `FaceIndex` with the indices of the triangles sorted according to the `TriangleComparator`
         */
        std::vector<FaceIndex> sortTriangles(MeshType* outputMesh);
        /**
         * Computes the circumcenter of all of the mesh's triangles with their corresponding `FaceIndex`
         * @param outputMesh The mesh whose triangles will have their circumcenters computed
         * @param sortedTriangles A vector of triangle indices in `outputMesh` resulting from a @ref `sortTriangles` call
         * @return A vector with <Circumcenter,TriangleIndex> pairs
         */
        std::vector<std::pair<MeshVertex,FaceIndex>> computeCircumcenters(MeshType* outputMesh, std::vector<FaceIndex> sortedTriangles);
        /**
         * Computes the cavities for the mesh given a vector of circumcenter,triangle pairs and a vector to check if a triangle has been visited or not
         * 
         * To compute the cavities, a BFS search is done starting from a circumcenter, the visited vector is created once and reused accross searches
         * 
         * @param inputMesh The mesh for which to compute the cavities
         * @param circumcenters A vector of Circumcenter,FaceIndex pairs with the coordinates of the circumcenter for each triangle
         * @param visited A vector of 8-bit unsigned integers used as a yes or no check. 8-bit integers are preferred over a vector of booleans for
         * memory aligment and performance reasons
         * @return A vector of `Cavity` objects with the required information to insert a cavity into the mesh.
         */
        std::vector<Cavity> computeCavities(const MeshType* inputMesh, const std::vector<std::pair<MeshVertex,FaceIndex>>& circumcenters, std::vector<uint8_t>& visited);

        /**
         * Resets the BFS `visited` vector to perform a new search starting from another circumcenter
         * @param visited A "boolean" vector of unsigned integers
         * @param cavity A cavity object that was created within a call of `computeCavities`
         */
        inline void resetVisited(std::vector<uint8_t>& visited, const Cavity& cavity) {
            for (FaceIndex triangle: cavity.allTriangles) {
                visited[triangle] = 0;
            }
        }
        
    public:
        MeshType* refineMesh(const MeshType* inputMesh) override;
        std::vector<OutputIndex>& getOutputSeeds() override {
            return outputSeeds;
        }
        explicit DelaunayCavityRefiner(Criterion criterion, bool storeBeforePostProcess = false) : refinementCriterion(std::move(criterion)), storeMeshBeforePostProcess(storeBeforePostProcess) {
            meshStats[N_POLYGONS] = 0;
            meshStats[N_VERTICES] = 0;
            meshStats[N_EDGES] = 0;
            
            timeStats[T_TRIANGULATION_GENERATION] = 0.0;
            timeStats[T_TRIANGLE_SORTING] = 0.0;
            timeStats[T_CIRCUMCENTER_COMPUTATION] = 0.0;
            timeStats[T_CAVITY_COMPUTATION] = 0.0;
            timeStats[T_CAVITY_INSERTION] = 0.0;
            timeStats[T_CAVITY_MERGING] = 0.0;
        }

        explicit DelaunayCavityRefiner(bool storeBeforePostProcess = false) requires std::same_as<Criterion, NullRefinementCriterion<MeshType>>
        : DelaunayCavityRefiner(NullRefinementCriterion<MeshType>(), storeBeforePostProcess) {}


        std::unordered_map<MeshStat,int>& getRefinementStats() override {
            return meshStats;
        }
        std::unordered_map<TimeStat,double>& getRefinementTimes() override {
            return timeStats;
        }

        std::vector<OutputIndex>& getOutputSeedsBeforePostProcess() override {
            if constexpr (HasPostInsertionMethod<MergingStrategy,MeshType>) {
                return outputSeedsBeforePostProcess;
            } else {
                return outputSeeds;
            }
        }
        MeshType* getMeshBeforePostProcess() override {
            if constexpr (HasPostInsertionMethod<MergingStrategy,MeshType>) {
                return meshBeforePostProcess;
            } else {
                return nullptr;
            }
        }

        ~DelaunayCavityRefiner() {
            delete meshBeforePostProcess;
        }
};

#include<mesh_refiners/delaunay_cavity/delaunay_cavity_refiner.ipp>


#undef DELAUNAY_CAVITY_REFINER_TEMPLATE
#endif