
CXX=g++
Lib=lib
MAIN_TARGET=Epoll
MULTITHREAD_TARGET_SUFFIX=MulThread
SINGLETHREAD_TARGET_SUFFIX=SnglThread
STATIC_NAME=.a
DYNAMIC_NAME=.so

MULTITHREAD_LIB_DYN_EPOLL=${Lib}${MAIN_TARGET}-${MULTITHREAD_TARGET_SUFFIX}${DYNAMIC_NAME}
MULTITHREAD_LIB_DYN_EPOLL_LINK=${MAIN_TARGET}-${MULTITHREAD_TARGET_SUFFIX}
SINGLETHREAD_LIB_DYN_EPOLL=${Lib}${MAIN_TARGET}-${SINGLETHREAD_TARGET_SUFFIX}${DYNAMIC_NAME}
SINGLETHREAD_LIB_DYN_EPOLL_LINK=-l${MAIN_TARGET}-${SINGLETHREAD_TARGET_SUFFIX}

MULTITHREAD_LIB_STA_EPOLL=${MAIN_TARGET}-${MULTITHREAD_TARGET_SUFFIX}${STATIC_NAME}
SINGLETHREAD_LIB_STA_EPOLL=${MAIN_TARGET}-${SINGLETHREAD_TARGET_SUFFIX}${STATIC_NAME}

LINKING_LIB=${MULTITHREAD_LIB_DYN_EPOLL_LINK}
DYNAMIC_LIB=${MULTITHREAD_LIB_DYN_EPOLL}
STATIC_LIB=${MULTITHREAD_LIB_STA_EPOLL}

all :  bin/${MULTITHREAD_LIB_DYN_EPOLL} bin/${SINGLETHREAD_LIB_DYN_EPOLL} bin/lib${MULTITHREAD_LIB_STA_EPOLL} bin/lib${SINGLETHREAD_LIB_STA_EPOLL}   example/Example1/bin/server.out example/Example1/bin/client.out   example/Example2/bin/server.out  example/Example2/bin/client.out bin/libEpoll.so

bin/libEpoll.so : bin/lib${MULTITHREAD_LIB_STA_EPOLL}
	cp bin/lib${MULTITHREAD_LIB_DYN_EPOLL_LINK}.so $@


example/Example1/bin/server.out : bin/${MULTITHREAD_LIB_DYN_EPOLL} bin/lib${STATIC_LIB} example/Example1/EpollClassServer.cpp
	${CXX} -o example/Example1/bin/server.out example/Example1/EpollClassServer.cpp -Ihdr -Lbin -l${LINKING_LIB} -Wl,-R. -g
	cp bin/${DYNAMIC_LIB} example/Example1/bin/.

example/Example2/bin/server.out : bin/${MULTITHREAD_LIB_DYN_EPOLL} bin/lib${STATIC_LIB} example/Example2/EpollClassServer.cpp example/Example2/MessageHandler.cpp
	${CXX} -o example/Example2/bin/server.out example/Example2/EpollClassServer.cpp example/Example2/MessageHandler.cpp  -Iexample/Example2 -Ihdr -Lbin -l${LINKING_LIB} -Wl,-R. -O1 -DNEW_DEFINES
	cp bin/${DYNAMIC_LIB} example/Example2/bin/.


bin/${MULTITHREAD_LIB_DYN_EPOLL} : src/EpollClass.cpp
	${CXX} -o $@  -fPIC -shared  src/EpollClass.cpp -Ihdr -lpthread -g --std=c++11 -Wall -DMULTITHREAD

bin/${SINGLETHREAD_LIB_DYN_EPOLL} : src/EpollClass.cpp
	${CXX} -o $@  -fPIC -shared  src/EpollClass.cpp -Ihdr -lpthread -g --std=c++11 -Wall -DSINGLETHREAD



bin/lib${MULTITHREAD_LIB_STA_EPOLL} : src/EpollClass.cpp
	${CXX} -c  src/EpollClass.cpp -o bin/EpollClass.o -Ihdr -g -DMULTITHREAD 
	ar rcs  $@ bin/EpollClass.o
	rm bin/EpollClass.o


bin/lib${SINGLETHREAD_LIB_STA_EPOLL} : src/EpollClass.cpp
	${CXX} -c  src/EpollClass.cpp -o bin/EpollClass.o -Ihdr -g -DSINGLETHREAD -DNEW_DEFINES
	ar rcs  $@ bin/EpollClass.o
	rm bin/EpollClass.o



example/Example1/bin/client.out : example/Example1/EpollClassclient.cpp
	${CXX} -o example/Example1/bin/client.out example/Example1/EpollClassclient.cpp -Ihdr -g

example/Example2/bin/client.out : example/Example2/EpollClassclient.cpp
	${CXX} -o example/Example2/bin/client.out example/Example2/EpollClassclient.cpp  -Ihdr 


clean :
	rm bin/*.out || true
	rm bin/*.so || true
	rm bin/*.a || true
	rm example/Example1/bin/* || true
	rm example/Example2/bin/* || true
