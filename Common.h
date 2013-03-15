#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#define log_error(fmtstr, str) printf("%s:%s:%d: ",__FILE__, __FUNCTION__,__LINE__);printf(fmtstr,str);printf("\n")

int set_fd_nonblock(int fd);

int set_tcp_nodelay(int fd);


#endif