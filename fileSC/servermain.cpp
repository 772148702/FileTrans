//
// Created by Shinelon on 2020/5/5.
//


#include "fileServer.h"
#include <memory>
int main() {
    MultiBase multiBase(4);
    std::shared_ptr<FileServer> fileServerPtr = FileServer::StartServer(&multiBase,"",8080);
    fileServerPtr->SetReadCb([](const TcpConnPtr &con) {
        std::string ss(con->m_inbuffer.Begin(),con->m_inbuffer.Size());
        info("%s",ss.c_str());
        con->Send(con->m_inbuffer.Data(),con->m_inbuffer.Size());
        con->m_inbuffer.Consume(con->m_inbuffer.Size());
    });
    multiBase.Loop();
}