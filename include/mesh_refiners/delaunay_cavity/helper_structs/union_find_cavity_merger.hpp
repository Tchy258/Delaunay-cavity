#ifndef UNION_FIND_CAVITY_MERGER_HPP
#define UNION_FIND_CAVITY_MERGER_HPP
#include <concepts/mesh_data.hpp>
#include <numeric>
namespace refiners::helpers::delaunay_cavity {
    /**
     * A union-find like class that merges polygon representatives.
     * 
     * Unlike traditional union-find, the union is directed and does not
     * rely on size or other metric, it's purely decided by the caller
     */
    template <MeshData Mesh>
    class UnionFindCavityMerger {
        private:
            using OutputIndex = typename Mesh::OutputIndex;
            std::vector<OutputIndex> parent;

        public:
            UnionFindCavityMerger(size_t n) : parent(n, Mesh::invalidIndexValue) {}

            /**
             * @param outputIdentifier An output index that may or may not represent a polygon
             * @return The representative index of `outputIdentifier`, or itself
             */
            OutputIndex find(OutputIndex outputIdentifier) {
                OutputIndex representative = outputIdentifier;
                while (parent[representative] != representative) {
                    representative = parent[representative];
                }
                OutputIndex current = outputIdentifier;
                while (parent[current] != current) {
                    OutputIndex next = parent[current];
                    parent[current] = representative;
                    current = next;
                }
                return representative;
            }

            /**
             * Makes `newRepresentative` the representative of `targetIndex`
             */
            void unite(OutputIndex targetIndex, OutputIndex newRepresentative) {
                parent[targetIndex] = newRepresentative;
            }

            bool isRepresentative(OutputIndex queriedIndex) const noexcept {
                return parent[queriedIndex] == queriedIndex;
            }

            unsigned long long memoryUsage() const {
                return sizeof(decltype(parent.back())) * parent.capacity();
            }
    };
}


#endif