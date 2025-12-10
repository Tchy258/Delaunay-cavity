from svg_steps_utils import *

svg = svg_header(1000, 1130)
svg += f'<g transform="translate(-800,1800) scale(10,-10)">\n'
i = 0
for a,b,c in tris:
    svg += svg_triangle_edges(
        vertices[a], vertices[b], vertices[c],
        #edge_colors=[colors[i], colors[i], colors[i]],
        dashed=[False, False, False]
    )
    i+=1
svg += "</g>\n"
svg += svg_footer()

print(svg)
