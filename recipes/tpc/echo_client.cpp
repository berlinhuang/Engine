//
// Created by root on 18-8-1.
//
#include <stdio.h>
#include <cstdlib>
#include "InetAddress.h"


int main(int argc, const char* argv[])
{
    if(argc < 3)
    {
        printf("Usage:%s hostname message_length [scp]\n", argv[0]);
        return 0;
    }
    const int len = atoi(argv[2]);
    InetAddress addr(3007);
    if(!InetAddress::resolve(argv[1],&addr))
    {
        printf("Unables to resolve %s\n", argv[1]);
        return 0;
    }

    printf("connecting to %s\n",addr.toIpPort().c_str());



}