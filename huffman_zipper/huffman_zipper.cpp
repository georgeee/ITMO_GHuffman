/* 
 * File:   huffman_zipper.cpp
 * Author: georgeee
 * 
 * Created on 9 Июнь 2013 г., 22:00
 */

#include <string.h>

#include "huffman_zipper.h"
#include "../huffman_tree/huffman_tree.h"

huffman_zipper::huffman_zipper(){
    buffer_size = 1024;
}
huffman_zipper::huffman_zipper(unsigned int buffer_size){
    this->buffer_size = buffer_size;
}
huffman_zipper::huffman_zipper(const huffman_zipper& orig){
    this->buffer_size = orig.buffer_size;
}
void huffman_zipper::set_buffer_size(unsigned int buffer_size){
    this->buffer_size = buffer_size;
}
unsigned int huffman_zipper::get_buffer_size(){
    return buffer_size;
}
unsigned int huffman_zipper::encode_string(char * src, char * dest, int src_len){
    unsigned long long * freqs = new unsigned long long[256];
    if(src_len < 0) src_len = strlen(src);
    huffman_tree::inc_frequencies(src, freqs, 1, src_len);
    huffman_tree ht(freqs);
    unsigned int bit_offset = ht.write_serialized(dest, 0);
    for (int i = 0; i < src_len; i++) {
        bit_offset += ht.write_code(src[i], dest, bit_offset);
    }
    bit_offset += ht.write_eof_code(dest, bit_offset);
    delete[] freqs;
    return bit_offset/8 + (bit_offset%8 > 0);
}
unsigned int huffman_zipper::decode_string(char * src, char * dest){
    unsigned int bit_offset = 0;
    huffman_tree ht(src, bit_offset);
    unsigned int dest_length = 0;
    short ch;
    while((ch = ht.get_char(src, bit_offset)) != -1){
        dest[dest_length++] = ch;
    }
    return dest_length;
}
//void huffman_zipper::encode_file(char * src_path, char * dest_path);
//void huffman_zipper::decode_file(char * src_path, char * dest_path);
huffman_zipper::~huffman_zipper(){
    
}