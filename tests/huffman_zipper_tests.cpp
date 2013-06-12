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
#include <cstdio>
#include "../huffman_zipper/huffman_zipper.h"
#include "../huffman_tree/huffman_tree.h"
#include "../huffman_zipper/huffman_zipper_file_io_exception.h"




#define BOOST_TEST_MODULE huffman_zipper_tests
#define BOOST_ALL_DYN_LINK
#define BOOST_MAIN
#include <boost/test/unit_test.hpp>

//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
//namespace fs = boost::filesystem;

using namespace std;

#define HUFFMAN_ZIPPER_TESTS_STR_MAX_LEN 10000

bool _file_exists(const char * path) {
    FILE * fp = fopen(path, "r");
    if (fp == NULL) return false;
    fclose(fp);
    return true;
}

bool _file_check_equal(const char * file1, const char * file2, int bit_cnt = -1) {
    FILE * fp1 = fopen(file1, "r");
    FILE * fp2 = fopen(file2, "r");
    BOOST_CHECK(fp1 != NULL);
    BOOST_CHECK(fp2 != NULL);
    if (fp1 == NULL || fp2 == NULL) return false;
    setvbuf(fp1, NULL, _IOFBF, 1024);
    setvbuf(fp2, NULL, _IOFBF, 1024);
    int ch1, ch2;
    int bc = 0;
    do {
        ch1 = fgetc(fp1);
        ch2 = fgetc(fp2);
        ++bc;
        if (bit_cnt >= 0 && bc * 8 > bit_cnt) break;
        BOOST_CHECK_EQUAL(ch1, ch2);
        if (ch1 != ch2) {
            fclose(fp1);
            fclose(fp2);
            return false;
        }
    } while (ch1 != EOF && ch2 != EOF);
    if (bit_cnt >= 0 && bc > 0) {
        bc = bc * 8 - bit_cnt;
        char _ch1 = ch1, _ch2 = ch2;
        for (int i = 0; i < bc; ++i) {
            bool bit1 = huffman_tree::get_bit(&_ch1, i), bit2 = huffman_tree::get_bit(&_ch2, i);
            BOOST_CHECK_EQUAL(bit1, bit2);
            if (bit1 != bit2) {
                fclose(fp1);
                fclose(fp2);
                return false;
            }
        }
    }
    fclose(fp1);
    fclose(fp2);
    return true;
}

int _file_size(const char * path) {
    FILE * fp = fopen(path, "r");
    if (fp == NULL) return 0;
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fclose(fp);
    return size;
}

void _file_put_bytes(const char * fpath, const char * bytes, int length = -1) {
    if (length < 0) length = strlen(bytes);
    FILE * fp = fopen(fpath, "w");
    setbuf(fp, NULL);
    fwrite(bytes, 1, length, fp);
    fclose(fp);
}

void _file_get_bytes(const char * fpath, char * bytes, int count = -1) {
    if (count < 0) count = _file_size(fpath);
    FILE * fp = fopen(fpath, "r");
    setbuf(fp, NULL);
    fread(bytes, 1, count, fp);
    fclose(fp);
}

