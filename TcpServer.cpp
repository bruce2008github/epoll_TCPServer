#include <errno.h>
#include <string.h>

#include "TcpServer.h"
#include "Common.h"
#include "IOBuffer.h"

using namespace std;

TcpServer::TcpServer(int port)
:event_loop_(new EventLoop()),
 acceptor_(new Acceptor(port,TcpServer::new_conn))
{
   
}

TcpServer::~TcpServer()
{
  delete acceptor_;
  acceptor_ = NULL;

  delete event_loop_;
  event_loop_ = NULL;
}

void TcpServer::start()
{
    int lfd = this->acceptor_->listen();    

    this->event_loop_->set_fd_event(lfd,eREADABLE,
                                     Acceptor::accept_handler,
                                     (void*)this->acceptor_);

    this->event_loop_->run();
}

void TcpServer::new_conn(EventLoop*el, Connctx* connctx)
{
    int fd = connctx->get_fd();

    if(0 != set_fd_nonblock(fd))
    {
    	return ;
    }
    	    
    if(0 != set_tcp_nodelay(fd))
    {
    	return ;
    }   

    el->set_fd_event(fd, eREADABLE, TcpServer::on_msg, (void*)connctx);    
}

void TcpServer::del_conn(EventLoop*el, Connctx* connctx)
{
    //printf("release conn\n");

	int fd = connctx->get_fd();    

    el->release_fd_event(fd, eREADABLE);
    
    el->release_fd_event(fd, eWRITABLE);

    close(fd);

    delete connctx;
    connctx = NULL;
}

void TcpServer::shutdown_conn(EventLoop*el, Connctx*connctx)
{ 
   //we make the current conn can not be write
   //and continue make the fd readable
   //so that can make sure that we will NOT loss the data on wire
   int fd = connctx->get_fd();

   Event* event = el->get_event(fd);
   
   if((event->get_mask()& eWRITABLE) !=0)
   {
     el->release_fd_event(fd, eWRITABLE); 

     shutdown(fd, SHUT_WR);
   }   
}

void TcpServer::on_msg(EventLoop*el, void* ctx )
{
    Connctx* connctx =(Connctx*)ctx;	
    
    IOBuffer* buf = connctx->get_buf();

    int nread = buf->read_to_buffer();

    if (nread == -1) 
    {
        if (errno == EAGAIN) 
        {
            nread = 0;
        } 
        else 
        {                 
            del_conn(el, connctx);

            return;
        }
    } 
    else if (nread == 0) 
    {     
        del_conn(el, connctx);
        return;
    }
    
    //try best to write
    buf->write_from_buffer();   

    int fd = connctx->get_fd(); 
    
    if(buf->remain() > 0)
    {//need to write again when th fd is writable         

       Event* event = el->get_event(fd);

       int mask = event->get_mask();

       if(0 == (mask & eWRITABLE))
       {
           el->set_fd_event(fd, eWRITABLE, TcpServer::on_can_write, (void*)connctx); 
       }      
    } 
    else
    {
       el->release_fd_event(fd, eWRITABLE);  
    }
}

void TcpServer::on_can_write(EventLoop*el, void* ctx )
{
    Connctx* connctx =(Connctx*)ctx;
    
    int fd = connctx->get_fd();   
    
    IOBuffer* buf = connctx->get_buf();

    int nwrite = buf->write_from_buffer();  

    if(nwrite < 0)
    {
        shutdown_conn(el, connctx);
        return;
    }    

    if(0 == buf->remain())
    {
       el->release_fd_event(fd, eWRITABLE);         
    }
}