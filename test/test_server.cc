//
// Created by Shinelon on 2020/4/23.
//
#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "conn.h"
#include "server.h"
using namespace handy;
using namespace handy::port;


TEST(test::TestBase,Server)
{
    EventBase base;
    TcpServerPtr svr = TcpServer::StartServer(&base, "", 8080);
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
    svr->SetReadCb([](const TcpConnPtr &con) {

        info("%s",con->m_inbuffer.Data());
        con->Send(con->m_inbuffer.Data(),con->m_inbuffer.Size());
        con->m_inbuffer.Consume(con->m_inbuffer.Size());
    });

    base.Loop();

}