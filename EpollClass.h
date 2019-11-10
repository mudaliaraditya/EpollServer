#ifndef CEPOLL_H
#define CEPOLL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <iostream>
#include <set>
//using namespace std;

class CEpoll
{
   public:
      CEpoll(int nNoOfEvents,int nPort,int nBufferLen);
      int Initialize();
      int CreateAServerSocket();
      int InitializeMemoryForEvents(); 
      int InitalizeEpFD();
      int BindSock();
      int ListenSock();
      int StartEpollListener();
      int SetHandlerFunction(void*(*pFunc)(void*,CEpoll*));
      int AddServerSockToEpoll();
      int GetCurrentFD(); 
      const std::set<int>& GetFDStore();
      ~CEpoll(); 
      static void*  SimpleHandler(void* pBuffer,CEpoll* cExecutingObj);
      
   private:

      struct sockaddr_in lstClientData;
      int                 m_nBufferLen;
      char*               m_cBuffer;
      int                 m_nSockFD;
      int                 m_nCurrentFD;
      std::set<int>       m_nFDStore;
      int                 m_nEpFD;
      struct epoll_event  m_stEpEvent;
      struct epoll_event* m_pstEpHolderEvent;
      int                 m_nNoOfEvents;
      int                 m_nCurrentNoOfEvents;
      void*               (*m_pFunc)(void*,CEpoll*);
      struct sockaddr_in  m_stServerBindAddresStruct;
      socklen_t           m_SockAddrStructLen;
      int                 m_nPort;
};


#endif
