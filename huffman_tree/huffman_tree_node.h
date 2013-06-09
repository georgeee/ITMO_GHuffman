/* 
 * File:   huffman_tree_node.h
 * Author: georgeee
 *
 * Created on 1 Июнь 2013 г., 1:22
 */

#ifndef HUFFMAN_TREE_NODE_H
#define	HUFFMAN_TREE_NODE_H

class huffman_tree_node {
    friend class huffman_tree;
    typedef huffman_tree_node* huffman_tree_node_pt;
protected:
    unsigned char value;
    unsigned long long weight;
    unsigned short id;
    huffman_tree_node_pt left_child;
    huffman_tree_node_pt right_child;
    huffman_tree_node_pt parent;
    huffman_tree_node(unsigned short id, unsigned long long weight, char value);
    huffman_tree_node(unsigned short id, unsigned long long weight);
    huffman_tree_node(unsigned short id);
    huffman_tree_node();
    bool is_terminal() const;
    bool is_eof_node() const;
    bool operator<(const huffman_tree_node & node) const;
    virtual ~huffman_tree_node();
    void _init(unsigned short id, unsigned long long weight, char value);
    void debug_print() const;
    /**
     * Writes serialized node's tree to buffer
     * @param buffer Buffer, in which we should write bytes of serialized node
     * @param bit_offset initial offset in buffer (in bits)
     * @return size of serialized tree in bits
     */
    unsigned short write_serialized(char * buffer, unsigned int bit_offset) const;
    unsigned short read_serialized(char * bytes, unsigned int bit_offset, huffman_tree_node_pt * eofNode, huffman_tree_node_pt * mapping, huffman_tree_node_pt parent = NULL);
};
typedef huffman_tree_node* huffman_tree_node_pt;

#endif	/* HUFFMAN_TREE_NODE_H */

