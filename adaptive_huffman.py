# -*- coding: utf-8 -*-


# Adaptive Huffman Algorithm

Adaptive Huffman Coding (Algorithm M) in Python
# --------------------------------------------------
This is a memory-efficient adaptive Huffman coding algorithm converted from C++.
It maintains a tree of frequency classes instead of individual symbols to reduce memory usage.
The tree evolves as input symbols are processed, adapting to symbol frequencies dynamically.
"""

from collections import deque
from typing import List, Optional,Tuple
import os


# Node class represents a node in the Huffman tree.


class Node:
    def __init__(self, data=None):
        self.occurrences: int = 0                  # Number of times the symbol(s) occurred
        self.weight: int = 0                       # Weight used for balancing (depends on frequency)
        self.leftChild: Optional['Node'] = None    # Left child in the tree
        self.rightChild: Optional['Node'] = None   # Right child in the tree
        self.parent: Optional['Node'] = None       # Pointer to parent node
        self.isLeft: bool = False
        self.data: List[int] = [] if data is None else [data]

"""## i.Huffman Tree Structure"""

# Step 1: Initialize the Huffman tree with a root node

def initialize_tree() -> Node:
    node = Node()
    node.data = []  # NYT node
    node.occurrences = 0
    node.weight = 0
    return node

"""## ii.To find the leaf node"""

# Step 2: Utility to find a leaf containing a specific symbol

from collections import deque


def find_leaf_by_term(root: Node, dict_node: Node, term: int) -> Node:
    q = deque([root])
    while q:
        current = q.popleft()
        if current.data and term in current.data:
            return current
        if current.leftChild:
            q.append(current.leftChild)
        if current.rightChild:
            q.append(current.rightChild)
    return dict_node

"""## iii.To find leaf by frequency count"""

# Step 3: Utility to find leaf by frequency count


def find_leaf_by_occurrences(root: Node, occurrences: int) -> Optional[Node]:
    q = deque([root])
    while q:
        current = q.popleft()
        if current.occurrences == occurrences:
            return current
        if current.leftChild:
            q.append(current.leftChild)
        if current.rightChild:
            q.append(current.rightChild)
    return None


# Get sibling of a node

def get_sibling(node: Node) -> Optional[Node]:
    if not node.parent:
        return None
    return node.parent.rightChild if node.parent.leftChild == node else node.parent.leftChild

# Exchange positions of two nodes in the tree

def exchange_nodes(left_node: Node, right_node: Node):
    if not left_node or not right_node or left_node == right_node:
        return
    left_parent = left_node.parent
    right_parent = right_node.parent
    if not left_parent or not right_parent:
        return
    left_node.parent, right_node.parent = right_parent, left_parent
    if left_parent.leftChild == left_node:
        left_parent.leftChild = right_node
    else:
        left_parent.rightChild = right_node
    if right_parent.leftChild == right_node:
        right_parent.leftChild = left_node
    else:
        right_parent.rightChild = left_node


# To rebal ance the tree, we need to ensure that the weights of the nodes are updated correctly.
def shift_up(root: Node, node: Node):
    curr_node = node
    while curr_node:
        if curr_node.leftChild and curr_node.rightChild:
            curr_node.weight = curr_node.leftChild.weight + curr_node.rightChild.weight
        else:
            curr_node.weight = curr_node.occurrences * len(curr_node.data)
        sibling = get_sibling(curr_node)
        if curr_node.parent and sibling and curr_node.weight > sibling.weight + 1:
            parent_sibling = get_sibling(curr_node.parent)
            if curr_node.parent.parent and parent_sibling and curr_node.weight > parent_sibling.weight:
                exchange_nodes(curr_node, parent_sibling)
                curr_node.parent.leftChild, curr_node.parent.rightChild = curr_node.parent.rightChild, curr_node.parent.leftChild
        curr_node = curr_node.parent

# Connvert an integer to binary representation
def to_binary(n: int) -> List[int]:
    return [int(x) for x in bin(n)[2:]]

def node_prefix(node: Node, root: Node) -> List[int]:
    prefix = []
    curr_node = node
    while curr_node != root:
        prefix.insert(0, 0 if curr_node.isLeft else 1)
        curr_node = curr_node.parent
    return prefix

## Remove a child node from its parent
def remove_child_from_parent(child: Node):
    sibling = get_sibling(child)
    if child.parent:
        if child.parent.leftChild == child:
            child.parent.leftChild = None
        else:
            child.parent.rightChild = None
    if child.parent and child.parent.parent:
        grandparent = child.parent.parent
        if grandparent.leftChild == child.parent:
            grandparent.leftChild = sibling
        else:
            grandparent.rightChild = sibling
        if sibling:
            sibling.parent = grandparent

