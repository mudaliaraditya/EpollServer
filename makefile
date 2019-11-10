


all : server.out client.out

server.out : EpollClassmain.cpp libEpoll.so
	g++ -o server.out EpollClassmain.cpp -L. -lEpoll -Wl,-R. -g 2>CompilerError 

libEpoll.so : EpollClass.cpp
	g++ -o libEpoll.so  -fPIC -shared  EpollClass.cpp -g

client.out : EpollClassclient.cpp
	g++ -o client.out EpollClassclient.cpp

clean :
	rm *.out
	rm *.so
