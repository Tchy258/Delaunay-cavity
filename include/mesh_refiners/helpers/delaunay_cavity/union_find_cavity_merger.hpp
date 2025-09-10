#ifndef UNION_FIND_CAVITY_MERGER_HPP
#define UNION_FIND_CAVITY_MERGER_HPP
#include <concepts/mesh_data.hpp>
#include <numeric>
namespace refiners::helpers::delaunay_cavity {
    template <MeshData Mesh>
    class UnionFindCavityMerger {
        private:
            using FaceIndex = typename Mesh::FaceIndex;
            std::vector<FaceIndex> parent;
            std::vector<int> sizeOfSet;

        public:
            UnionFindCavityMerger(size_t n) : parent(n), sizeOfSet(n, 1) {
                std::iota(parent.begin(), parent.end(), 0);
            }

            FaceIndex find(FaceIndex face) {
                if (parent[face] != face) {
                    parent[face] = find(parent[face]);
                }
                return parent[face];
            }

            void unite(FaceIndex first, FaceIndex second) {
                FaceIndex rootFirst = find(first);
                FaceIndex rootSecond = find(second);

                if (rootFirst != rootSecond) {
                        
                    if (sizeOfSet[rootFirst] >= sizeOfSet[rootSecond]) {
                        parent[rootSecond] = rootFirst;
                        sizeOfSet[rootFirst] += sizeOfSet[rootSecond];
                    } else {
                        parent[rootFirst] = rootSecond;
                        sizeOfSet[rootSecond] += sizeOfSet[rootFirst];
                    }
                }
            }
    };
}


#endif