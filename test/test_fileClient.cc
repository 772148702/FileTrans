//
// Created by Shinelon on 2020/5/5.
//
#include "test_harness.h"

#include "conn.h"


#include "fileSC/fileClient.h"
using namespace handy;
using namespace handy::port;


TEST(test::TestBase,FileClient)
{
        EventBase base;
        FileClient client(&base,"127.0.0.1",8080);
        //client.GetInputStart();
        client.SendFile("CMakeCache.txt");
        base.Loop();

}