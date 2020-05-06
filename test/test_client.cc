//
// Created by Shinelon on 2020/5/3.
//

#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "buffer.h"
#include "conn.h"

using namespace handy;
using namespace handy::port;

TEST(test::TestBase,Client)
{
    EventBase base;
    TcpConnPtr con = TcpConnection::CreateConnection(&base, "127.0.0.1", 8080, 3000);
    con->SetReadCb([](const TcpConnPtr &con) {
        std::string ss(con->m_inbuffer.Begin(),con->m_inbuffer.Size());
        info("%s",ss.c_str());
        //con->Send(con->m_inbuffer.Data(),con->m_inbuffer.Size());
        con->m_inbuffer.Consume(con->m_inbuffer.Size());}
        );

    char ch[]="hello kill";
    con->Send(ch,6);
    base.Loop();
}