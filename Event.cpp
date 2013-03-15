#include <string.h>

#include "Event.h"

using namespace std;

Event::Event()
:rhandler_(NULL),
 whandler_(NULL),
 handler_data_(NULL),
 mask_(eNONE),
 fd_(-1)
{
  
}

Event::~Event()
{
  //the event it self does not has the ownership of the member data.
  //so nothing need to release
}

void Event::set_rhandler(EventHandler handler)
{
   this->rhandler_ = handler;
}

EventHandler Event::get_rhandler()
{
   return this->rhandler_;
}

void Event::set_whandler(EventHandler handler)
{
   this->whandler_ = handler;
}

EventHandler Event::get_whandler()
{
   return this->whandler_;
}

void Event::set_handler_data(void* data)
{
   this->handler_data_ = data;
}

void Event::rhandle(EventLoop* el)
{
   this->rhandler_(el, this->handler_data_);
}

void Event::whandle(EventLoop* el)
{
   this->whandler_(el, this->handler_data_);
}

void Event::set_mask(int mask)
{
   this->mask_ = mask;
}
     
int Event::get_mask()
{
	return this->mask_;
}

int Event::get_fd()
{
   return this->fd_;
}

void Event::set_fd(int fd)
{
   this->fd_ = fd;
}