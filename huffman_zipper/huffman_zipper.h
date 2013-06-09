/* 
 * File:   huffman_zipper.h
 * Author: georgeee
 *
 * Created on 9 Июнь 2013 г., 22:00
 */

#ifndef HUFFMAN_ZIPPER_H
#define	HUFFMAN_ZIPPER_H

class huffman_zipper {
public:
    huffman_zipper();
    huffman_zipper(unsigned int buffer_size);
    huffman_zipper(const huffman_zipper& orig);
    void set_buffer_size(unsigned int buffer_size);
    unsigned int get_buffer_size();
//    unsigned int encode_string(char * src, char * dest, int src_len = -1);
//    unsigned int decode_string(char * src, char * dest);
//    void encode_file(char * src_path, char * dest_path);
//    void decode_file(char * src_path, char * dest_path);
//    virtual ~huffman_zipper();
private:
    unsigned int buffer_size;
};

#endif	/* HUFFMAN_ZIPPER_H */

