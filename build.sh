#!/bin/bash
if [ ! -d build ]; then mkdir build; fi
if [ ! -d dist ]; then mkdir dist; fi

g++ -fPIC -c huffman_tree/huffman_tree.cpp -o build/huffman_tree.o
g++ -fPIC -c huffman_tree/huffman_tree_node.cpp -o build/huffman_tree_node.o
g++ -fPIC -c huffman_zipper/huffman_zipper.cpp -o build/huffman_zipper.o

g++ -shared -o dist/libHuffmanZipper.so build/huffman_tree.o build/huffman_tree_node.o build/huffman_zipper.o


if [ ! -d dist/includes ]; then mkdir dist/includes; fi
if [ ! -d dist/includes/huffman_tree ]; then mkdir dist/includes/huffman_tree; fi
if [ ! -d dist/includes/huffman_zipper ]; then mkdir dist/includes/huffman_zipper; fi

cp huffman_zipper/huffman_zipper.h huffman_zipper/huffman_zipper_file_io_exception.h dist/includes/huffman_zipper
cp huffman_tree/huffman_tree.h huffman_tree/huffman_tree_node.h dist/includes/huffman_tree

g++ -o dist/hzip utilite.cpp  -lHuffmanZipper -Ldist

rm -R build