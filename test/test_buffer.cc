//
// Created by Shinelon on 2020/4/17.
//




#include <fileSC/common.h>
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

TEST(test::TestBase,Buffer3)
{
    //2*4+13=21
    Buffer buf;
    int size = 2*sizeof(int);
    char * ch = "hello buffer!";

    size+=strlen(ch);
    size = 48126;
    buf.AppendValue<int>(size);
    buf.AppendValue<int>(Op::FilePut);
    buf.Append(ch,strlen(ch));
    info("size of Op %d",sizeof(Op));
    int* temp = (int*) buf.Begin();
    ASSERT_EQ(*temp,size);
    int k;
    buf.PopValue<int>(k);
    info("buffer Popvalue %d",k);

    Op op;
    buf.PopValue<Op>(op);
    info("buffer Popvalue %d",op);
    std::string str(buf.Begin(),size-sizeof(int)*2);
    info("data: %s!",str.c_str());
}

TEST(test::TestBase,Buffer4) {
    Buffer buf;
    std::string str={"32   1"};
    int size =  str.size();
    buf.AppendValue<int>(size);
    buf.Append(str.c_str());
    std::string pp;
    int i;
    buf.PopString(pp,i);
    info("%s",pp.c_str());
}