from svg_steps_utils import *

svg = svg_header(1000, 1130)
svg += f'<g transform="translate(-800,1800) scale(10,-10)">\n'
tri_idx = 0
svg += svg_line(vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]])
tri_idx = 1
svg += svg_line(vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]])
svg += svg_line(vertices[tris[tri_idx][2]], vertices[tris[tri_idx][0]])
tri_idx = 2
svg += svg_line(vertices[tris[tri_idx][0]], vertices[tris[tri_idx][1]])
svg += svg_line(vertices[tris[tri_idx][1]], vertices[tris[tri_idx][2]])
svg += "</g>\n"
svg += svg_footer()

print(svg)
