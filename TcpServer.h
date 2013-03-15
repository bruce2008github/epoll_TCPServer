#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include <netinet/in.h>
#include <netinet/tcp.h>

#include "Connctx.h"
#include "EventLoop.h"
#include "Acceptor.h"

class TcpServer
{
   
  public:

   TcpServer(int port);

   ~TcpServer();

   void start();

   //the following are call back function by eventloop
   
   static void new_conn(EventLoop*el, Connctx* connctx);

   static void del_conn(EventLoop*el, Connctx* connctx);

   static void on_msg(EventLoop*el, void* connctx);

   static void on_can_write(EventLoop*el, void* connctx);

   static void shutdown_conn(EventLoop*el, Connctx*connctx);

  private:
    
    EventLoop* event_loop_;

    Acceptor*  acceptor_;

};

#endif
