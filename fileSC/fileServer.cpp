//
// Created by Shinelon on 2020/5/3.
//

#include <dirent.h>
#include "fileServer.h"
#include "common.h"
#include "file.h"

void FileServer::DecodeMsgServer(TcpConnPtr conn) {

    int size;
    Op op;
    std::string fileName;
   if(!DecodeMsg(conn->m_inbuffer,size,op,fileName)) {
       return ;
   }
    std::string path;

    switch (op) {
        case Op::FilePut:
            FilePutRes(conn,fileName,size);
            return;
        case Op::FileGet:
            FileGetRes(conn,fileName);
        case Op::Cd:
            conn->m_inbuffer.PopStringWithoutDataLength(path,size);
            if(ChangeDir(path)) {
                std::string message = "successful changed!";
                SendMessage(conn,message);
            } else {
                std::string message = "changed failed please check target path!";
                SendMessage(conn,message);
            }
        case Op::Msg:
            conn->m_inbuffer.PopStringWithoutDataLength(path,size);
            info("receive msg %s",path.c_str());
            break;
        case Op::FileLs:
            DirLs(conn);
            info("receive op dirls");
        case Op::Pwd:
            PrintCurDir(path);
            SendMessage(conn,path);
        default:
            return;
    }

}

//响应上传文件到服务器的请求
void FileServer::FilePutRes(TcpConnPtr conn,std::string path,int len) {
    File file((char*)path.c_str(),"wb");
    file.Write(conn->m_inbuffer,len);
    file.Close();
    conn->m_inbuffer.Consume(len);

    //response
    Buffer tempBuffer;
    int size = 2*sizeof(int);
    char* msg = "Save file successful";
    size+=strlen(msg);
    tempBuffer.AppendValue<int>(size);
    tempBuffer.AppendValue<int>(Op::Msg);
    tempBuffer.Append(msg,strlen(msg));
    //发送响应
    conn->SendOutput();
}

void FileServer::FileGetRes(TcpConnPtr conn, std::string filename) {

    File file((char*)filename.c_str(),"rb");
    if(!file.IsExist()) {
        FileNotExist(conn,filename);
        return;
    }
    int fileSize = 0;
    Buffer tempBuffer;
    file.Read(tempBuffer,fileSize);

    Buffer * refBuffer =& conn->m_outbuffer;
    //计算包的大小
    int size = 3*sizeof(int)+filename.size()+fileSize;

    refBuffer->AppendValue<int>(size);
    refBuffer->AppendValue<int>(Op::FileGet);
    refBuffer->AppendValue<int>(filename.size());
    refBuffer->Append(filename.c_str());
    refBuffer->Absorb(tempBuffer);
    //发送数据
    conn->SendOutput();
}

FileServer::FileServer(EventBases *bases) : TcpServer(bases) {
}

std::shared_ptr<FileServer>
FileServer::StartServer(EventBases *bases, const std::string &host, unsigned short port, bool reusePort) {
   std::shared_ptr<FileServer> p(new FileServer(bases));
    int r = p->Bind(host,port,reusePort);
    if(r) {
        error("bind to %s:%d failed %d %s", host.c_str(), port, errno, strerror(errno));
    }
    return r == 0 ? p : NULL;
}

void FileServer::DirLs(TcpConnPtr conn) {
     std::string results;
     LsCurDir(results);
     Buffer tempBuffer,outputBuffer;
     tempBuffer.Append(results.c_str(),results.length());
     EncodeMsg(FileLs,tempBuffer,outputBuffer);
     conn->m_outbuffer.Absorb(outputBuffer);
     conn->SendOutput();
}

void FileServer::SendMessage(TcpConnPtr conn, std::string message) {
    Buffer tempBuffer;
    tempBuffer.Append(message.c_str(),message.size());
    EncodeMsg(Op::Msg,tempBuffer,conn->m_outbuffer);
    conn->SendOutput();
}



