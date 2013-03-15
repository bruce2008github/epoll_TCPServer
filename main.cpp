#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "TcpServer.h"

using namespace std;


int main()
{
	printf("pid =%d\n", getpid());

	int port = 2255;

	TcpServer server(port);

	server.start();

	return 0;
}
