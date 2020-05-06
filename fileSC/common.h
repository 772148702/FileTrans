//
// Created by Shinelon on 2020/5/3.
//

#pragma  once
#include "buffer.h"


enum Op: int {
    FileGet,
    FilePut,
    Msg
};
static void EncodeMsg(Op op,Buffer& inputData, Buffer& resData,char* path=NULL) {
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
static bool  DecodeMsg(Buffer& inputData, int& size,Op& op) {
      int* temp = (int*) inputData.Begin();
      if(*temp>inputData.Size()) return false;
      inputData.PopValue<int>(size);
      inputData.PopValue<Op>(op);
      size = size - 2*sizeof(int);
      return true;
}


