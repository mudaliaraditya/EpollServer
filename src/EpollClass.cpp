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
#include "EpollClass.h"
using namespace std;

CEpoll::CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen)
{
	memset(&m_stEpEvent,0,sizeof(m_stEpEvent));
	memset(&lstClientData,0,sizeof(lstClientData));
	m_pstEpHolderEvent = NULL;
	m_SockAddrStructLen = 0;
	m_nNoOfEvents = nNoOfEvents;
	m_nPort = nPort;
	m_nRecvBufferLen = nRecvBufferLen;
	m_cRecvBuffer = new char[nRecvBufferLen + 1];
   m_nSendBufferLen = nSenderBufferLen;
   m_cSendBuffer = new char[nSenderBufferLen + 1];
	m_nCurrentNoOfEvents = 0;
	m_nRunning = 0;
}

CEpoll::CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen,int (*pFunc)(void*,void*,CEpoll*)) : CEpoll(nNoOfEvents,nPort,nRecvBufferLen,nSenderBufferLen)
{ 
	if(Initialize())
	{
		throw -1;
	}
	if(SetHandlerFunction(pFunc))
	{
		throw -1;
	}
}


int  CEpoll::Initialize()
{
	int lnRetVal = 0;
	int lnFD = CreateAServerSocket();
	if(lnFD < 0 )
	{
		cout << "error " << " CEpoll::Initialize::CreateAServerSocket()"<< endl;
		return -1;
	}
	lnRetVal = BindSock();
	if(0 != lnRetVal)
	{
		cout << "error " << " CEpoll::Initialize::BindSock() "  << endl;
		return -1;
	}
	lnRetVal = ListenSock();
	if(0 != lnRetVal)
	{
		cout << "error " << " CEpoll::Initialize::ListenSock()" << endl;
		return -1;
	}
	lnRetVal = InitializeMemoryForEvents();
	if(0 != lnRetVal)
	{
		cout << "error" << "CEpoll::Initialize::InitializeMemoryForEvents()" << endl;
		return -1;
	}
	lnRetVal = InitalizeEpFD();
	if(0 != lnRetVal)
	{
		cout << "error" << "CEpoll::Initialize::InitalizeEpFD()" << endl;
		return -1;
	}
	lnRetVal = AddServerSockToEpoll();
	if(0 != lnRetVal)
	{
		cout << "error" << "CEpoll::Initialize::AddServerSockToEpoll()" << endl;
		return -1;
	}
	return 0;

}

int  CEpoll::CreateAServerSocket()
{
	m_nSockFD = socket(PF_INET, SOCK_STREAM, 0); 
	if(m_nSockFD <= 0)
	{
		printf("socket creation failed...\n");
		return -1; 
		exit(-1); 
	}
	return m_nSockFD;
}

int  CEpoll::InitializeMemoryForEvents()
{
	m_pstEpHolderEvent =(epoll_event*) malloc(sizeof(epoll_event) * m_nNoOfEvents);
	if(m_pstEpHolderEvent == NULL)
	{
		return -1;
	}
	memset(m_pstEpHolderEvent,0,(sizeof(epoll_event) * m_nNoOfEvents));
	return 0;
}
int  CEpoll::InitalizeEpFD()
{
	m_nEpFD = epoll_create(m_nNoOfEvents);
	if(m_nEpFD == 0)
	{
		return -1;
	}
	return 0;
}
int  CEpoll::BindSock()
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
int  CEpoll::ListenSock()
{
	if(listen(m_nSockFD, -1)==-1)
	{
		printf("listen() error");
		return -1;
	}
	return 0;
}


int  CEpoll::StartEpollListener()
{
	int lnRetVal = 0;
   m_nRunning = 1;
	while(m_nRunning == 1)
	{
      if(m_pFunc == NULL)
      {
        return -1;
      }
		m_nCurrentNoOfEvents = epoll_wait(m_nEpFD, m_pstEpHolderEvent, m_nNoOfEvents, -1);
		if (m_nCurrentNoOfEvents == -1)
		{
			printf(" epoll_wait %s", strerror(errno)); //puts("epoll_wait() error");
			break;
		}
		for (int i=0; i < m_nCurrentNoOfEvents; i++)//we are now iterating and going to the ith event
		{
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
				int str_len=read(m_pstEpHolderEvent[i].data.fd, m_cRecvBuffer, m_nRecvBufferLen);
				if (str_len == 0)
				{
					m_nFDStore.erase(m_pstEpHolderEvent[i].data.fd);
					epoll_ctl(m_nEpFD, EPOLL_CTL_DEL, m_pstEpHolderEvent[i].data.fd, NULL);
					close(m_pstEpHolderEvent[i].data.fd);
					//printf("closed client: %d \n", m_pstEpHolderEvent[i].data.fd);
				} else
				{
					m_nCurrentFD = m_pstEpHolderEvent[i].data.fd;
					lnRetVal = m_pFunc(m_cRecvBuffer,m_cSendBuffer,this);
					if (lnRetVal != 0)
					{
						return -1;
					}
               else
               {
                 int lnTotalLen = m_nSendBufferLen;
                 int lnBytesSent = 0;
                 while(true)
                 {
                    lnBytesSent = send(m_nCurrentFD,m_cSendBuffer,lnTotalLen,0);
                    if(lnTotalLen != lnBytesSent)
                    {
                      lnTotalLen - lnBytesSent; 
                    }
                    else
                    {
                       break;
                    }
                 }

               }
				}
            memset(m_cRecvBuffer,0,m_nRecvBufferLen);
			}
		}            
	}
	return 0;
}

int   CEpoll::SetHandlerFunction(int (*pFunc)(void*,void*,CEpoll*))
{
	m_pFunc = pFunc;
	if(m_pFunc == NULL)
	{
		return -1;
	}
	return 0;
}

int  CEpoll::AddServerSockToEpoll()
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

int  CEpoll::GetCurrentFD()
{
	return m_nCurrentFD;
}

const set<int>&  CEpoll::GetFDStore()
{
	return m_nFDStore;
}

void CEpoll::StopServer()
{
   m_nRunning = 0;
}

CEpoll::~CEpoll()
{
	delete[] m_cRecvBuffer;
	m_cRecvBuffer = nullptr;
   delete[] m_cSendBuffer;
   m_cSendBuffer = nullptr;
	free(m_pstEpHolderEvent);
}

int   CEpoll::SimpleHandler(void* pRecvBuffer,void* pSendBuffer,CEpoll* cExecutingObj)
{
	return 0;
}


