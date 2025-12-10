import math
from typing import Tuple, List, Optional

Point = Tuple[float, float]


# --------------------------
# Geometry utilities
# --------------------------

def circumcenter(A: Point, B: Point, C: Point) -> Point:
    """Return the circumcenter (Ux, Uy) of triangle ABC."""
    (x1, y1), (x2, y2), (x3, y3) = A, B, C
    
    d = 2 * (
        x1 * (y2 - y3)
        + x2 * (y3 - y1)
        + x3 * (y1 - y2)
    )

    if d == 0:
        raise ValueError("Points are collinear; circumcenter undefined.")

    ux = (
        (x1**2 + y1**2) * (y2 - y3)
        + (x2**2 + y2**2) * (y3 - y1)
        + (x3**2 + y3**2) * (y1 - y2)
    ) / d

    uy = (
        (x1**2 + y1**2) * (x3 - x2)
        + (x2**2 + y2**2) * (x1 - x3)
        + (x3**2 + y3**2) * (x2 - x1)
    ) / d

    return ux, uy


def distance(p: Point, q: Point) -> float:
    """Euclidean distance between p and q."""
    return math.hypot(p[0] - q[0], p[1] - q[1])


# --------------------------
# SVG primitives
# --------------------------

def svg_header(width: int, height: int) -> str:
    return f'<svg width="{width}" height="{height}" xmlns="http://www.w3.org/2000/svg">\n'

def svg_footer() -> str:
    return "</svg>"

def svg_line(p1: Point, p2: Point,
             color: str = "black",
             width: float = 1.0,
             dashed: bool = False) -> str:
    dash = 'stroke-dasharray="5,5"' if dashed else ""
    return (f'<line x1="{p1[0]}" y1="{p1[1]}" '
            f'x2="{p2[0]}" y2="{p2[1]}" '
            f'stroke="{color}" stroke-width="{width}" {dash} />\n')

def svg_circle(center: Point, radius: float,
               stroke: str = "blue",
               width: float = 1.5,
               fill: str = "none") -> str:
    return (f'<circle cx="{center[0]}" cy="{center[1]}" '
            f'r="{radius}" stroke="{stroke}" stroke-width="{width}" fill="{fill}" />\n')

def svg_point(p: Point, color: str = "red", r: float = 3.0) -> str:
    return f'<circle cx="{p[0]}" cy="{p[1]}" r="{r}" fill="{color}" />\n'


# --------------------------
# Triangle drawing utilities
# --------------------------

def svg_triangle_edges(
    A: Point, B: Point, C: Point,
    edge_colors: Optional[List[str]] = None,
    dashed: Optional[List[bool]] = None,
    width: float = 1.5
) -> str:
    """
    Draws the edges of a triangle with individual edge color & dashed control.
    edge order: AB, BC, CA
    """
    if edge_colors is None:
        edge_colors = ["black", "black", "black"]
    if dashed is None:
        dashed = [False, False, False]

    (cAB, cBC, cCA) = edge_colors
    (dAB, dBC, dCA) = dashed

    svg = ""
    svg += svg_line(A, B, color=cAB, width=width, dashed=dAB)
    svg += svg_line(B, C, color=cBC, width=width, dashed=dBC)
    svg += svg_line(C, A, color=cCA, width=width, dashed=dCA)
    return svg


def svg_circumcircle_and_center(A: Point, B: Point, C: Point,
                                circle_color: str = "blue",
                                center_color: str = "red") -> str:
    """Draw circumcircle + circumcenter."""
    O = circumcenter(A, B, C)
    r = distance(O, A)

    return (
        svg_circle(O, r, stroke=circle_color) +
        svg_point(O, color=center_color, r=4)
    )


# --------------------------
# Composite helper
# --------------------------

def svg_triangle_with_circumcircle(
    A: Point, B: Point, C: Point,
    edge_colors: Optional[List[str]] = None,
    dashed: Optional[List[bool]] = None,
    circle_color: str = "blue",
    center_color: str = "red"
) -> str:
    """Convenience function: edges + circumcircle + center."""
    svg = ""
    svg += svg_triangle_edges(A, B, C, edge_colors, dashed)
    svg += svg_circumcircle_and_center(A, B, C,circle_color=circle_color, center_color=center_color)
    return svg

def svg_triangle_with_circumcenter(
        A: Point, B: Point, C: Point,
    edge_colors: Optional[List[str]] = None,
    dashed: Optional[List[bool]] = None,
    center_color: str = "red"
) -> str:
    """Convenience function: edges + circumcircle + center."""
    O = circumcenter(A, B, C)
    svg = ""
    svg += svg_triangle_edges(A, B, C, edge_colors, dashed)
    svg += svg_point(O, color=center_color, r=4)
    return svg


vertices = [
    (100,100),
    (154.494148060321668, 122.044062204069669),
    (100,200),
    (163.942679845788371,100),
    (200,133.659454511262675),
    (122.321073814882275, 173.647121416401242),
    (100,121.863797480360336),
    (200,200),
    (169.813939498822683, 134.025051651799185),
    (200,100)
]

tris = [
    (6,0,1),
    (5,6,1),
    (0,3,1)
]

colors = ["red","green","blue"]
ccolors = ["#ff8f00","#7000ff","#6afff2"]