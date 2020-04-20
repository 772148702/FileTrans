//
// Created by Shinelon on 2020/4/17.
//




#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "buffer.h"


using namespace handy;
using namespace handy::port;



//测试端口是否阻塞
TEST(test::TestBase,Buffer)
{
   std::string s = "test Buffer";
   Buffer buf;

   //测试Append
   buf.Append(s.c_str());
   info("%s",buf.ToString().c_str());

   //测试添加超过512时，容量是否翻倍
   std::string ss(600,'a');
   buf.Append(ss.c_str());
   info("%s",buf.ToString().c_str());

   //测试Consume
   buf.Consume(500);
   info("%s",buf.ToString().c_str());

   Buffer buf2;
   buf2.Absorb(buf);
   info("%s",buf.ToString().c_str());
   info("%s",buf2.ToString().c_str());
}

TEST(test::TestBase,Buffer2)
{
    std::string s = "test Buffer";
    Buffer buf;
    buf.Append(s.c_str());
    buf.Consume(500);
    info("%s",buf.ToString().c_str());
}