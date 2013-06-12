#!/bin/bash
cd=$(pwd)
need_valgrind=false
if [ ! "$1" == '-nv' ] ; then
	if [ ! "$2" == '-nv' ]; then
		need_valgrind=true
	fi
fi
need_compile=false
if [ ! "$1" == '-nc' ] ; then
	if [ ! "$2" == '-nc' ]; then
		need_compile=true
	fi
fi
export LD_LIBRARY_PATH="$cd/dist"
for f in huffman_tree_tests.cpp \
		 huffman_zipper_tests.cpp ; do
	o=$(basename "$f"|sed -r 's/\.[a-z0-9]+$//')
	if [ $need_compile == true ] ; then
		g++ -o "tests/workaround/$o" "tests/$f" -lHuffmanZipper -L dist -lboost_unit_test_framework
	fi
	cd tests/workaround
	if [ $need_valgrind == true ] ; then
		valgrind "./$o"
	else
		"./$o"
	fi
	cd "$cd"
done