//
// Created by Shinelon on 2020/4/17.
//

#ifndef HANDY_BUFFER_H
#define HANDY_BUFFER_H


#include <algorithm>
#include <string>
#include <string.h>
#include <sstream>
struct Buffer {

    char* m_data;
    size_t m_b,m_e, m_capital,m_size,m_expand;

    Buffer& operator=(const Buffer& b) {
        if(this==&b)
            return *this;
        delete[] m_data;
        m_data = nullptr;

    }
    Buffer(const Buffer &b) {
        CopyFrom(b);
    }

    Buffer(int size=0) {
        m_size = 0;
        m_expand=512;
        m_capital = 0;
        m_e = m_b = 0;
        m_data = nullptr;
    }
    bool Empty() const  {
        return m_e==m_b;
    }
     char* Begin() const{
        return  m_data + m_b;
    }
     char* End() const{
        return m_data + m_e;
    }
    char* Data() const {
        return m_data+m_b;
    }
    size_t Size() const {
        return m_e - m_b;
    }

    void Expand(size_t len) {
        size_t nCap =std::max(m_expand,std::max(2*m_capital,Size()+len));
        m_capital= nCap;
        char* p = new char[nCap];
        std::copy(Begin(),End(),p);
        m_e = Size();
        m_b = 0;
        delete [] m_data;
        m_data = p;
    }
    size_t Space() {
        return m_capital-m_e;
    }
    void MoveHead() {
      std::copy(Begin(),End(),m_data);
      m_e -=m_b;
      m_b = 0;
    }

    char* MakeRoom(size_t space) {
        if(space+m_e<m_capital) {
        } else if(Size()+space<m_capital/2){
            MoveHead();
        } else {
            Expand(space);
        }
        return End();
    }

    void MakeRoom() {
        if(Space()<m_expand) {
            Expand(0);
        }
    }
    void CopyFrom(const Buffer &b) {
        memcpy(this,&b,sizeof(b));
        if(b.m_data) {
            m_data = new char[m_capital];
            memcpy(m_data,b.Begin(),b.Size());
        }
    }
    void AddSize(size_t len) {
        m_e +=len;
    }

    char* AllocRoom(size_t len) {
        char* p = MakeRoom(len);
        AddSize(len);
        return p;
    }

    Buffer & Append(const char* p,size_t len ) {
        memcpy(AllocRoom(len),p,len);
        return *this;
    }

    Buffer& Append(const char *p) {
        return Append(p,strlen(p));
    }

    template <class T>
    Buffer & AppendValue(const T &v) {
        Append((const char*)&v,sizeof v);
        return *this;
    }
    void Clear() {
        delete [] m_data;
        m_data = nullptr;
        m_capital = 0;
        m_b = m_e = 0;
    }
    Buffer& Consume(size_t len) {
        m_b +=len;
        if(m_b>=m_e)
            Clear();
        return *this;
    }

    Buffer& Absorb(Buffer& buf) {
         if(&buf!=this) {
             if(Size()==0) {
                 char b[sizeof(buf)];
                 memcpy(b,this,sizeof(b));
                 memcpy(this,&buf,sizeof(b));
                 memcpy(&buf,b,sizeof(b));
                 std::swap(m_expand,buf.m_expand);
             } else {
                 Append(buf.Begin(),buf.Size());
                 buf.Clear();
             }
         }
         return *this;
    }
    std::string ToString() {
      std::stringstream ss;
      ss<<"\nm_b:"<<m_b<<" m_e:"<<m_e<<" size:"<<Size()<<" Cap:"<<m_capital<<"\n";
      ss<<"data:"<<Data();
      return  ss.str();
    }
};


#endif //HANDY_BUFFER_H
