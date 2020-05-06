//
// Created by Shinelon on 2020/5/3.
//

#include "file.h"

File::File(char *path, const char *mode) {
    m_file = fopen(path, mode);
}

void File::Read(Buffer &buffer,int& size) {
    size = 0;
    int bufsize=1024;
    char buf[bufsize];
    while(!feof(m_file)) {
        int curSize = fread(buf,1,bufsize,m_file);
        buffer.Append(buf,curSize);
        size+=curSize;
    }

}

void File::Write(Buffer &buffer, int size) {
    fwrite(buffer.Begin(),1,size,m_file);
}

File::~File() {

}

void File::Close() {
    fclose(m_file);
}
