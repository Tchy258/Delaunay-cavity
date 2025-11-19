#ifndef MEMORY_STAT_HPP
#define MEMORY_STAT_HPP

enum MemoryStat {
    M_TOTAL,
    M_MAX_EDGES,
    M_FRONTIER_EDGES,
    M_SEED_EDGES,
    M_SEED_BOUNDARY_EDGE_TIP_MARK,
    M_TRIANGLE_LIST,
    M_CAVITY_ARRAY,
    M_VISITED_ARRAY,
    M_EDGE_MAP,
    M_EDGES_INPUT,
    M_EDGES_OUTPUT,
    M_VERTICES_INPUT,
    M_VERTICES_OUTPUT,
};

inline constexpr unsigned int memoryStatAmount = 13;

inline constexpr const char* MemoryStatNames[memoryStatAmount] = {
    "memory_total",
    "memory_max_edges",
    "memory_frontier_edges",
    "memory_seed_edges",
    "memory_seed_boundary_edge_tip_mark",
    "memory_triangle_list",
    "memory_cavity_array",
    "memory_visited_array",
    "memory_edge_map",
    "memory_edges_input",
    "memory_edges_output",
    "memory_vertices_input",
    "memory_vertices_output"
};

#endif