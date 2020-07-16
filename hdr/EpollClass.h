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
      CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen);
      CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen,int (*pFunc)(void*,void*,CEpoll*));
      int Initialize();
      int CreateAServerSocket();
      int InitializeMemoryForEvents(); 
      int InitalizeEpFD();
      int BindSock();
      int ListenSock();
      int StartEpollListener();
      int SetHandlerFunction(int (*pFunc)(void*,void*,CEpoll*));
      int AddServerSockToEpoll();
      int GetCurrentFD(); 
      const std::set<int>& GetFDStore();
      void StopServer();
      ~CEpoll(); 
      static int  SimpleHandler(void* pRecvBuffer,void* pSendBuffer,CEpoll* cExecutingObj);
      
   private:

      struct sockaddr_in lstClientData;
      int                 m_nRecvBufferLen;
      int                 m_nSendBufferLen;
      char*               m_cRecvBuffer;
      char*               m_cSendBuffer;
      int                 m_nSockFD;
      int    m_nCurrentFD;
      std::set<int>       m_nFDStore;
      int                 m_nEpFD;
      struct epoll_event  m_stEpEvent;
      struct epoll_event* m_pstEpHolderEvent;
      int                 m_nNoOfEvents;
      int                 m_nCurrentNoOfEvents;
      int                 (*m_pFunc)(void*,void*,CEpoll*);
      struct sockaddr_in  m_stServerBindAddresStruct;
      socklen_t           m_SockAddrStructLen;
      int                 m_nPort;
      bool                m_nRunning;
};


#endif
