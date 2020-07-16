


all :  bin/libEpoll.so  bin/server.out bin/client.out bin/libEpoll.a

bin/server.out : bin/libEpoll.so bin/libEpoll.a example/EpollClassmain.cpp
	g++ -o example/bin/server.out example/EpollClassmain.cpp -Ihdr -Lbin -lEpoll -Wl,-R. -g
	cp bin/libEpoll.so example/bin/.

bin/libEpoll.so : src/EpollClass.cpp
	g++ -o bin/libEpoll.so  -fPIC -shared  src/EpollClass.cpp -Ihdr -g 

bin/libEpoll.a : src/EpollClass.cpp
	g++ -c  src/EpollClass.cpp -o bin/EpollClass.o -Ihdr -g
	ar rcs  bin/libEpoll-static.a bin/EpollClass.o
	rm bin/EpollClass.o


bin/client.out : example/EpollClassclient.cpp
	g++ -o example/bin/client.out example/EpollClassclient.cpp -Ihdr  2>>CompilerError

clean :
	rm bin/*.out || true
	rm bin/*.so || true
	rm CompilerError
	touch CompilerError
