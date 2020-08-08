//
// Created by Shinelon on 2020/8/8.
//
#include <fileSC/common.h>
#include "handy/net.h"
#include "handy/logging.h"
#include "port_posix.h"
#include "buffer.h"
#include "fileSC/fileClient.h"
int main() {
    EventBase base;

    InputCommanderClient client;
    //client.Init(&base,"127.0.0.1",8081);
    client.InputCommandToClient(&base,"127.0.0.1",8081);
}