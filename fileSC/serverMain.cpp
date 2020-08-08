//
// Created by Shinelon on 2020/5/5.
//


#include "fileServer.h"
#include <memory>
int main() {
    MultiBase base(4);
    std::shared_ptr<FileServer> svr = FileServer::StartServer(&base, "", 8081);
    exitif(svr == NULL, "start tcp server failed");
    svr->SetConnectionCb([](const TcpConnPtr &con) {
        info("new connection %s",con->m_peer.ToString().c_str());
    });
    svr->SetDisconnectionCb([](const TcpConnPtr &con) {
        info("disconnected %s",con->m_peer.ToString().c_str());
    });
    svr->SetWriteCb([](const TcpConnPtr &con) {
        con->SendOutput();
        info("wirte event triggered!");
    });
    svr->SetReadCb([&](const TcpConnPtr &con) {
        svr->DecodeMsgServer(con);
    });
    base.Loop();
}