#include "Common.h"

int set_fd_nonblock(int fd)
{
    int flags;
    
    if ((flags = fcntl(fd, F_GETFL)) == -1) {

       log_error("fcntl(F_GETFL): %s", strerror(errno));

       return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {

        log_error("fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));

        return -1;
    }

    return 0;
}

int set_tcp_nodelay(int fd)
{
    int yes = 1;
    
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1)
    {
        log_error("setsockopt TCP_NODELAY: %s", strerror(errno));

        return -1;
    }

    return 0;
}