BOOST_AUTO_TEST_CASE(test_encode_decode_string_simple) {
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

unsigned int test_encode_decode_string(char * src, int src_len = -1, char * encoded_str = NULL) {
    if (src_len < 0) src_len = strlen(src);
    char * dest;
    if (encoded_str == NULL) dest = new char[src_len + 1024];
    else dest = encoded_str;
    char * new_src = new char[src_len + 1024];
    huffman_zipper hz;
    unsigned int dest_byte_cnt = hz.encode_string(src, dest, src_len);
    unsigned int new_src_len = hz.decode_string(dest, new_src);
    BOOST_CHECK_EQUAL(src_len, new_src_len);
    for (int i = 0; i < src_len; i++) {
        BOOST_CHECK_EQUAL(src[i], new_src[i]);
    }
    if (encoded_str == NULL) delete[] dest;
    delete[] new_src;
    return dest_byte_cnt;
}

BOOST_AUTO_TEST_CASE(test_encode_decode_predefined_strings) {
    test_encode_decode_string("abracadabra\0");
    test_encode_decode_string("asddkpowokwd;asla\0");
    test_encode_decode_string("a\0ad", 4);
    test_encode_decode_string("Waiting for the sun...\0");
}

void test_encode_decode_string_from_file(const char * fpath, const char * encoded_path = NULL) {
    int fsize = _file_size(fpath);
    char dest_bytes[fsize + 1024];
    memset(dest_bytes, 0, (fsize + 1024) * sizeof (char));
    char bytes[fsize];
    _file_get_bytes(fpath, bytes, fsize);
    unsigned int dest_byte_cnt = test_encode_decode_string(bytes, fsize, dest_bytes);
    //    fs::path orig_path(fpath);
    //    fs::path enc_dir = orig_path.parent_path() / fs::path("encoded");
    //    if (fs::exists(enc_dir) && fs::is_directory(enc_dir))
    //      _file_put_bytes((enc_dir / orig_path.filename()).c_str(), dest_bytes, dest_byte_cnt);
    if (encoded_path != NULL) {
        _file_put_bytes(encoded_path, dest_bytes, dest_byte_cnt);
    }
}

BOOST_AUTO_TEST_CASE(test_encode_decode_string_predefined_files) {
    //    fs::path dir("test_files_str");
    //    if (fs::exists(dir) && fs::is_directory(dir)) {
    //        fs::directory_iterator end_iter;
    //        for (fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr) {
    //            try {
    //                if (fs::is_regular_file(dir_itr->status())) {
    //                    fs::path fpath_obj = dir_itr->path();
    //                    const char * fpath = fpath_obj.native().c_str();
    //                    test_encode_decode_string_from_file(fpath);
    //                }
    //            } catch (const std::exception & ex) {
    //            }
    //        }
    //    }
    test_encode_decode_string_from_file("test_files_str/rand_1", "test_files_str/encoded/rand_1.hz");
    test_encode_decode_string_from_file("test_files_str/rand_2", "test_files_str/encoded/rand_2.hz");
    test_encode_decode_string_from_file("test_files_str/rand_3", "test_files_str/encoded/rand_3.hz");
}

BOOST_AUTO_TEST_CASE(test_encode_decode_random_strings) {
    srand(time(0));
    for (int n = 10; n < HUFFMAN_ZIPPER_TESTS_STR_MAX_LEN; n *= 4) {
        char * str = new char[n];
        for (int i = 0; i < n; ++i) {
            str[i] = rand();
        }
        test_encode_decode_string(str, n);
        delete[] str;
    }
}

BOOST_AUTO_TEST_CASE(test_file_not_exists) {
    huffman_zipper hz;
    char tmp_src[255];
    tmpnam(tmp_src);
    if (_file_exists(tmp_src)) {
        remove(tmp_src);
    }
    char tmp_src2[255];
    tmpnam(tmp_src2);
    BOOST_CHECK_THROW(hz.encode_file(tmp_src, tmp_src2), huffman_zipper_file_io_exception);
    BOOST_CHECK_THROW(hz.decode_file(tmp_src, tmp_src2), huffman_zipper_file_io_exception);
    if (_file_exists(tmp_src2)) {
        remove(tmp_src2);
    }
}

BOOST_AUTO_TEST_CASE(test_encode_decode_file_simple) {
    char * decoded_str = "abracadabra\0";
    unsigned int decoded_length = strlen(decoded_str);
    char * __encoded_str = "0101000011000100010011010010001100100100111001110110001100101110011110100010111001110\0\0\0";
    unsigned int encoded_length = 11;
    char * encoded_str = new char[encoded_length];
    memset(encoded_str, 0, sizeof (char)*encoded_length);
    for (int i = 0; i < encoded_length * 8; ++i) {
        huffman_tree::set_bit(encoded_str, i, __encoded_str[i] == '1');
    }
    huffman_zipper hz;
    char dtmp[255];
    char etmp[255];
    char ndtmp[255];
    char netmp[255];
    tmpnam(dtmp);
    tmpnam(ndtmp);
    tmpnam(etmp);
    tmpnam(netmp);
    _file_put_bytes(dtmp, decoded_str, decoded_length);
    _file_put_bytes(etmp, encoded_str, encoded_length);
    hz.encode_file(dtmp, netmp);
    hz.decode_file(etmp, ndtmp);
    BOOST_CHECK(_file_check_equal(dtmp, ndtmp));
    BOOST_CHECK(_file_check_equal(etmp, netmp, 85));
    if (_file_exists(dtmp)) remove(dtmp);
    if (_file_exists(etmp)) remove(etmp);
    if (_file_exists(ndtmp)) remove(ndtmp);
    if (_file_exists(netmp)) remove(netmp);
    delete[] encoded_str;
}

void test_encode_decode_file_from_str(char * str, int str_len = -1) {
    if (str_len < 0) str_len = strlen(str);
    huffman_zipper hz;
    char dtmp[255];
    char etmp[255];
    char ndtmp[255];
    tmpnam(dtmp);
    tmpnam(ndtmp);
    tmpnam(etmp);
    _file_put_bytes(dtmp, str, str_len);
    hz.encode_file(dtmp, etmp);
    hz.decode_file(etmp, ndtmp);
    bool eq;
    BOOST_CHECK(eq = _file_check_equal(dtmp, ndtmp));
    if (eq) {
        if (_file_exists(dtmp)) remove(dtmp);
        if (_file_exists(etmp)) remove(etmp);
        if (_file_exists(ndtmp)) remove(ndtmp);
    } else {
        printf("Test failed: dtmp=%s etmp=%s ndtmp%s\n", dtmp, etmp, ndtmp);
    }
}

void test_encode_decode_file(const char * original, const char * encoded = NULL) {
    huffman_zipper hz;
    const char *etmp;
    char * _etmp = NULL;
    if (encoded == NULL) {
        _etmp = new char[2550];
        tmpnam(_etmp);
        etmp = _etmp;
    } else etmp = encoded;
    char * ndtmp = new char[2550];
    tmpnam(ndtmp);
    hz.encode_file(original, etmp);
    hz.decode_file(etmp, ndtmp);
    bool eq;
    BOOST_CHECK(eq = _file_check_equal(original, ndtmp));
    if (eq) {
        if (_file_exists(etmp) && encoded == NULL) remove(etmp);
        if (_file_exists(ndtmp)) remove(ndtmp);
    } else {
        printf("Test failed: dtmp='%s' etmp='%s' ndtmp='%s'\n", original, etmp, ndtmp);
    }
    delete[] _etmp;
    delete[] ndtmp;
}

BOOST_AUTO_TEST_CASE(test_encode_decode_file_predefined_strings) {
    test_encode_decode_file_from_str("abracadabra\0");
    test_encode_decode_file_from_str("asddkpowokwd;asla\0");
    test_encode_decode_file_from_str("a\0ad", 4);
    test_encode_decode_file_from_str("Waiting for the sun...\0");
}

BOOST_AUTO_TEST_CASE(test_encode_decode_file_predefined_files) {
    //        fs::path dir("test_files");
    //        if (fs::exists(dir) && fs::is_directory(dir)) {
    //            fs::directory_iterator end_iter;
    //            for (fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr) {
    //                try {
    //                    if (fs::is_regular_file(dir_itr->status())) {
    //                        fs::path fpath = dir_itr->path();
    //                        const char * encoded_path = NULL;
    //                        fs::path encoded_dir = fpath.parent_path() / fs::path("encoded");
    //                        if (fs::exists(encoded_dir) && fs::is_directory(encoded_dir))
    //                            encoded_path = (encoded_dir / fpath.filename()).c_str();
    //                        test_encode_decode_file(fpath.native().c_str(), encoded_path);
    //                    }
    //                } catch (const std::exception & ex) {
    //                    throw ex;
    //                }
    //            }
    //        }
    test_encode_decode_file("test_files/rand_gen_1", "test_files/encoded/rand_gen_1.hz");
    test_encode_decode_file("test_files/rand_gen_2", "test_files/encoded/rand_gen_2.hz");
    test_encode_decode_file("test_files/rand_3", "test_files/encoded/rand_3.hz");
    test_encode_decode_file("test_files/linal.djvu", "test_files/encoded/linal.djvu.hz");
}

BOOST_AUTO_TEST_CASE(test_encode_decode_file_random_strings) {
    srand(time(0));
    for (int n = 10; n < HUFFMAN_ZIPPER_TESTS_STR_MAX_LEN; n *= 4) {
        char * str = new char[n];
        for (int i = 0; i < n; ++i) {
            str[i] = rand();
        }
        test_encode_decode_file_from_str(str, n);
        delete[] str;
    }
}