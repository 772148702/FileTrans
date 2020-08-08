//
// Created by Shinelon on 2020/5/3.
//

#ifndef HANDY_FILECLIENT_H
#define HANDY_FILECLIENT_H
#include "conn.h"
#include "common.h"
#include "memory"
using namespace handy;



struct FileClient{
    FileClient(EventBase *base, const std::string &host, unsigned short port);
    void GetInputStart();
    bool SendFileReq(std::string path);
    bool ReceFileReq(std::string path);
    bool LsServerCurrentPath();
    bool CdServerDir(std::string path);
    bool PwdServerDir();
    static void ReceFileRes(TcpConnPtr conn, std::string path,int size);
    static void HandleInput(TcpConnPtr conn);

    std::shared_ptr<TcpConnection> m_pConn;

    EventBase * m_pBase;
};


struct InputCommanderClient {
    std::shared_ptr<std::thread> t1;
    std::shared_ptr<FileClient> fileClient;
    bool Init(EventBase *base, const std::string &host, unsigned short port);
    void InputCommandToClient(EventBase *base, const std::string &host, unsigned short port);
};

#endif //HANDY_FILECLIENT_H
