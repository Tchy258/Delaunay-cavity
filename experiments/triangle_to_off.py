#!/usr/bin/env python3
import sys

def read_node(filename):
    with open(filename, "r") as f:
        lines = [l.strip() for l in f if l.strip() and not l.startswith("#")]

    header = lines[0].split()
    npts = int(header[0])
    dim = int(header[1])
    assert dim == 2 or dim == 3, "Only 2D or 3D node files supported."

    nodes = {}
    for line in lines[1:]:
        parts = line.split()
        idx = int(parts[0])
        coords = list(map(float, parts[1:1+dim]))

        # Pad 2D → 3D for OFF
        if dim == 2:
            coords.append(0.0)

        nodes[idx] = coords

    return nodes


def read_ele(filename):
    with open(filename, "r") as f:
        lines = [l.strip() for l in f if l.strip() and not l.startswith("#")]

    header = lines[0].split()
    ntri   = int(header[0])
    nnode  = int(header[1])
    assert nnode == 3, "Only triangular .ele files supported for OFF output."

    tris = []
    for line in lines[1:]:
        parts = line.split()
        idx = int(parts[0])
        tri_nodes = list(map(int, parts[1:4]))
        tris.append(tri_nodes)

    return tris


def write_off(filename, nodes, triangles):
    # OFF format uses 0-based indexing
    # Triangle uses 1-based user indexing, so we must remap.

    # Consistent remapping:
    index_map = {}
    sorted_keys = sorted(nodes.keys())
    for new_i, old_i in enumerate(sorted_keys):
        index_map[old_i] = new_i

    with open(filename, "w") as f:
        f.write("OFF\n")
        f.write(f"{len(nodes)} {len(triangles)} 0\n")

        # Write vertices
        for k in sorted_keys:
            x, y, z = nodes[k]
            f.write(f"{x} {y} {z}\n")

        # Write faces
        for tri in triangles:
            a, b, c = (index_map[i] for i in tri)
            f.write(f"3 {a} {b} {c}\n")

    print(f"Written: {filename}")


def main():
    if len(sys.argv) != 3:
        print("Usage: python triangle_to_off.py mesh.node mesh.ele")
        sys.exit(1)

    node_file = sys.argv[1]
    ele_file  = sys.argv[2]

    base = node_file.rsplit(".", 1)[0]
    off_file = base + ".off"

    nodes = read_node(node_file)
    triangles = read_ele(ele_file)
    write_off(off_file, nodes, triangles)


if __name__ == "__main__":
    main()