## Update the tree with a new term
def update_tree(root: Node, dict_node: Node, next_term: int) -> Tuple[Node, Node, List[int]]:
    result_code = []
    p = find_leaf_by_term(root, dict_node, next_term)
    q = find_leaf_by_occurrences(root, p.occurrences + 1)
    if p != dict_node:
        prefix = node_prefix(p, root)
        data_bin = to_binary(len(p.data) - 1) if len(p.data) != 1 else []
        result_code.extend(prefix)
        result_code.extend(data_bin)
    else:
        prefix = node_prefix(p, root)
        data_bin = to_binary(next_term)
        result_code.extend(prefix)
        result_code.extend(data_bin)
    if q:
        if p != dict_node and next_term in p.data:
            p.data.remove(next_term)
            p.weight -= p.occurrences
        q.data.append(next_term)
        q.weight += q.occurrences
        shift_up(root, q)
        if p != dict_node:
            if not p.data:
                remove_child_from_parent(p)
            elif sibling := get_sibling(p):
                shift_up(root, sibling)
    else:
        new_node = Node()
        new_class = Node()
        new_node.leftChild = p
        new_node.rightChild = new_class
        new_node.parent = p.parent
        new_class.parent = new_node
        p.parent = new_node
        p.isLeft = True
        new_class.isLeft = False
        new_class.occurrences = p.occurrences + 1
        new_class.weight = new_class.occurrences
        new_class.data = [next_term]
        if not new_node.parent:
            root = new_node
        else:
            if new_node.parent.leftChild == p:
                new_node.parent.leftChild = new_node
            else:
                new_node.parent.rightChild = new_node
        if p != dict_node and next_term in p.data:
            p.data.remove(next_term)
            p.weight -= p.occurrences
        new_node.weight = new_class.weight + p.weight
        if not p.data and p != dict_node:
            remove_child_from_parent(p)
        elif sibling := get_sibling(p):
            shift_up(root, sibling)
        shift_up(root, new_node)
        dict_node = new_class
    return root, dict_node, result_code

"""## iv.Visualization of the tree"""

# To print full Tree
def print_full_tree(root: Node):
    q = deque([(root, 0)])
    node_id = 0
    node_map = {id(root): 0}
    nodes_info = []
    while q:
        current, curr_id = q.popleft()
        nodes_info.append((curr_id, current.occurrences, current.weight, current.data))
        if current.leftChild:
            node_id += 1
            q.append((current.leftChild, node_id))
            node_map[id(current.leftChild)] = node_id
        if current.rightChild:
            node_id += 1
            q.append((current.rightChild, node_id))
            node_map[id(current.rightChild)] = node_id
    for nid, occ, wt, data in nodes_info:
        print(f"Node {nid}: Occurrences = {occ}, Weight = {wt}, Data = {data}")
    print(f"\nTotal Nodes = {len(nodes_info)}\n")

# To read a bitmap image and convert it to a list of lists
import numpy as np
from PIL import Image


def read_bitmap(path: str) -> List[List[int]]:
    image = Image.open(path).convert("L")
    pixels = np.array(image)
    return pixels.tolist()

# To read a bitmap image and convert it to a list of lists
def run_adaptive_huffman_on_image(image_path: str, output_path: str) -> Tuple[Node, List[int]]:
    data = read_bitmap(image_path)
    tree = initialize_tree()
    dict_node = tree
    code = []
    for row in data:
        for pixel in row:
            tree, dict_node, encoded_bits = update_tree(tree, dict_node, pixel)
            code.extend(encoded_bits)
    with open(output_path, 'w') as f:
        f.write(''.join(map(str, code)))
    print_full_tree(tree)
    input_size = os.path.getsize(image_path) / 1024 / 1024
    encoded_size = len(code) / 8 / 1024
    print(f"File Size: {input_size:.1f} MB")
    print(f"Encoded Size: {encoded_size:.0f} KB")
    return tree, code

"""Call driver Function"""

# def main():
#     # Example data stream (simulate pixel values or symbols)
#     data = [1, 2, 5, 2, 4, 1, 2, 5, 3, 5, 0, 1]

#     # Initialize tree with the first symbol
#     tree = initialize_tree(data[0])
#     dict_node = tree  # Initially, the tree only has a dictionary node
#     root_ref = [tree]  # Use list for mutable reference to root

#     # Process each symbol in the stream
#     for value in data:
#         update_tree(root_ref, dict_node, value)
#         print_full_root(root_ref[0])
#         print("Next Loop\n")

# # Entry point
# if __name__ == '__main__':
#     main()

"""## v. Performance Test"""

# One Color Image
# tree, bitstream = run_adaptive_huffman_on_image("/home/sahau24/csc790project/Algo/dataset/blue.bmp", "out.txt")

# One noise image

# tree, bitstream = run_adaptive_huffman_on_image("/home/sahau24/csc790project/Algo/dataset/noise.bmp", "out.txt")

# One Full Color Image

# tree, bitstream = run_adaptive_huffman_on_image("/home/sahau24/csc790project/Algo/dataset/lichtenstein.bmp", "out.txt")

# tree, bitstream = run_adaptive_huffman_on_image("/home/sahau24/csc790project/Algo/dataset/noise.bmp", "out.txt")

def run_adaptive_huffman_on_image2(image_path: str, output_path: str) -> Tuple[Node, List[int]]:
    data = read_bitmap(image_path)
    tree = initialize_tree()
    dict_node = tree
    code = []

    for row in data:
        for pixel in row:

            tree, dict_node, encoded_bits = update_tree(tree, dict_node, pixel)
            code.extend(encoded_bits)

    input_size = os.path.getsize(image_path) / 1024 / 1024  # in MB
    encoded_size = len(code) / 8 / 1024  # in KB
    print(f"Original Image Size: {input_size:.2f} MB")
    print(f"Compressed File Size: {encoded_size:.2f} KB")
    print(f"Total Encoded Bits: {len(code)}")

    return tree, code

from typing import List, Optional,Tuple

# tree, bitstream = run_adaptive_huffman_on_image2("/home/sahau24/csc790project/Algo/dataset/blue.bmp", "out.txt")

# tree, bitstream = run_adaptive_huffman_on_image2("/home/sahau24/csc790project/Algo/dataset/lichtenstein.bmp", "out.txt")

