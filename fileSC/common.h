//
// Created by Shinelon on 2020/5/3.
//

#pragma  once

#include <dirent.h>
#include "buffer.h"
#include "conn.h"
#include "logging.h"
using  namespace handy;
enum Op: int {
    FileGet,
    FilePut,
    Msg,
    FileLs,
    Cd,
    Pwd
};


//一个问题，为什么不加static会报错重定义呢？明显用了#pragma once

/*
 * format
 * FilePut FileGet
 * |size of message (int)|operation code (int)|filename length (int)|filename (string)| file content (string)|
 * Msg FileLs
 * |size of message (int)|operation code (int)| file content (string)|
 */
static void EncodeMsg(Op op, Buffer& inputData, Buffer& resData, char* path=NULL) {
   if(path!=NULL) {
       int size = inputData.Size() + sizeof(int) * 3 + strlen(path);
       // fatalif(resData.Size()==0,"resData is not empty!");
       resData.AppendValue<int>(size);
       resData.AppendValue<Op>(op);
       resData.AppendValue<int>(strlen(path));
       resData.Append(path);
       resData.Absorb(inputData);
       int *temp = (int *) resData.Begin();
       if (*temp != size) {
           printf("not equal!");
       }
   } else {
       int size = inputData.Size() + sizeof(int) * 2 ;
       // fatalif(resData.Size()==0,"resData is not empty!");
       resData.AppendValue<int>(size);
       resData.AppendValue<Op>(op);
       resData.Absorb(inputData);
       int *temp = (int *) resData.Begin();
       if (*temp != size) {
           printf("not equal!");
       }
   }
}

/*
 * refer to the format of encode to decode
 */
/*
 * format
 * |size of message (int)|operation code (int)|filename length (int)|filename (string)| file content (string)|
 */
static bool  DecodeMsg(Buffer& inputData, int& size,Op& op,std::string& filename) {

      int* temp = (int*) inputData.Begin();
      //does not receive enough data from buffer
      if(*temp>inputData.Size()) return false;
      //the data in buffer is enough to decode to a complete message
      inputData.PopValue<int>(size);
      inputData.PopValue<Op>(op);
      if(op==Op::Msg||op==Op::FileLs||op==Op::Cd) {
          size = size-2*sizeof(int);
      } else {
          int nameLen = 0;
          inputData.PopValue<int>(nameLen);
          inputData.PopStringWithoutDataLength(filename,nameLen);
          size = size - 3*sizeof(int)-nameLen;
      }
      return true;
}

static void FileNotExist(handy::TcpConnPtr conn,std::string path){
        Buffer buf;
        std::string tempstr = "file "+ path +" not exist";
        EncodeMsg(Op::Msg,buf,buf,(char*)tempstr.c_str());
        conn->m_outbuffer.Absorb(buf);
        conn->SendOutput();
}

/*
 *  get the content of cur dir
 */

static bool LsCurDir(std::string& outstr) {
    char p[200];
    if(!getcwd(p,sizeof(p))){
      error("error getcwd" );
    }
    DIR* d = opendir(p);
    struct dirent * e;
    std::stringstream ss;
    while(e=readdir(d)) {
        ss<<(e->d_type == 4 ? "DIR:" : e->d_type == 8 ? "FILE:" : "UNDEF:")<<"\t"<< e->d_name<<"\n";
    }

    outstr = ss.str();
}
/*
 * change current directory to target directory
 */
static  bool ChangeDir(std::string targetDir) {
    if(chdir(targetDir.c_str())==-1) {
        return false;
    }
    return true;
}
/*
 * input curDir
 * output curDir
 * get current directory
 */
static bool PrintCurDir(std::string& curDir) {
    char p[200];
    if(!getcwd(p,sizeof(p))){
      // errno("conn %s print directory error","321");
       return false;
    }
    curDir.clear();
    curDir.append(p,strlen(p));
    return true;
}

