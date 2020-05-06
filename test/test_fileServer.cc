//
// Created by Shinelon on 2020/5/3.
//

#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "conn.h"
#include "server.h"
#include "fileSC/fileServer.h"
using namespace handy;
using namespace handy::port;


TEST(test::TestBase,FileServer)
{
    MultiBase base(4);
    std::shared_ptr<FileServer> svr = FileServer::StartServer(&base, "", 8080);
    exitif(svr == NULL, "start tcp server failed");
    svr->SetConnectionCb([](const TcpConnPtr &con) {
        info("new connection %s",con->m_peer.ToString().c_str());
    });
    svr->SetDisconnectionCb([](const TcpConnPtr &con) {
        info("disconnected %s",con->m_peer.ToString().c_str());
    });
    svr->SetWriteCb([](const TcpConnPtr &con) {
        info("wirte complete!");
    });
    svr->SetReadCb([&](const TcpConnPtr &con) {
        svr->DecodeMsgServer(con);
    });

    base.Loop();

}