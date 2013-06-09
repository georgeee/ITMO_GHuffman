/* 
 * File:   huffman_tree_tests.cpp
 * Author: georgeee
 *
 * Created on 01.06.2013, 4:47:16
 * @TODO static function tests (huffman_tree::)
 */

#include <stdlib.h>
#include <ctime>
#include <iostream>
#include <string.h>
#include "../huffman_tree/huffman_tree.h"



#define BOOST_TEST_MODULE huffman_tree_tests
#define BOOST_TEST_DYN_LINK
#define BOOST_MAIN
#include <boost/test/unit_test.hpp>

using namespace std;

void test_build_serialize(char * str, char * check_value, bool print_debug = false, int str_len = -1) {
    unsigned long long * freqs = new unsigned long long [256];
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    huffman_tree ht(freqs);
    if (print_debug) ht.print_tree_debug_info();
    char * compressed = new char[256];
    short bit_cnt = 0;
    bit_cnt += ht.write_serialized(compressed, bit_cnt);
    BOOST_CHECK_EQUAL(strlen(check_value), bit_cnt);
    for (int i = 0; i < bit_cnt; i++) {
        BOOST_CHECK_EQUAL(check_value[i], huffman_tree::get_bit(compressed, i) ? '1' : '0');
    }
    delete[] freqs;
    delete[] compressed;
}

BOOST_AUTO_TEST_CASE(test_build_serialize_sizes) {
    srand(time(0));
    unsigned long long * freqs = new unsigned long long [256];
    for (int n = 10; n < 10000; n *= 4) {
        char * str = new char[n + 1];
        str[n] = 0;
        for (int i = 0; i < n; ++i) {
            str[i] = rand() % 256;
        }
        huffman_tree::inc_frequencies(str, freqs, 1);
        huffman_tree ht(freqs);
        int symbol_cnt = 0;
        for (int i = 0; i < 256; ++i) symbol_cnt += freqs[i] > 0;
        char * compressed = new char[1000];
        BOOST_CHECK_EQUAL(ht.write_serialized(compressed, 0), symbol_cnt * 11 + 2);
        delete[] str;
        delete[] compressed;
    }
    delete[] freqs;
}

BOOST_AUTO_TEST_CASE(test_build_serialize_0) {
    test_build_serialize("aaaaa\0", "0111010000110\0");
}

BOOST_AUTO_TEST_CASE(test_build_serialize_1) {
    test_build_serialize("a\0a", "010100001100111000000000\0", 0, 3);
}

BOOST_AUTO_TEST_CASE(test_build_serialize_2) {
    char * _str = new char[5] ;
    memset(_str, 0, sizeof(char)*5);
    _str[1] = _str[2] = (unsigned char) 255;
    test_build_serialize(_str, "010111111110111000000000\0", 0, 4);
    delete [] _str;
}

BOOST_AUTO_TEST_CASE(test_build_serialize_3) {
    //10000110 (97 a) 01000110 (98 b) 11000110 (99 c) 00100110 (100 d) 01001110 (114 r) 
    test_build_serialize("abracadabra\0", "010100001100010001001101001000110010010011100111011000110\0");
}

void test_encode_str(char * str, char * compressed_bits, bool include_eof = false, int str_len = -1) {
    unsigned long long * freqs = new unsigned long long [256];
    if (str_len < 0) str_len = strlen(str);
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    huffman_tree ht(freqs);
    char * compressed = new char[str_len * 2];
    unsigned int bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        bit_cnt += ht.write_code(str[i], compressed, bit_cnt);
    }
    if (include_eof) bit_cnt += ht.write_eof_code(compressed, bit_cnt);
    BOOST_CHECK_EQUAL(strlen(compressed_bits), bit_cnt);
    for (int i = 0; i < bit_cnt; i++) {
        BOOST_CHECK_EQUAL(compressed_bits[i], huffman_tree::get_bit(compressed, i) ? '1' : '0');
    }
    delete[] freqs;
    delete[] compressed;
}

