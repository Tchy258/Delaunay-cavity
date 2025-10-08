#ifndef MESH_STAT_HPP
#define MESH_STAT_HPP

enum MeshStat {
    N_POLYGONS,
    N_FRONTIER_EDGES,
    N_BARRIER_EDGE_TIPS,
    N_POLYGONS_TO_REPAIR,
    N_POLYGONS_ADDED_AFTER_REPAIR,
    N_VERTICES,
    N_EDGES,
    N_BORDER_EDGES
};

inline constexpr unsigned int meshStatAmount = 8;

inline constexpr const char* MeshStatNames[meshStatAmount] = {
    "n_polygons",
    "n_frontier_edges",
    "n_barrier_edge_tips",
    "n_polygons_to_repair",
    "n_polygons_added_after_repair",
    "n_vertices",
    "n_edges",
    "n_border_edges"
};

#endif