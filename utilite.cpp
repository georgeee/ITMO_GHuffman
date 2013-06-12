/* 
 * File:   main.cpp
 * Author: georgeee
 *
 * Created on 12 Июнь 2013 г., 5:16
 */

#include <cstdlib>
#include <cstdio>
#include <string.h>
#include "huffman_tree/huffman_tree.h"
#include "huffman_zipper/huffman_zipper.h"
#include "huffman_zipper/huffman_zipper_file_io_exception.h"

using namespace std;

#define HZIP_MODE_ENCODE 1
#define HZIP_MODE_DECODE 2
#define HZIP_MODE_UNKNOWN (-1)
/*
 * 
 */
int main(int argc, char** argv) {
    int mode = HZIP_MODE_UNKNOWN;
    char * src_file;
    char * dest_file;
    int shift = 1;
    if(argc > shift){
        if(strcmp(argv[shift], "-e") == 0) mode = HZIP_MODE_ENCODE, ++shift;
        else if(strcmp(argv[shift], "-d") == 0) mode = HZIP_MODE_DECODE, ++shift;
        if(shift >= argc) mode = HZIP_MODE_UNKNOWN;
        else{
            if(mode == HZIP_MODE_UNKNOWN) mode = HZIP_MODE_ENCODE;
            src_file = argv[shift++];
            if(shift < argc){
                dest_file = argv[shift++];
            }else{
                int len = strlen(src_file);
                dest_file = new char[len+5];
                if(mode == HZIP_MODE_DECODE){
                    strncpy ( dest_file, src_file, len-3);
                    dest_file[len-3] = 0;
                }else if(mode == HZIP_MODE_ENCODE){
                    strncpy ( dest_file, src_file, len);
                    dest_file[len] = '.';
                    dest_file[len+1] = 'h';
                    dest_file[len+2] = 'z';
                    dest_file[len+3] = 0;
                }
            }
        }
    }
    if(mode == HZIP_MODE_UNKNOWN){
        printf("Syntax:\nhzip [-e] src_file [dest_file]            Encodes src_file, saves into dest_file\n");
        printf("hzip -d src_file [dest_file]            Decodes src_file, saves into dest_file\n");
    }else{
        huffman_zipper hz;
        if(mode == HZIP_MODE_DECODE){
            hz.decode_file(src_file, dest_file);
        }else if(mode == HZIP_MODE_ENCODE){
            hz.encode_file(src_file, dest_file);
        }
    }
    return 0;
}

