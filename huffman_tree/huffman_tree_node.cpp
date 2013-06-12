/* 
 * File:   huffman_tree_node.cpp
 * Author: georgeee
 * 
 * Created on 1 Июнь 2013 г., 1:22
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include "huffman_tree_node.h"
#include "huffman_tree.h"


using namespace std;

void huffman_tree_node::_init(unsigned short id, unsigned long long weight, char value) {
    this->id = id;
    this->value = value;
    this->weight = weight;
    left_child = right_child = parent = NULL;
}

huffman_tree_node::huffman_tree_node(unsigned short id, unsigned long long weight, char value) {
    _init(id, weight, value);
}

huffman_tree_node::huffman_tree_node(unsigned short id, unsigned long long weight) {
    _init(id, weight, 0);
}

huffman_tree_node::huffman_tree_node(unsigned short id) {
    _init(id, 0, 0);
}

huffman_tree_node::huffman_tree_node() {
    _init(0, 0, 0);
}

bool huffman_tree_node::operator<(const huffman_tree_node& node) const {
    if (weight != node.weight) return weight < node.weight;
    return id < node.id;
}

bool huffman_tree_node::is_terminal() const {
    return left_child == NULL && right_child == NULL;
}

bool huffman_tree_node::is_eof_node() const {
    return id == 0;
}

huffman_tree_node::~huffman_tree_node() {
    delete left_child;
    delete right_child;
}

void huffman_tree_node::debug_print() const {
    cout << id << ") weight: " << weight << ", char: " << value << " (" << (int) value << ")"
            << " left: " << (left_child == NULL ? -1 : left_child->id) << " right: " << (right_child == NULL ? -1 : right_child->id)
            << " parent: " << (parent == NULL ? -1 : parent->id) << endl;
    if (left_child != NULL) left_child->debug_print();
    if (right_child != NULL) right_child->debug_print();
}

unsigned short huffman_tree_node::write_serialized(char * buffer, unsigned int bit_offset) const {
    unsigned short shift = 0;
    if (is_terminal()) {
        huffman_tree::set_bit(buffer, bit_offset + shift++, 1);
        if (id == 0) {//EOF char
            huffman_tree::set_bit(buffer, bit_offset + shift++, 1);
        } else {
            huffman_tree::set_bit(buffer, bit_offset + shift++, 0);
            for (int i = 0; i < 8; i++) {
                huffman_tree::set_bit(buffer, bit_offset + shift++, (value & (1 << i)) > 0);
            }
        }
    } else {
        huffman_tree::set_bit(buffer, bit_offset + shift++, 0);
        shift += left_child->write_serialized(buffer, bit_offset + shift);
        shift += right_child->write_serialized(buffer, bit_offset + shift);
    }
    return shift;
}

unsigned short huffman_tree_node::read_serialized(const char * bytes, unsigned int bit_offset, huffman_tree_node_pt * eofNode, huffman_tree_node_pt * mapping, huffman_tree_node_pt parent) {
    unsigned short shift = 0;
    bool type_bit = huffman_tree::get_bit(bytes, bit_offset + shift++);
    this->parent = parent;
    value = 0;
    if (type_bit) {
        bool is_eof_bit = huffman_tree::get_bit(bytes, bit_offset + shift++);
        if (is_eof_bit) {
            id = 0;
            *eofNode = this;
        } else {
            for (int i = 0; i < 8; i++) {
                value |= (1 << i) * huffman_tree::get_bit(bytes, bit_offset + shift++);
            }
            mapping[value] = this;
        }
        left_child = right_child = NULL;
    } else {
        left_child = new huffman_tree_node;
        right_child = new huffman_tree_node;
        shift += left_child->read_serialized(bytes, bit_offset + shift, eofNode, mapping, this);
        shift += right_child->read_serialized(bytes, bit_offset + shift, eofNode, mapping, this);
    }
    return shift;
}
