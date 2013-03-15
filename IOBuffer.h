#ifndef __IOBUFFER_H__
#define __IOBUFFER_H__

class IOBuffer
{

public:

	IOBuffer(int fd);

	~IOBuffer();

	int read_to_buffer();

	int write_from_buffer();

    int remain();

private:
	
	int fd_ ;

	char* buff_;

	int ridx_;

	int widx_;

	int buf_size_;
};

#endif