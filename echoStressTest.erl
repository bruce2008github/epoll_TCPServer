#!/usr/bin/env escript

getCSOCK_OPTS() -> [binary, {packet, 0}, {nodelay, true}].	

getTestData() ->
    Data = <<"abcdefghijk">>,
    {byte_size(Data), Data}.

main([SIP,SPort,CNum,CPingNum]) ->   
    {ok, IP} = inet_parse:ipv4_address(SIP),
    Port = list_to_integer(SPort),
    Num = list_to_integer(CNum),
    PingNum = list_to_integer(CPingNum),
    FPid = self(),
    ChildArray = [],
    forkAllClients(FPid, IP,Port,Num,Num,PingNum,ChildArray),
    waitChild(Num,0,0);

main(_) -> usage().

usage()->
   io:format("~nUsage: ./echoStressTest.erl <dest addr> <dest port> <client num> <ping round num>~n"),
   io:format("~nExample:~n"),
   io:format("   ./echoStressTest.erl 127.0.0.1 6678 223 1900~n~n").

waitChild(0,DoneNum,ErrorNum)->
   io:format("*** waitChild DoneNum: ~p~n", [DoneNum]),
   io:format("*** waitChild ErrorNum: ~p~n", [ErrorNum]);

waitChild(Num,DoneNum,ErrorNum)->
   io:format("*** Work process Done number: ~p~n", [DoneNum]),
   receive
   	  {done}->
   	     waitChild(Num-1,DoneNum+1,ErrorNum);
   	  {error}->
   	     waitChild(Num-1,DoneNum,ErrorNum+1);
   	  Other->
   	     io:format("*** waitChild Invalid message: ~p~n", [Other]),
   	     waitChild(Num,DoneNum,ErrorNum)
   end.


forkAllClients(_,_,_,0,Num,_,ChildArray)->
    io:format("The ~p clients forked!~n", [Num]),
    lists:foreach( fun(Pid)-> Pid ! {go} end, ChildArray),
    io:format("Begin to wait for all the process work is done !~n");

forkAllClients(FPid,IP,Port,Remain,Total,PingNum,ChildArray)->
   
    Pid = spawn(
	    fun	() ->
	    	 receive
	    	 	{go}->
	    	 	  {ok, US} = gen_tcp:connect(IP, Port, [{active, true} | getCSOCK_OPTS()]),
                  client_loop(FPid,US,PingNum)
             end
	    end
	   ),
    forkAllClients(FPid,IP,Port,Remain-1,Total,PingNum,[Pid|ChildArray]).

client_loop(FPid,US,0)->  
    gen_tcp:close(US),
    FPid ! {done};

client_loop(FPid,US,PingNum)->    
    
    {DataSize,BinData} = getTestData(),
    
    ok = gen_tcp:send(US, BinData),
    
    client_loop(FPid,US,DataSize,DataSize),
    
    client_loop(FPid,US,PingNum-1).   
    
  
client_loop(_FPid,_US,0,_DataSize)->
    ok;
client_loop(FPid,US,_UnReceived,DataSize)->
    receive
		{tcp, US, Data} ->		 
		  %%io:format("received ~p~n",[Data]),
		  %%io:format("The unreceived:~p~n",[DataSize-byte_size(Data)]),
		  client_loop(FPid,US,DataSize-byte_size(Data),DataSize);
		Other ->
		  io:format("*** Invalid message: ~p~n", [Other]),
		  FPid ! {error}
	after 1000 *10 ->
	      FPid ! {error}
	end.

