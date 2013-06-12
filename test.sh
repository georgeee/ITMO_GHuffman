#!/bin/bash
cd=$(pwd)
export LD_LIBRARY_PATH="$cd/dist"
for f in huffman_tree_tests.cpp \
		 huffman_zipper_tests.cpp ; do
	o=$(basename "$f"|sed -r 's/\.[a-z0-9]+$//')
	if [ ! "$1" == '-nc' ]; then
		g++ -o "tests/workaround/$o" "tests/$f" -lHuffmanZipper -L dist -lboost_unit_test_framework
	fi
	cd tests/workaround
	valgrind "./$o"
	cd "$cd"
done