BOOST_AUTO_TEST_CASE(test_encode_str_1) {
    test_encode_str("a\0", "1\0");
}

BOOST_AUTO_TEST_CASE(test_encode_str_2) {
    test_encode_str("aabbbbbbbbb\0", "0101111111111\0");
}

BOOST_AUTO_TEST_CASE(test_encode_str_3) {
    test_encode_str("abracadabra\0", "010111001111010001011100\0");
    test_encode_str("abracadabra\0", "0101110011110100010111001110\0", true);
}

void test_encode_decode(char * str, int str_len = -1) {
    unsigned long long * freqs = new unsigned long long [256];
    if (str_len < 0) str_len = strlen(str);
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    huffman_tree ht(freqs);
    char * compressed = new char[str_len * 2];
    unsigned int compressed_bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        compressed_bit_cnt += ht.write_code(str[i], compressed, compressed_bit_cnt);
    }
    compressed_bit_cnt += ht.write_eof_code(compressed, compressed_bit_cnt);
    unsigned int decoded_len = 0, bit_offset = 0;
    short ch;
    while ((ch = ht.get_char(compressed, bit_offset)) != -1) {
        BOOST_CHECK_EQUAL((unsigned char) str[decoded_len], ch);
        ++decoded_len;
    }
    BOOST_CHECK_EQUAL(decoded_len, str_len);
    delete[] freqs;
    delete[] compressed;
}

BOOST_AUTO_TEST_CASE(test_encode_decode_predefined_strings) {
    test_encode_decode("a\0");
    test_encode_decode("aabbbbbbbbb\0");
    test_encode_decode("abracadabra\0");
    test_encode_decode("Wait until the war is over\nAnd we're both a little older\nThe unknown soldier \0");
}

BOOST_AUTO_TEST_CASE(test_encode_decode_random_strings) {
    srand(time(0));
    for (int n = 10; n < 10000; n *= 4) {
        char * str = new char[n + 1];
        str[n] = 0;
        for (int i = 0; i < n; ++i) {
            str[i] = rand() % 256;
        }
        test_encode_decode(str);
        delete[] str;
    }
}

BOOST_AUTO_TEST_CASE(test_unserialize_tree_nocheck) {
    char * str = "aaaa\0";
    int str_len = 4;
    unsigned long long * freqs = new unsigned long long [256];
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    huffman_tree ht(freqs);
    char * _ht_serialized = new char[1000];
    ht.write_serialized(_ht_serialized, 0);
    unsigned int _bit_offset = 0;
    huffman_tree ht_copy(_ht_serialized, _bit_offset);

    delete[] _ht_serialized;
    delete[] freqs;
}

void test_unserialize_tree(char * str, int str_len = -1) {
    unsigned long long * freqs = new unsigned long long [256];
    char * _ht_serialized = new char[1000];
    if(str_len < 0) str_len = strlen(str);
    char * compressed = new char[str_len * 2];
    char * _compressed = new char[str_len * 2];
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    huffman_tree ht(freqs);
    ht.write_serialized(_ht_serialized, 0);
    unsigned int _bit_offset = 0;
    huffman_tree ht_copy(_ht_serialized, _bit_offset);
    unsigned int compressed_bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        compressed_bit_cnt += ht.write_code(str[i], compressed, compressed_bit_cnt);
    }
    compressed_bit_cnt += ht.write_eof_code(compressed, compressed_bit_cnt);
    unsigned int _compressed_bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        _compressed_bit_cnt += ht_copy.write_code(str[i], _compressed, _compressed_bit_cnt);
    }
    _compressed_bit_cnt += ht_copy.write_eof_code(_compressed, _compressed_bit_cnt);
    BOOST_CHECK_EQUAL(_compressed_bit_cnt, compressed_bit_cnt);
    for (unsigned int i = 0; i < compressed_bit_cnt; i++) {
        BOOST_CHECK_EQUAL(huffman_tree::get_bit(compressed, i), huffman_tree::get_bit(_compressed, i));
    }
    unsigned int decoded_len = 0, bit_offset = 0;
    short ch;
    while ((ch = ht_copy.get_char(compressed, bit_offset)) != -1) {
        BOOST_CHECK_EQUAL((unsigned char) str[decoded_len], ch);
        ++decoded_len;
    }
    BOOST_CHECK_EQUAL(decoded_len, str_len);
    delete[] compressed;
    delete[] _compressed;
    delete[] _ht_serialized;
    delete[] freqs;
}

