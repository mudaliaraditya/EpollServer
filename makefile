


all :  bin/libEpoll.so  bin/server.out bin/client.out 

bin/server.out : src/EpollClassmain.cpp
	g++ -o bin/server.out src/EpollClassmain.cpp -Ihdr -Lbin -lEpoll -Wl,-R. -g 2>>CompilerError 

bin/libEpoll.so : src/EpollClass.cpp
	g++ -o bin/libEpoll.so  -fPIC -shared  src/EpollClass.cpp -Ihdr -g 2>>CompilerError

bin/client.out : src/EpollClassclient.cpp
	g++ -o bin/client.out src/EpollClassclient.cpp -Ihdr  2>>CompilerError

clean :
	rm bin/*.out || true
	rm bin/*.so || true
	rm CompilerError
	touch CompilerError
