ITMO_GHuffman
=============

Huffman algorithm implementation. Made as a library + tiny cmd utility.
My homework for c++ course (university, june 2013).

Description of files:
./build.sh                                #Shell script to build library, utilite
./test.sh                                 #Shell script to compile tests (if -nc param is not passed) 
											and test the library (using valgrind (if -nv not passed) and boost framework)
./utilite.cpp                             #Source file of cmd utilite
./huffman_zipper, ./huffman_tree          #Folders with library sources
./tests                                   #Folder with test sources
./tests/huffman_tree_tests.cpp            #huffman_tree class tester
./tests/huffman_zipper_tests.cpp          #huffman_zipper class tester
./tests/workaround                        #Folder, from which tests should be executed
./tests/workaround/test_files             #Files for testing methods huffman_zipper::encode_file(), huffman_zipper::decode_file()
./tests/workaround/test_files_str         #Files for testing methods huffman_zipper::encode_string(), huffman_zipper::decode_string()

After executing build script ./dist folder will occur:
./dist/includes               #Header files to include in your cpp sources
./dist/hzip                   #Binary, that requires libHuffmanZipper.so to work (shared)
./dist/hzip_static            #Binary, that doesn't use libHuffmanZipper.so (static)
./dist/libHuffmanZipper.so    #Shared library
./dist/libHuffmanZipper.a     #Static library