BOOST_AUTO_TEST_CASE(test_unserialize_tree_predefined_strings) {
    test_unserialize_tree("a\0");
    test_unserialize_tree("aabbbbbbbbb\0");
    test_unserialize_tree("abracadabra\0");
    test_unserialize_tree("Wait until the war is over\nAnd we're both a little older\nThe unknown soldier \0");
}

BOOST_AUTO_TEST_CASE(test_unserialize_tree_random_strings) {
    srand(time(0));
    for (int n = 10; n < 10000; n *= 4) {
        char * str = new char[n + 1];
        str[n] = 0;
        for (int i = 0; i < n; ++i) {
            str[i] = rand() % 256;
        }
        test_unserialize_tree(str);
        delete[] str;
    }
}

void test_copy_tree(char * str, int str_len = -1) {
    unsigned long long * freqs = new unsigned long long [256];
    huffman_tree::inc_frequencies(str, freqs, 1, str_len);
    if(str_len < 0) str_len = strlen(str);
    huffman_tree ht(freqs);
    huffman_tree ht_copy(ht);
    char * compressed = new char[str_len * 2];
    unsigned int compressed_bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        compressed_bit_cnt += ht.write_code(str[i], compressed, compressed_bit_cnt);
    }
    compressed_bit_cnt += ht.write_eof_code(compressed, compressed_bit_cnt);
    char * _compressed = new char[str_len * 2];
    unsigned int _compressed_bit_cnt = 0;
    for (int i = 0; i < str_len; ++i) {
        _compressed_bit_cnt += ht_copy.write_code(str[i], _compressed, _compressed_bit_cnt);
    }
    _compressed_bit_cnt += ht_copy.write_eof_code(_compressed, _compressed_bit_cnt);
    BOOST_CHECK_EQUAL(_compressed_bit_cnt, compressed_bit_cnt);
    for (unsigned int i = 0; i < compressed_bit_cnt; i++) {
        BOOST_CHECK_EQUAL(huffman_tree::get_bit(compressed, i), huffman_tree::get_bit(_compressed, i));
    }
    unsigned int decoded_len = 0, bit_offset = 0;
    short ch;
    while ((ch = ht_copy.get_char(compressed, bit_offset)) != -1) {
        BOOST_CHECK_EQUAL((unsigned char) str[decoded_len], ch);
        ++decoded_len;
    }
    BOOST_CHECK_EQUAL(decoded_len, str_len);
    delete[] freqs;
    delete[] compressed;
    delete[] _compressed;
}

BOOST_AUTO_TEST_CASE(test_copy_tree_predefined_strings) {
    test_copy_tree("a\0");
    test_copy_tree("aabbbbbbbbb\0");
    test_copy_tree("abracadabra\0");
    test_copy_tree("Wait until the war is over\nAnd we're both a little older\nThe unknown soldier \0");
}

BOOST_AUTO_TEST_CASE(test_copy_tree_random_strings) {
    srand(time(0));
    for (int n = 10; n < 10000; n *= 4) {
        char * str = new char[n + 1];
        str[n] = 0;
        for (int i = 0; i < n; ++i) {
            str[i] = rand() % 256;
        }
        test_copy_tree(str);
        delete[] str;
    }
}
