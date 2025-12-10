from svg_steps_utils import *

svg = svg_header(1000, 1130)
svg += f'<g transform="translate(-800,1800) scale(10,-10)">\n'
tri_idx = 1
svg += svg_triangle_edges(
            vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]], vertices[tris[tri_idx][2]],
            edge_colors=[colors[tri_idx], "gray", colors[tri_idx]],
            dashed=[False, True, False]
        )
tri_idx = 0
svg += svg_triangle_with_circumcenter(
        vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]], vertices[tris[tri_idx][2]],
        edge_colors=[colors[tri_idx], "gray", "gray"],
        center_color=colors[tri_idx],
        dashed=[False, True, True]
    )
tri_idx = 2
svg += svg_triangle_edges(
        vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]], vertices[tris[tri_idx][2]],
        edge_colors=[colors[tri_idx], colors[tri_idx], "gray"],
        dashed=[False, False, True]
    )
svg += "</g>\n"
svg += svg_footer()

print(svg)
