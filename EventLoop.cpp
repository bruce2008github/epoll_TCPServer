#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>

#include "EventLoop.h"
#include "Common.h"

using namespace std;

EventLoop::EventLoop()
: epoller_fd_(epoll_create(1024)),
  stop_(false)
{

}

EventLoop::~EventLoop()
{
    close(epoller_fd_);
}

Event* EventLoop::get_event(int fd)
{
   if(fd >= eWATCH_EVENT_SIZE) 
        return NULL;

   Event * event = &this->watch_events_[fd]; 

   return event;
}

bool EventLoop::update_poller(Event*event,int emask)
{
    struct epoll_event ee;

    int op = event->get_mask() == eNONE ?
            EPOLL_CTL_ADD : EPOLL_CTL_MOD;

    //printf("EPOLL_CTL_ADD=%d, EPOLL_CTL_MOD=%d\n",EPOLL_CTL_ADD,EPOLL_CTL_MOD);

    ee.events = 0;
    
    int mask = emask;

    mask |= event->get_mask(); 

    if (mask & eREADABLE) ee.events |= EPOLLIN;
    if (mask & eWRITABLE) ee.events |= EPOLLOUT;

    ee.data.u64 = 0; 
    
    int fd = event->get_fd();
    ee.data.fd = fd;
    
    //printf("epoll_ctl epoller_fd_=%d, op=%d, fd=%d\n", epoller_fd_,op,fd);

    if (epoll_ctl(this->epoller_fd_,op,fd,&ee) == -1) 
    {
        log_error("epoll_ctl error: %s", strerror(errno));
        return false;
    }
    	
    return true;         
}

bool EventLoop::set_fd_event(int fd, int mask, EventHandler handler, void* handler_data)
{
    if (fd >= eWATCH_EVENT_SIZE) 
        return false;

    Event * event = &this->watch_events_[fd];   
    
    //printf("set_fd fd=%d\n", fd);
    event->set_fd(fd);
    
    if(!update_poller(event,mask))
    {
       log_error("%s\n","error update_poller");
       return false;
    }

    event->set_mask(event->get_mask()|mask);
   
    event->set_handler_data(handler_data);

    if(mask & eREADABLE)
    {
       //printf("set_rhandler\n");
       event->set_rhandler(handler);
    }
      
    if(mask & eWRITABLE)
    {
       //printf("set_whandler\n");
       event->set_whandler(handler);    
    }      

    return true;
}

void EventLoop::release_fd_event(int fd, int delmask)
{
    if (fd >= eWATCH_EVENT_SIZE) 
        return;

    Event *event = &this->watch_events_[fd];
    
    //printf("event mask =%d\n", event->get_mask());

    if (event->get_mask() == eNONE) 
        return;

    int smask = event->get_mask() & (~delmask);

    event->set_mask(smask);

    struct epoll_event ee;
    int mask = event->get_mask();

    ee.events = 0;

    if (mask & eREADABLE) 
        ee.events |= EPOLLIN;

    if (mask & eWRITABLE)
        ee.events |= EPOLLOUT;

    ee.data.u64 = 0; 
    ee.data.fd = fd;

    if (mask != eNONE) 
    {       
      if (epoll_ctl(this->epoller_fd_,EPOLL_CTL_MOD,fd,&ee) == -1) 
      {
          log_error("epoll_ctl error: %s", strerror(errno));
          return ;         
      }
    }
    else 
    {       
      if (epoll_ctl(epoller_fd_,EPOLL_CTL_DEL,fd,&ee) == -1) 
      {
          log_error("epoll_ctl error: %s", strerror(errno));
          return ;         
      }     
    }
}

int EventLoop::poll()
{      
    int numevents = epoll_wait(this->epoller_fd_,this->tmp_events_,eWATCH_EVENT_SIZE,-1);

    if (numevents > 0) {
        
        for (int i = 0; i < numevents; i++) {

            int mask = 0;

            struct epoll_event *e = this->tmp_events_+i;

            if (e->events & EPOLLIN)  mask |= eREADABLE;
            if (e->events & EPOLLOUT) mask |= eWRITABLE;
            if (e->events & EPOLLERR) mask |= eWRITABLE;
            if (e->events & EPOLLHUP) mask |= eWRITABLE;

            this->fired_[i].fd = e->data.fd;

            this->fired_[i].mask = mask;
        }
    }

    return numevents;
}

void EventLoop::process_events()
{
    int numevents = this->poll();

    for (int i = 0; i < numevents; ++i) {
       
        //printf(" event %d \n", i);

        Event * event = &this->watch_events_[this->fired_[i].fd];

        int mask = this->fired_[i].mask;        

        int rfired = 0;

        /* note the fe->mask & mask & ... code: maybe an already processed
         * event removed an element that fired and we still didn't
         * processed, so we check if the event is still valid. */
        if (event->get_mask() & mask & eREADABLE) {
            rfired = 1;
            event->rhandle(this);
        }

        if (event->get_mask() & mask & eWRITABLE) {
            if (!rfired || event->get_rhandler() != event->get_whandler())
            event->whandle(this);
        }       
    }
}

void EventLoop::run()
{
     while (!this->stop_) 
     {        
        this->process_events();
     }
}