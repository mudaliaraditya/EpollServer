


all : server.out client.out

server.out : EpollClassmain.cpp 
	g++ -o server.out EpollClassmain.cpp -g 2>CompilerError 

client.out : EpollClassclient.cpp
	g++ client.out EpollClassclient.cpp
