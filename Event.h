#ifndef __EVENT_H__
#define __EVENT_H__

class EventLoop;

typedef void(*EventHandler)(EventLoop*el, void*handler_data);

#define eNONE 0
#define eREADABLE 1
#define eWRITABLE 2

class Event
{
   public:
      
       Event();
      
      ~Event();       

       void rhandle(EventLoop* el);

       void whandle(EventLoop* el);

       void set_rhandler(EventHandler handler);

       EventHandler get_rhandler();       

       void set_whandler(EventHandler handler);

       EventHandler get_whandler();       

       void set_handler_data(void* data);      

       void set_mask(int mask);

       int get_mask();

       int get_fd();

       void set_fd(int);

   private:
      
       EventHandler rhandler_;
      
       EventHandler whandler_;

       void* handler_data_;
       
       int mask_;

       int fd_;
};

#endif