/* 
 * File:   newsimpletest.cpp
 * Author: georgeee
 *
 * Created on 01.06.2013, 4:47:16
 */

#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <string.h>
#include "../huffman_zipper/huffman_zipper.h"
#include "../huffman_tree/huffman_tree.h"



#define BOOST_TEST_MODULE huffman_zipper_tests
#define BOOST_TEST_DYN_LINK
#define BOOST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

void _test_encode_string_simple(){
    char * decoded_str = "abracadabra\0";
    unsigned int decoded_length = strlen(decoded_str);
    char * __encoded_str = "0101000011000100010011010010001100100100111001110110001100101110011110100010111001110\0\0\0\0\0\0\0\0\0";
    unsigned int encoded_length = strlen(__encoded_str) / 8 + 1;
    char * encoded_str = new char[encoded_length];
    memset(encoded_str, 0, sizeof (char)*encoded_length);
    for (int i = 0; i < encoded_length * 8; ++i) {
        huffman_tree::set_bit(encoded_str, i, __encoded_str[i] == '1');
    }
    char * new_decoded = new char[decoded_length * 4];
    char * new_encoded = new char[encoded_length * 4];
    memset(new_encoded, 0, sizeof (char)*encoded_length * 4);
    huffman_zipper hz;
    unsigned int new_decoded_len = hz.decode_string(encoded_str, new_decoded);
    unsigned int new_encoded_len = hz.encode_string(decoded_str, new_encoded, decoded_length);
    BOOST_CHECK_EQUAL(new_decoded_len, decoded_length);
    BOOST_CHECK_EQUAL(new_encoded_len, encoded_length);
    for (int i = 0; i < decoded_length; i++) {
        BOOST_CHECK_EQUAL(decoded_str[i], new_decoded[i]);
    }
    for (int i = 0; i < encoded_length; i++) {
        BOOST_CHECK_EQUAL(encoded_str[i], new_encoded[i]);
    }
    delete[] encoded_str;
    delete[] new_decoded;
    delete[] new_encoded;
}

BOOST_AUTO_TEST_CASE(test_encode_string_simple) {
    _test_encode_string_simple();
}

void test_encode_decode_string(char * src, int src_len = -1) {
    if (src_len < 0) src_len = strlen(src);
    char * dest = new char[src_len * 2];
    char * new_src = new char[src_len * 2];
    huffman_zipper hz;
    hz.encode_string(src, dest, src_len);
    unsigned int new_src_len = hz.decode_string(dest, new_src);
    BOOST_CHECK_EQUAL(src_len, new_src_len);
    for (int i = 0; i < src_len; i++) {
        BOOST_CHECK_EQUAL(src[i], new_src[i]);
    }
    delete[] dest;
    delete[] new_src;
}


BOOST_AUTO_TEST_CASE(test_encode_decode_predefined_strings){
    test_encode_decode_string("abracadabra\0");
    test_encode_decode_string("asddkpowokwd;asla\0");
    test_encode_decode_string("a\0ad", 4);
    test_encode_decode_string("Waiting for the sun...\0");
}

BOOST_AUTO_TEST_CASE(test_encode_decode_random_strings) {
    srand(time(0));
    for (int n = 10; n < 10000; n *= 4) {
        char * str = new char[n + 1];
        str[n] = 0;
        for (int i = 0; i < n; ++i) {
            str[i] = rand() % 256;
        }
        test_encode_decode_string(str);
        delete[] str;
    }
}