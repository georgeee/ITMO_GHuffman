/* 
 * File:   huffman_zipper.cpp
 * Author: georgeee
 * 
 * Created on 9 Июнь 2013 г., 22:00
 */

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
//unsigned int huffman_zipper::encode_string(char * src, char * dest, int src_len = -1){
//    
//}
//unsigned int huffman_zipper::decode_string(char * src, char * dest){
//void huffman_zipper::encode_file(char * src_path, char * dest_path);
//void huffman_zipper::decode_file(char * src_path, char * dest_path);
//huffman_zipper::~huffman_zipper();