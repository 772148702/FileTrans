//
// Created by Shinelon on 2020/8/5.
//

#include <fileSC/common.h>
#include "test_harness.h"
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "buffer.h"
#include "fileSC/fileClient.h"
TEST(test::TestBase,InputCommanderClient) {
    EventBase base;

    InputCommanderClient client;
    //client.Init(&base,"127.0.0.1",8081);
    client.InputCommandToClient(&base,"127.0.0.1",8081);
}