#include "Acceptor.h"
#include "Connctx.h"


using namespace std;

Acceptor::Acceptor(int port, NewConnHandler newconn_handler)
 :port_(port),
 newconn_handler_(newconn_handler),
 listen_fd_(-1)
{
  
}

Acceptor::~Acceptor()
{

}

int Acceptor::listen()
{
    int s, on = 1;
    struct sockaddr_in sa;
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    {
        log_error("creating socket: %s", strerror(errno));
        return -1;
    }
    
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) 
    {
        log_error("setsockopt SO_REUSEADDR: %s", strerror(errno));
        return -1;
    }    

    memset(&sa,0,sizeof(sa));

    sa.sin_family = AF_INET;

    sa.sin_port = htons(this->port_);

    sa.sin_addr.s_addr = htonl(INADDR_ANY);    

    if (bind(s,(struct sockaddr*)&sa,sizeof(sa)) == -1) {

        log_error("bind: %s", strerror(errno));

        close(s);

        return -1;
    }

    if (::listen(s, eBackLog) == -1) {

        log_error("listen: %s", strerror(errno));

        close(s);

        return -1;
    }
  
    this->listen_fd_ = s;

    return s;
}

int Acceptor::acceptfd(int listen_fd, struct sockaddr *sa, socklen_t* len)
{
	int fd = -1;
    
    bool flag = true;

    while(flag) {

        fd = accept(listen_fd,sa,len);

        if (fd == -1) {

            if (errno == EINTR)
            {                
                flag = false;
                continue;
            }
            else {
                log_error("accept: %s", strerror(errno));
                return -1;
            }
        }
        
        break;
    }

    return fd;
}

Connctx* Acceptor::acceptfd()
{
    struct sockaddr_in sa;

    socklen_t salen = sizeof(sa);
    
    int newconn_fd = -1;   
    

   if((newconn_fd = this->acceptfd(this->listen_fd_,(struct sockaddr*)&sa,&salen)) < 0)
   {      
      return NULL;
   }

   //conn will be released when the work is done or some error's happen.
   Connctx* newctx = new Connctx(newconn_fd);

   return newctx;
}

void Acceptor::accept_handler(EventLoop *el, void *acceptor)
{   
    Connctx* ctx = ((Acceptor*)acceptor)->acceptfd();

    if(ctx == NULL) 
    {
       return;
    }
    
   ((Acceptor*)acceptor)->newconn_handler_(el,ctx);
}

