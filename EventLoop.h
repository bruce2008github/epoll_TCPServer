#ifndef __EVENTLOOP_H__
#define __EVENTLOOP_H__

#include <sys/epoll.h>

#include "Event.h"

class EventLoop
{
  public:

  	 EventLoop();

  	~EventLoop();
     
     bool update_poller(Event*event,int mask);     

     bool set_fd_event(int fd, int mask, EventHandler handler, void* handler_data);

     void release_fd_event(int fd, int delmask);

     Event* get_event(int fd);    

     int poll();

     void process_events();

     void run();

  private:

  	int epoller_fd_;
     
    enum {eWATCH_EVENT_SIZE=1024*10};

  	Event watch_events_[eWATCH_EVENT_SIZE];

    struct epoll_event tmp_events_[eWATCH_EVENT_SIZE];

    struct FiredEvent {
        int fd;
        int mask;
    };
    
    FiredEvent fired_[eWATCH_EVENT_SIZE];

    bool stop_ ;
    
};

#endif