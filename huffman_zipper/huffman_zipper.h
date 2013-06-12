/* 
 * File:   huffman_zipper.h
 * Author: georgeee
 *
 * Created on 9 Июнь 2013 г., 22:00
 */


using namespace std;

#ifndef HUFFMAN_ZIPPER_H
#define	HUFFMAN_ZIPPER_H

class huffman_zipper {
public:
    huffman_zipper(unsigned int buffer_size = 1024);
    huffman_zipper(const huffman_zipper& orig);
    void set_buffer_size(unsigned int buffer_size);
    unsigned int get_buffer_size() const;
    unsigned int encode_string(const char * src, char * dest, int src_len = -1) const;
    unsigned int decode_string(const char * src, char * dest) const;
    void encode_file(const char * src_path, const char * dest_path) const;
    void decode_file(const char * src_path, const char * dest_path) const;
    virtual ~huffman_zipper();
protected:
    unsigned int buffer_size;
    void write_flush_buffers(unsigned int & bit_cnt, char * double_buffer, FILE* dest_fp, const char * dest_path) const;
};

#endif	/* HUFFMAN_ZIPPER_H */

