#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Common.h"

class EventLoop;

class Connctx;

typedef void (*NewConnHandler)(EventLoop* el, Connctx* connctx);

class Acceptor
{
  public:
   
   Acceptor(int port, NewConnHandler newconn_handler);
   
  ~Acceptor();

   int listen();   

   Connctx* acceptfd();

   static void accept_handler(EventLoop *el, void *acceptor);

  private:
    
    int acceptfd(int listen_fd, struct sockaddr *sa, socklen_t* len);

    int port_;

  	NewConnHandler newconn_handler_;

  	int listen_fd_;

    //the magic 511 constant is from nginx 
  	enum {eBackLog = 511 };

};


#endif