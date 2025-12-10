#!/usr/bin/env python3
import sys

def read_ele(filename):
    with open(filename, "r") as f:
        lines = [line.strip() for line in f if line.strip() and not line.startswith("#")]
    header = lines[0].split()
    num_tri = int(header[0])
    nodes_per_tri = int(header[1])

    tris = []
    for line in lines[1:]:
        parts = line.split()
        # Format: <id> n1 n2 n3 ...
        nodes = list(map(int, parts[1:1+nodes_per_tri]))
        tris.append(nodes)
    return tris

def write_face(filename, faces):
    with open(filename, "w") as f:
        f.write(f"{len(faces)} 1\n")  # boundary markers = 1
        for i, (n1, n2) in enumerate(faces, start=1):
            f.write(f"{i} {n1} {n2} 0\n")  # marker=0 (internal)
    print(f"Written: {filename}")

def main():
    if len(sys.argv) != 2:
        print("Usage: python ele_to_face.py input.ele")
        sys.exit(1)

    ele_file = sys.argv[1]
    base = ele_file.rsplit(".", 1)[0]
    face_file = base + ".face"

    tris = read_ele(ele_file)

    # Collect unique undirected edges
    edges = set()
    for tri in tris:
        n1, n2, n3 = tri[:3]
        tri_edges = [
            tuple(sorted((n1, n2))),
            tuple(sorted((n2, n3))),
            tuple(sorted((n3, n1))),
        ]
        for e in tri_edges:
            edges.add(e)

    # Sort edges by first then second node
    faces = sorted(edges)

    write_face(face_file, faces)

if __name__ == "__main__":
    main()
