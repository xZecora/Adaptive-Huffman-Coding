An implementation of a memory efficient Adaptive Huffman Coding algorithm
# Adaptive Huffman Coding: A Memory-Efficient Implementation

This repository contains a full implementation of a memory-efficient adaptive Huffman coding algorithm inspired by the work of **Pigeon and Bengio (1997)**. Unlike traditional adaptive Huffman coders that operate on grayscale images or limited symbols, this implementation is tested on **full color images**, encoding millions of unique 32-bit integer symbols effectively.

The algorithm avoids pre-scanning of the dataset and dynamically adjusts the Huffman tree based on observed symbol frequency, making it ideal for streaming or real-time encoding tasks.

---

## Motivation

In traditional Huffman coding, the symbol frequency distribution must be known in advance, which is often not feasible in streaming or dynamic contexts. Our implementation:

- Avoids pre-scanning the data
- Adapts symbol encoding based on frequency
- Handles large symbol sets (e.g., 32-bit integers for RGB image pixels)
- Maintains compression performance close to optimal static Huffman coding

---

## 📂 Project Structure
adaptive-huffman-coding/
├── cpp/                 # 💻 C++ implementation
│   ├── main.cpp         #    → Entry point for encoder
│   └── huffman_utils.hpp#    → Header with core Huffman logic
│
├── python/              # 🐍 Python implementation (NumPy + PIL)
│   ├── adaptive_huffman.py  # → Main encoder script
│   └── utils.py         # → Utility functions
│
├── data/              # 🖼️ Sample input/output images
│   ├── input/           #    → Original images
│   └── output/          #    → Encoded/decoded results
│
├── results/             # 📊 Encoded files, performance metrics
│
├── README.md            # 📘 Project overview and documentation
└── LICENSE              # 📄 License file (ACM-compatible)



See the paper 📄 [`A More Memory-Efficient Adaptive Huffman Coding`](https://github.com/xZecora/Adaptive-Huffman-Coding) for technical details and performance benchmarks.
