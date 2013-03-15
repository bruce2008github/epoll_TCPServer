#include <string.h>
#include "Connctx.h"

using namespace std;

Connctx::Connctx(int fd)
:fd_(fd),
 iobuf_(new IOBuffer(fd))
{

}

Connctx::~Connctx()
{
	delete iobuf_;

	iobuf_ = NULL;
}

int Connctx::get_fd()
{
	return this->fd_;
}

IOBuffer* Connctx::get_buf()
{
	return this->iobuf_;
}