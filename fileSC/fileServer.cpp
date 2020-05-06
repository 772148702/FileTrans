//
// Created by Shinelon on 2020/5/3.
//

#include "fileServer.h"
#include "common.h"
#include "file.h"

void FileServer::DecodeMsgServer(TcpConnPtr conn) {

    int size;
    Op op;
   if(!DecodeMsg(conn->m_inbuffer,size,op) ) {
       return ;
   }
    std::string path;
    switch (op) {
        case Op::FilePut:
            conn->m_inbuffer.PopString(path);
            path = "test";
            FilePutRes(conn,path,size-2*sizeof(int) );

            return;
        case Op::FileGet:
            conn->m_inbuffer.PopString(path);
            FileGetRes(conn,path);

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
    int fileSize = 0;
    Buffer tempBuffer;
    file.Read(tempBuffer,fileSize);

    Buffer * refBuffer =& conn->m_outbuffer;
    //计算包的大小
    int size = 2*sizeof(int)+fileSize;

    refBuffer->AppendValue<int>(size);
    refBuffer->AppendValue<int>(Op::FilePut);
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
