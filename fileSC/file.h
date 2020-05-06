//
// Created by Shinelon on 2020/5/3.
//

#ifndef HANDY_FILE_H
#define HANDY_FILE_H


#include "buffer.h"

struct File {
    File(char* path, const char* mode);
    ~File();
    void Read(Buffer &buffer, int &size);
    void Write(Buffer& buffer,int size);
    void Close();
    FILE * m_file;
    char* m_path;


};


#endif //HANDY_FILE_H
