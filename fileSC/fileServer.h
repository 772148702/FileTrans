//
// Created by Shinelon on 2020/5/3.
//

#ifndef HANDY_FILESERVER_H
#define HANDY_FILESERVER_H

#include "conn.h"
#include "server.h"
#include <memory>
using namespace handy;

struct FileServer: public TcpServer {
    FileServer(EventBases *bases);

    void DecodeMsgServer(TcpConnPtr conn);
    void FilePutRes(TcpConnPtr conn,std::string path,int len);
    void FileGetReq(TcpConnPtr conn,std::string  filename);
    void FileGetRes(TcpConnPtr conn,std::string filename);
    void DirLs(TcpConnPtr conn);
    void SendMessage(TcpConnPtr conn,std::string message);

public:
    static std::shared_ptr<FileServer> StartServer(EventBases *bases, const std::string &host, unsigned short port, bool reusePort=true);
};


#endif //HANDY_FILESERVER_H
