//
// Created by Shinelon on 2020/5/3.
//

#include <fileSC/file.h>
#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "buffer.h"
#include "conn.h"

using namespace handy;
using namespace handy::port;

TEST(test::TestBase,File)
{
    File file("testfile","wb+");
    char buf[4]="123";
    Buffer buffer;
    buffer.Append(buf,sizeof(buf));
    file.Write(buffer,buffer.Size());
    buffer.Clear();
    buffer.AppendValue<int>(123);

    file.Write(buffer,buffer.Size());
    Buffer buffer2;
    buffer2.Append("nihao 123");
    file.Write(buffer2,buffer2.Size());
    file.Close();

    File fil2("testfile","rb");
    Buffer buffer1;
    int size;
    fil2.Read(buffer1,size);
    fil2.Close();
    info("buffer1: %s %d",buffer1.Data(),size);
}