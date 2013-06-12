/* 
 * File:   huffman_zipper_file_io_exception.h
 * Author: georgeee
 *
 * Created on 11 Июнь 2013 г., 3:58
 */
#include <cstdlib>

using namespace std;

#ifndef HUFFMAN_ZIPPER_FILE_IO_EXCEPTION_H
#define	HUFFMAN_ZIPPER_FILE_IO_EXCEPTION_H

class huffman_zipper_file_io_exception : public exception{
public:
    const char * file;
    huffman_zipper_file_io_exception(const char * const file) throw(){
        this->file = file;
    }
    virtual ~huffman_zipper_file_io_exception()throw(){
        
    }
};

#endif	/* HUFFMAN_ZIPPER_FILE_IO_EXCEPTION_H */

