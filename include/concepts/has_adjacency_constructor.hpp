#ifndef HAS_ADJACENCY_CONSTRUCTOR_HPP
#define HAS_ADJACENCY_CONSTRUCTOR_HPP
#include<concepts>
#include<vector>

template<typename Mesh>
concept HasAdjacencyConstructor =
    std::constructible_from<Mesh,
        std::vector<typename Mesh::VertexType>,
        std::vector<typename Mesh::EdgeType>,
        std::vector<int>, 
        std::vector<int>>;

#endif