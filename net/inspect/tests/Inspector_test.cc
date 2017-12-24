#include "../Inspector.h"
#include "../../../net/EventLoop.h"
#include "../../../net/EventLoopThread.h"


int main()
{
  EventLoop loop;
  EventLoopThread t;
  Inspector ins(t.startLoop(), InetAddress(12345), "test");
  loop.loop();
}

