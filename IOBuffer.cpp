#include <string.h>
#include <unistd.h>

#include "IOBuffer.h"

IOBuffer::IOBuffer(int fd)
:fd_(fd),
 buff_(new char[1024]),
 ridx_(0),
 widx_(0),
 buf_size_(1024)
{
  
}

IOBuffer::~IOBuffer()
{
  delete [] buff_;
}

int IOBuffer::read_to_buffer()
{
    char tmpbuff[5000] = {0};
    
    int nread = 0 ;  

    int nread_total = 0 ;

    while((nread = read(this->fd_, tmpbuff, 5000)) > 0)
    {
       nread_total += nread;

       int tail_cap = this->buf_size_ - 1 - this->ridx_;

       int head_cap = this->widx_;

       if(tail_cap >= nread )
       {
       	 memcpy(buff_+ridx_, tmpbuff, nread);  

       	 this->ridx_ += nread;
       }
       else if( (head_cap + tail_cap) >= nread)
       {// Use all the cap can handle the data

       	 //1st move the data
       	 int curr_size = this->ridx_ - this->widx_;
         memmove(buff_, buff_+ridx_, curr_size); 

         //2nd copy the data
         memcpy(buff_+curr_size,tmpbuff, nread);

         //3rd update the idx
         this->ridx_ = curr_size + nread;
         this->widx_ = 0;
       }
       else
       {// need to relocate the memory

       	 int curr_size = this->ridx_ - this->widx_;

       	 //double the buffer
         int total = 2*(nread+curr_size);

         char* bf = new char[total];

         memmove(bf,buff_+ridx_, curr_size);

         memcpy(bf+curr_size,tmpbuff, nread);

         this->buf_size_ = total;
         
         delete this->buff_;

         this->buff_ = bf;
         
         this->ridx_ = curr_size + nread;
         this->widx_ = 0;
       }       
    }
    
    if(nread < 0)
    {
       return nread;
    }
    
    return nread_total;
}

int IOBuffer::write_from_buffer()
{
	int nwrite = 0;
	int nwrite_total = 0;

    while((this->remain()>0)&&
    	  (nwrite = write(this->fd_, this->buff_ + this->widx_, this->remain())) > 0)
    {
       nwrite_total += nwrite; 
       this->widx_ += nwrite;
    }
    
    if(nwrite < 0)
    {
    	return nwrite;
    }
    
    return nwrite_total;
}

int IOBuffer::remain()
{
	return this->ridx_ - this->widx_;
}