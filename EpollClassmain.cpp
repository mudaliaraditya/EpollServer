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
using namespace std;

class CEpoll
{
   public:
      CEpoll(int nNoOfEvents,int nPort,int nBufferLen)
      {
         memset(&m_stEpEvent,0,sizeof(m_stEpEvent));
         memset(&lstClientData,0,sizeof(lstClientData));
         m_pstEpHolderEvent = NULL;
         m_SockAddrStructLen = 0;
         m_nNoOfEvents = nNoOfEvents;
         m_nPort = nPort;
         m_nBufferLen = nBufferLen;
         m_cBuffer = new char[nBufferLen + 1];
         m_nCurrentNoOfEvents = 0;
      }


      int Initialize()
      {
         int lnRetVal = 0;
         int lnFD = CreateAServerSocket();
         if(lnFD < 0 )
         {
            cout << "error" << endl;
            return -1;
         }
         lnRetVal = BindSock();
         if(0 != lnRetVal)
         {
            cout << "error" << endl;
            return -1;
         }
         lnRetVal = ListenSock();
         if(0 != lnRetVal)
         {
            cout << "error" << endl;
            return -1;
         }
         lnRetVal = InitializeMemoryForEvents();
         if(0 != lnRetVal)
         {
            cout << "error" << endl;
            return -1;
         }
         lnRetVal = InitalizeEpFD();
         if(0 != lnRetVal)
         {
            cout << "error" << endl;
            return -1;
         }
         lnRetVal = AddServerSockToEpoll();
         if(0 != lnRetVal)
         {
            cout << "error" << endl;
            return -1;
         }
         return 0;

      }

      int CreateAServerSocket()
      {
         m_nSockFD = socket(PF_INET, SOCK_STREAM, 0); 
         if(m_nSockFD <= 0)
         {

            return -1;        printf("socket creation failed...\n"); 
            exit(-1); 
         }
         return m_nSockFD;
      }

      int InitializeMemoryForEvents()
      {
         m_pstEpHolderEvent =(epoll_event*) malloc(sizeof(epoll_event) * m_nNoOfEvents);
         if(m_pstEpHolderEvent == NULL)
         {
            return -1;
         }
         memset(m_pstEpHolderEvent,0,(sizeof(epoll_event) * m_nNoOfEvents));
         return 0;
      }
      int InitalizeEpFD()
      {
         m_nEpFD = epoll_create(m_nNoOfEvents);
         if(m_nEpFD == 0)
         {
            return -1;
         }
         return 0;
      }
      int BindSock()
      {
         m_stServerBindAddresStruct.sin_family = AF_INET;
         m_stServerBindAddresStruct.sin_addr.s_addr = htonl(INADDR_ANY);
         m_stServerBindAddresStruct.sin_port = htons(m_nPort);
         if (bind(m_nSockFD, (struct sockaddr*) &m_stServerBindAddresStruct, sizeof (m_stServerBindAddresStruct)) == -1)
         {
            printf("bind %s", strerror(errno)); //error_handling("bind() error");
            return -1;
         }
         return 0;
      }
      int ListenSock()
      {
         if(listen(m_nSockFD, -1)==-1)
         {
            printf("listen() error");
            return -1;
         }
         return 0;
      }


      int StartEpollListener()
      {
         int lnRetVal = 0;
         while(true)
         {
            m_nCurrentNoOfEvents = epoll_wait(m_nEpFD, m_pstEpHolderEvent, m_nNoOfEvents, -1);
            if (m_nCurrentNoOfEvents == -1)
            {
               //event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
               //if(event_cnt==-1)
               {
                  printf(" epoll_wait %s", strerror(errno)); //puts("epoll_wait() error");
                  break;
               }
            }
            for (int i=0; i < m_nCurrentNoOfEvents; i++)//we are now iterating and going to the ith event
            {
               printf("the value of event count is %d \n", m_nCurrentNoOfEvents);
               printf("the value of i counter is %d \n", i);
               //here we detect wether the recived connection is new or old//we detect events occuring on the server socket and add accordingly
               if (m_pstEpHolderEvent[i].data.fd == m_nSockFD)
               {
                  int adr_sz=sizeof (lstClientData);
                  int lnClientFD = accept(m_nSockFD, (struct sockaddr*) &lstClientData, (socklen_t*)&adr_sz);
                  if( lnClientFD <= 0)
                  {
                     exit(-1);
                  }
                  m_nFDStore.insert(lnClientFD);
                  m_stEpEvent.events = EPOLLIN;
                  m_stEpEvent.data.fd = lnClientFD;
                  epoll_ctl( m_nEpFD, EPOLL_CTL_ADD, lnClientFD, &m_stEpEvent);
               } else
               {
                  //if connection is an old one it is checked if its a disconnect or a packet is recieved
                  int str_len=read(m_pstEpHolderEvent[i].data.fd, m_cBuffer, m_nBufferLen);
                  if (str_len == 0)
                  {
                     m_nFDStore.erase(m_pstEpHolderEvent[i].data.fd);
                     epoll_ctl(m_nEpFD, EPOLL_CTL_DEL, m_pstEpHolderEvent[i].data.fd, NULL);
                     close(m_pstEpHolderEvent[i].data.fd);
                     printf("closed client: %d \n", m_pstEpHolderEvent[i].data.fd);
                  } else
                  {
                     m_nCurrentFD = m_pstEpHolderEvent[i].data.fd;
                     cout << "data recieved" << endl;
                     m_pFunc(m_cBuffer,this);
                     if (lnRetVal != 0)
                     {
                        return -1;
                     }
                     cout << "handler function is being called";
                  }
               }
            }            
         }
         return 0;
      }
      int  SetHandlerFunction(void*(*pFunc)(void*,CEpoll*))
      {
         m_pFunc = pFunc;
         if(m_pFunc == NULL)
         {
            return -1;
         }
         return 0;
      }

      int AddServerSockToEpoll()
      {
         m_stEpEvent.events = EPOLLIN;
         m_stEpEvent.data.fd = m_nSockFD;

         if ( -1 == epoll_ctl(m_nEpFD, EPOLL_CTL_ADD, m_nSockFD, &m_stEpEvent))
         {
            printf("epoll_ctl %s", strerror(errno)); //error_handling("bind() error");
            return -1;
         }




         return 0; 
      }

      int GetCurrentFD()
      {
         return m_nCurrentFD;
      }
      
     const set<int>& GetFDStore()
     {
          return m_nFDStore;
     }

      ~CEpoll()
      {
         delete[] m_cBuffer;
         free(m_pstEpHolderEvent);
      }
      static void*  SimpleHandler(void* pFunction,CEpoll* cExecutingObj)
      {
         return NULL;
      }
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

int main()

{
   int  lnRetVal = 0;
   CEpoll lcEpoll(2, 4176,10);

   lnRetVal = lcEpoll.Initialize();
   if(lnRetVal != 0 )
   {
      cout << "error" << endl;
      return -1;
   }

   lnRetVal = lcEpoll.SetHandlerFunction(CEpoll::SimpleHandler);
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }

   //int lnFD = lcEpoll.CreateAServerSocket();
   //if(lnFD < 0 )
   {
      //cout << "error" << endl;
      //return -1;
   }
   //lnRetVal = lcEpoll.BindSock();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   //lnRetVal = lcEpoll.ListenSock();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   //lnRetVal = lcEpoll.InitializeMemoryForEvents();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   //lnRetVal = lcEpoll.InitalizeEpFD();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   //lnRetVal = lcEpoll.AddServerSockToEpoll();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   lnRetVal = lcEpoll.StartEpollListener();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
}
