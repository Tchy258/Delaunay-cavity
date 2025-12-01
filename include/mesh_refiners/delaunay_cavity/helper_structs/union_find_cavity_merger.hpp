#ifndef UNION_FIND_CAVITY_MERGER_HPP
#define UNION_FIND_CAVITY_MERGER_HPP
#include <concepts/mesh_data.hpp>
#include <numeric>
namespace refiners::helpers::delaunay_cavity {
    template <MeshData Mesh>
    class UnionFindCavityMerger {
        private:
            using OutputIndex = typename Mesh::OutputIndex;
            std::vector<OutputIndex> parent;

        public:
            UnionFindCavityMerger(size_t n) : parent(n, Mesh::invalidIndexValue) {}

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