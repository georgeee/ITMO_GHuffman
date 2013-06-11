/* 
 * File:   huffman_tree.cpp
 * Author: georgeee
 * 
 * Created on 1 Июнь 2013 г., 1:19
 */

#include "huffman_tree.h"
#include <vector>
#include <set>
#include <map>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstring>

using namespace std;

//@TODO Uncomment, when dynamic huffman
//huffman_tree::huffman_tree() {
//    int freqs[256];
//    for (int i = 0; i < 256; i++) {
//        freqs[i] = 1;
//    }
//    _init(freqs);
//}

huffman_tree::huffman_tree(const unsigned long long * initialFrequencies) {
    _init(initialFrequencies);
}

huffman_tree::huffman_tree(const char * bytes, unsigned int & bit_offset) {
    root = NULL;
    read_serialized(bytes, bit_offset);
}

huffman_tree::huffman_tree(const char * bytes, unsigned int bit_offset, unsigned short & shift) {
    root = NULL;
    read_serialized(bytes, bit_offset, shift);
}

void huffman_tree::_init(const unsigned long long * initialFrequencies) {
    node_next_id = 1;
    set<huffman_tree_node_pt, compare> set;
    for (int _char = 0; _char < 256; _char++) {
        int freq = initialFrequencies[_char];
        mapping[_char] = NULL;
        if (freq == 0) continue;
        mapping[_char] = new huffman_tree_node(node_next_id++, freq, _char);
        set.insert(mapping[_char]);
    }
    eofNode = new huffman_tree_node(0, 1);
    set.insert(eofNode);
    while (set.size() > 1) {
        huffman_tree_node_pt a = *set.begin();
        set.erase(set.begin());
        huffman_tree_node_pt b = *set.begin();
        set.erase(set.begin());
        huffman_tree_node_pt c = new huffman_tree_node(node_next_id++, b->weight + a->weight);
        c->left_child = a;
        c->right_child = b;
        a->parent = b->parent = c;
        set.insert(c);
    }
    root = *set.begin();
}

bool huffman_tree::compare::operator ()(const huffman_tree_node_pt& x, const huffman_tree_node_pt& y) const {
    return *x<*y;
}

unsigned short huffman_tree::write_code(unsigned char _char, char * buffer, unsigned int bit_offset) const {
    return write_code(mapping[_char], buffer, bit_offset);
}

unsigned short huffman_tree::write_code(huffman_tree_node_pt node, char * buffer, unsigned int bit_offset) const{
    huffman_tree_node_pt parent;
    unsigned short cnt = 0;
    while ((parent = node->parent) != NULL) {
        bool bit = parent->right_child == node;
        set_bit(buffer, bit_offset + cnt++, bit);
        node = parent;
    }
    reverse_bits(buffer, bit_offset, cnt);
    return cnt;
}

unsigned short huffman_tree::write_eof_code(char* buffer, unsigned int bit_offset) const {
    return write_code(eofNode, buffer, bit_offset);
}

void huffman_tree::set_bit(char * code, unsigned int bit_offset, bool bit) {
    if (bit)
        code[bit_offset / 8] |= (1 << bit_offset % 8);
    else
        code[bit_offset / 8] &= ~(1 << bit_offset % 8);
}

bool huffman_tree::get_bit(const char * code, unsigned int bit_offset) {
    return code[bit_offset / 8] & (1 << bit_offset % 8);
}

void huffman_tree::reverse_bits(char * code, unsigned int bit_offset, unsigned int cnt) {
    for (unsigned int i = 0; i < cnt / 2; i++) {
        unsigned int offset1 = bit_offset + i;
        unsigned int offset2 = bit_offset + (cnt - i - 1);
        bool bit1 = get_bit(code, offset1);
        bool bit2 = get_bit(code, offset2);
        set_bit(code, offset1, bit2);
        set_bit(code, offset2, bit1);
    }
}

void huffman_tree::inc_frequencies(const char * bytes, unsigned long long * freqs, bool erase_previous, int str_len) {
    if (erase_previous) memset(freqs, 0, sizeof (unsigned long long)*256);
    if (str_len < 0) str_len = strlen(bytes);
    for (int i = 0; i < str_len; i++) {
        freqs[(unsigned char) bytes[i]]++;
    }
}

void huffman_tree::print_tree_debug_info() const {
    root->debug_print();
}

huffman_tree_node_pt huffman_tree::find_by_code(const char * code, unsigned int bit_offset, unsigned short & shift) const {
    shift = 0;
    huffman_tree_node_pt node = root;
    while (!node->is_terminal())
        node = get_bit(code, bit_offset + shift++) ? node->right_child : node->left_child;
    return node;
}

huffman_tree_node_pt huffman_tree::find_by_code(const char * code, unsigned int & bit_offset) const{
    unsigned short shift;
    huffman_tree_node_pt node = find_by_code(code, bit_offset, shift);
    bit_offset += shift;
    return node;
}

short huffman_tree::get_char(const char * code, unsigned int & bit_offset) const {
    unsigned short shift;
    huffman_tree_node_pt node = find_by_code(code, bit_offset, shift);
    bit_offset += shift;
    return node == eofNode ? -1 : node->value;
}

short huffman_tree::get_char(const char* code, unsigned int bit_offset, unsigned short & shift) const{
    huffman_tree_node_pt node = find_by_code(code, bit_offset, shift);
    return node == eofNode ? -1 : node->value;
}

unsigned short huffman_tree::write_serialized(char* buffer, unsigned int bit_offset) const {
    return root->write_serialized(buffer, bit_offset);
}

void huffman_tree::read_serialized(const char * bytes, unsigned int & bit_offset) {
    unsigned short shift;
    read_serialized(bytes, bit_offset, shift);
    bit_offset += shift;
}

void huffman_tree::read_serialized(const char * bytes, unsigned int bit_offset, unsigned short & shift) {
    delete root;
    root = new huffman_tree_node;
    shift = root->read_serialized(bytes, bit_offset, &eofNode, &mapping[0]);
}

huffman_tree::huffman_tree(const huffman_tree& orig) {
    root = NULL;
    //However, the code needs exactly 11n+1 bits, where n is symbol count (excluding EOF),
    //so (11*256+1)/8=384 bytes will be enough
    char bytes[1000];
    unsigned int bit_offset = 0;
    orig.write_serialized(bytes, 0);
    read_serialized(bytes, bit_offset);
}

huffman_tree::~huffman_tree() {
    delete root;
}

