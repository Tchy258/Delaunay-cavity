from svg_steps_utils import *

svg = svg_header(1000, 1130)
svg += f'<g transform="translate(-800,1800) scale(10,-10)">\n'
i = 0
for a,b,c in tris:
    if i > 0:
        svg += svg_triangle_edges(
            vertices[a], vertices[b], vertices[c],
            #edge_colors=[colors[i], colors[i], colors[i]],
            dashed=[False, False, False]
        )
    else:
        pass
    i+=1
i = 0
svg += svg_triangle_with_circumcircle(
        vertices[tris[0][0]], vertices[tris[0][1]], vertices[tris[0][2]],
        edge_colors=[colors[i], colors[i], colors[i]],
        circle_color=ccolors[i],
        center_color=colors[i],
        dashed=[False, False, False]
    )
svg += "</g>\n"
svg += svg_footer()

print(svg)
