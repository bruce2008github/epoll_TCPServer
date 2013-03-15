Note:

This tiny tcp servers implemented by c++ and use non-blocking IO and epoll(supported by Linux).

This is just a prototype framework, currently support echo function as an example.

*************************************************************************************
I know the smart pointer is good thing, but in this framework I prefer to use the raw pointer.
That's because the C++0x11 is not very popular currently, and I do NOT want to introduce the 
unnecessary dependency, I avoid to use smart pointer and others 3rd lib(such as boost)
**************************************************************************************

It can handle thousands client tcp connections at same time.

And I also write a echo testing tool---echoStressTest.erl by Erlang.

I use echoStressTest.erl to simulate thousonds clients to connect this echo server.

Build:

1) Make

Usage:

1) run ./main (listen on the 2255 port)

2) run your client(such as telnet)to access the echo server.

Stress Testing:

1) Make sure you have installed the erlang(http://www.erlang.org/)

2) execute command: "./echoStressTest.erl -h" to see the usage info

3) If you need to run a lot of concurrent connections testing, 

   Please tune your linux server at first, and check the system limit(ulimit -a)