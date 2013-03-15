#ifndef __CONNCTX_H__
#define __CONNCTX_H__

#include "IOBuffer.h"

class Connctx
{
   public:
   	  
   	  Connctx(int fd);
     
     ~Connctx();

      int get_fd();

      IOBuffer* get_buf();

   private:

   	 int fd_ ;

   	 IOBuffer* iobuf_;
};

#endif