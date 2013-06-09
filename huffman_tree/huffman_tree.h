/* 
 * File:   huffman_tree.h
 * Author: georgeee
 *
 * Created on 1 Июнь 2013 г., 1:19
 */


#include <vector>
#include <utility>
#include <bitset>

#include "huffman_tree_node.h"

#ifndef HUFFMAN_TREE_H
#define	HUFFMAN_TREE_H

class huffman_tree {
public:
    /**
     * 
     * @param initialFrequencies array of frequncies, size should be not less, that 256
     * (at i-th index of array should be frequency of (unsigned char)i symbol
     */
    //@TODO Uncomment, when dynamic huffman
    //huffman_tree();

    /**
     * 
     * @param initialFrequencies int[256], frequencies of symbols (for "aaaab" string initialFrequencies[(unsigned char)'a'] = 4)
     */
    huffman_tree(unsigned long long * initialFrequencies);
    /**
     * Reads serialized huffman tree from bytes
     * @param bytes
     * @param bit_offset
     */
    huffman_tree(char * bytes, unsigned int & bit_offset);
    /**
     * Reads serialized huffman tree from bytes
     * @param bytes
     * @param bit_offset
     * @param shift
     */
    huffman_tree(char * bytes, unsigned int bit_offset, unsigned short & shift);
    /**
     * Copies other huffman tree into self.
     * For purpose of copying serialization is being in use
     * @param orig
     */
    huffman_tree(const huffman_tree& orig);
    virtual ~huffman_tree();
    /**
     * Sets the bit in code
     * @param code
     * @param bit_offset
     * @param bit
     */
    static void set_bit(char * code, unsigned int bit_offset, bool bit);
    /**
     * Gets the bit from code
     * @param code
     * @param bit_offset
     * @return 
     */
    static bool get_bit(char * code, unsigned int bit_offset);
    /**
     * Reverses bits in code
     * @param code
     * @param bit_offset start index
     * @param cnt count of bits to reverse
     */
    static void reverse_bits(char * code, unsigned int bit_offset, unsigned int cnt);

    static void inc_frequencies(char * bytes, unsigned long long * freqs, bool erase_previous = false, int str_len = -1);
    /**
     * Encodes char to huffman code, writes bits of code into buffer
     * @param _char char to encode
     * @param buffer buffer, where we should put character's code bits
     * @param bit_offset offset in bits in buffer
     * @return count of bits, the symbol's code occupies
     */
    unsigned short write_code(unsigned char _char, char * buffer, unsigned int bit_offset);
    /**
     * Encodes EOF char to huffman code, writes bits of code into buffer
     * @param buffer buffer, where we should put EOF character's code bits
     * @param bit_offset offset in bits in buffer
     * @return count of bits, the EOF symbol's code occupies
     */
    unsigned short write_eof_code(char * buffer, unsigned int bit_offset);

    /**
     * 
     * @param code code bytes of symbol
     * @param bit_offset offset in bits, from which we should start reading
     * @param shift where resulting shift form bit_offset should be put (bit_offset+shift - offset for the next symbol search)
     * @return character, associated with the code (from 0 to 255, could be casted to char);
     *          -1 if eof symbol recieved 
     */
    short get_char(char * code, unsigned int bit_offset, unsigned short & shift);
    /**
     * 
     * @param code code bytes of symbol
     * @param bit_offset offset in bits, from which we should start reading (it will be updated for the next character search)
     * @return character, associated with the code (from 0 to 255, could be casted to char);
     *          -1 if eof symbol recieved 
     */
    short get_char(char * code, unsigned int & bit_offset);

    //@TODO Uncomment, when dynamic huffman
    //    /**
    //     * 
    //     * @param _char character to increase
    //     * @param inrement number, on which we should increase the weight of char
    //     */
    //    void increase_char(unsigned char _char, unsigned int inrement = 1);
    //    void increase_eof_char(unsigned int inrement = 1);
    
    /**
     * Prints some debug information about tree
     */
    void print_tree_debug_info();
    /**
     * Writes serialized tree to buffer
     * @param buffer Buffer, in which we should write bytes of serialized tree
     * @param bit_offset initial offset in buffer (in bits)
     * @return size of serialized tree in bits
     */
    unsigned short write_serialized(char * buffer, unsigned int bit_offset) const;
protected:
    huffman_tree_node_pt root;
    unsigned short write_code(huffman_tree_node_pt node, char * code, unsigned int bit_offset);

    //@TODO Uncomment, when dynamic huffman
    //    void increase_node_weight(huffman_tree_node_pt node, unsigned int increment = 1);

    huffman_tree_node_pt find_by_code(char * code, unsigned int bit_offset, unsigned short & shift);
    huffman_tree_node_pt find_by_code(char * code, unsigned int & bit_offset);
    unsigned short node_next_id;
    huffman_tree_node_pt mapping[256];
    huffman_tree_node_pt eofNode;
    void read_serialized(char * bytes, unsigned int & bit_offset);
    void read_serialized(char * bytes, unsigned int bit_offset, unsigned short & shift);

    class compare {
    public:
        bool operator() (const huffman_tree_node_pt & x, const huffman_tree_node_pt & y) const;
    };
private:
    void _init(unsigned long long * initialFrequencies);
};


#endif	/* HUFFMAN_TREE_H */

