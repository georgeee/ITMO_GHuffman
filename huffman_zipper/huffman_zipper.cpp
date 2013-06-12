/* 
 * File:   huffman_zipper.cpp
 * Author: georgeee
 * 
 * Created on 9 Июнь 2013 г., 22:00
 */

#include <string.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

#include "huffman_zipper.h"
#include "../huffman_tree/huffman_tree.h"
#include "huffman_zipper_file_io_exception.h"

huffman_zipper::huffman_zipper(unsigned int buffer_size) {
    this->buffer_size = buffer_size;
}

huffman_zipper::huffman_zipper(const huffman_zipper& orig) {
    this->buffer_size = orig.buffer_size;
}

void huffman_zipper::set_buffer_size(unsigned int buffer_size) {
    this->buffer_size = buffer_size;
}

unsigned int huffman_zipper::get_buffer_size() {
    return buffer_size;
}

unsigned int huffman_zipper::encode_string(const char * src, char * dest, int src_len) {
    unsigned long long * freqs = new unsigned long long[256];
    if (src_len < 0) src_len = strlen(src);
    huffman_tree::inc_frequencies(src, freqs, 1, src_len);
    huffman_tree ht(freqs);
    unsigned int bit_offset = ht.write_serialized(dest, 0);
    for (int i = 0; i < src_len; i++) {
        bit_offset += ht.write_code(src[i], dest, bit_offset);
    }
    bit_offset += ht.write_eof_code(dest, bit_offset);
    delete[] freqs;
    return bit_offset / 8 + (bit_offset % 8 > 0);
}

unsigned int huffman_zipper::decode_string(const char * src, char * dest) {
    unsigned int bit_offset = 0;
    huffman_tree ht(src, bit_offset);
    unsigned int dest_length = 0;
    short ch;
    while ((ch = ht.get_char(src, bit_offset)) != -1) {
        dest[dest_length++] = ch;
    }
    return dest_length;
}

void huffman_zipper::encode_file(const char * src_path, const char * dest_path) const {
    FILE *src_fp = fopen(src_path, "r");
    FILE *dest_fp = fopen(dest_path, "w");
    char * double_buffer = new char[buffer_size * 2];
    char * buffer = new char[buffer_size];
    try {
        if (src_fp == NULL) {
            throw huffman_zipper_file_io_exception(src_path);
        }
        if (dest_fp == NULL) {
            throw huffman_zipper_file_io_exception(dest_path);
        }
        unsigned long long freqs[256];
        memset(freqs, 0, sizeof (unsigned long long)*256);
        memset(double_buffer, 0, sizeof (char)*2 * buffer_size);
        setbuf(src_fp, NULL);
        setbuf(dest_fp, NULL);
        size_t read_count;
        while (read_count = fread(buffer, 1, buffer_size, src_fp))
            huffman_tree::inc_frequencies(buffer, freqs, false, read_count);
        if (ferror(src_fp) != 0) throw huffman_zipper_file_io_exception(src_path);
        fseek(src_fp, 0, SEEK_SET);
        huffman_tree ht(freqs);
        unsigned int bit_cnt = ht.write_serialized(double_buffer, 0);
        while (read_count = fread(buffer, 1, buffer_size, src_fp)) {
            for (int i = 0; i < read_count; ++i) {
                write_flush_buffers(bit_cnt, double_buffer, dest_fp, dest_path);
                bit_cnt += ht.write_code(buffer[i], double_buffer, bit_cnt);
            }
        }
        if (ferror(src_fp) != 0) throw huffman_zipper_file_io_exception(src_path);
        write_flush_buffers(bit_cnt, double_buffer, dest_fp, dest_path);
        bit_cnt += ht.write_eof_code(double_buffer, bit_cnt);
        unsigned int byte_cnt = bit_cnt / 8 + (bit_cnt % 8 > 0);
        fwrite(double_buffer, 1, byte_cnt, dest_fp);
    } catch (huffman_zipper_file_io_exception e) {
        if (src_fp != NULL) fclose(src_fp);
        if (dest_fp != NULL) fclose(dest_fp);
        delete[] buffer;
        delete[] double_buffer;
        throw e;
    }
    fclose(src_fp);
    fclose(dest_fp);
    delete[] buffer;
    delete[] double_buffer;
}

void huffman_zipper::write_flush_buffers(unsigned int & bit_cnt, char * double_buffer, FILE* dest_fp, const char * dest_path) const {
    unsigned int byte_cnt = bit_cnt / 8 + (bit_cnt % 8 > 0);
    if (byte_cnt > buffer_size) {
        fwrite(double_buffer, 1, buffer_size, dest_fp);
        if (ferror(dest_fp) != 0) throw huffman_zipper_file_io_exception(dest_path);
        byte_cnt -= buffer_size;
        bit_cnt -= buffer_size * 8;
        memcpy(double_buffer, double_buffer + buffer_size, byte_cnt);
    }
}

void huffman_zipper::decode_file(const char * src_path, const char * dest_path) const {
    FILE *src_fp = fopen(src_path, "r");
    FILE *dest_fp = fopen(dest_path, "w");
    char * double_buffer = new char[buffer_size * 2];
    try {
        if (src_fp == NULL) {
            throw huffman_zipper_file_io_exception(src_path);
        }
        if (dest_fp == NULL) {
            throw huffman_zipper_file_io_exception(dest_path);
        }
        setbuf(src_fp, NULL);
        setvbuf(dest_fp, NULL, _IOFBF, buffer_size);
        size_t read_count = fread(double_buffer, 1, buffer_size * 2, src_fp);
        if (ferror(src_fp) != 0) throw huffman_zipper_file_io_exception(src_path);
        unsigned int bit_cnt = 0, byte_cnt;
        huffman_tree ht(double_buffer, bit_cnt);
        while (read_count > 0) {
            while ((byte_cnt = bit_cnt / 8 + (bit_cnt % 8 > 0)) <= buffer_size) {
                short _char = ht.get_char(double_buffer, bit_cnt);
                if (_char < 0) {//EOF
                    read_count = 0;
                    break;
                }
                fputc(_char, dest_fp);
                if (ferror(dest_fp) != 0) throw huffman_zipper_file_io_exception(dest_path);
            }
            if (read_count > 0) {
                byte_cnt = bit_cnt / 8;
                memcpy(double_buffer, double_buffer + byte_cnt, buffer_size * 2 - byte_cnt);
                read_count = fread(double_buffer + buffer_size * 2 - byte_cnt, sizeof (char), byte_cnt, src_fp) + buffer_size * 2 - byte_cnt;
                bit_cnt %= 8;
                if (ferror(src_fp) != 0) throw huffman_zipper_file_io_exception(src_path);
            }
        }
        fflush(dest_fp);
        if (ferror(dest_fp) != 0) throw huffman_zipper_file_io_exception(dest_path);
    } catch (huffman_zipper_file_io_exception e) {
        if (src_fp != NULL) fclose(src_fp);
        if (dest_fp != NULL) fclose(dest_fp);
        delete[] double_buffer;
        throw e;
    }
    fclose(src_fp);
    fclose(dest_fp);
    delete[] double_buffer;
}

huffman_zipper::~huffman_zipper() {

}