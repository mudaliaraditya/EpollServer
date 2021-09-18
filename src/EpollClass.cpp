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
#include <fcntl.h>

CEpoll::CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen)   : m_cMsgs() 
//#ifdef SINGLETHREAD
//CEpoll::CEpoll(int nNoOfEvents,int nPort,int nRecvBufferLen,int nSenderBufferLen)    
//#endif
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
		throw std::runtime_error("error in Initialize()");
	}
	if(SetHandlerFunction(pFunc))
	{
		throw std::runtime_error("error in SetHandlerFunction()");
	}
}


int  CEpoll::Initialize()
{
	int lnRetVal = 0;
	int lnFD = CreateAServerSocket();
	if(lnFD < 0 )
	{
		std::cout << "error " << " CEpoll::Initialize::CreateAServerSocket()"<< std::endl;
		return -1;
	}
	lnRetVal = BindSock();
	if(0 != lnRetVal)
	{
		std::cout << "error " << " CEpoll::Initialize::BindSock() "  << std::endl;
		return -1;
	}
	lnRetVal = ListenSock();
	if(0 != lnRetVal)
	{
		std::cout << "error " << " CEpoll::Initialize::ListenSock()" << std::endl;
		return -1;
	}
	lnRetVal = InitializeMemoryForEvents();
	if(0 != lnRetVal)
	{
		std::cout << "error" << "CEpoll::Initialize::InitializeMemoryForEvents()" << std::endl;
		return -1;
	}
	lnRetVal = InitalizeEpFD();
	if(0 != lnRetVal)
	{
		std::cout << "error" << "CEpoll::Initialize::InitalizeEpFD()" << std::endl;
		return -1;
	}
	lnRetVal = AddServerSockToEpoll();
	if(0 != lnRetVal)
	{
		std::cout << "error" << "CEpoll::Initialize::AddServerSockToEpoll()" << std::endl;
		return -1;
	}
	return 0;

}

int  CEpoll::CreateAServerSocket()
{
	int opt = 1;
	m_nSockFD = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK , 0); 
	if(m_nSockFD <= 0)
	{
		printf("socket creation failed...\n");
		return -1; 
		exit(-1); 
	}
	if (setsockopt(m_nSockFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
   {
        printf("setsockopt failed");
		  return -1;
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
   m_nRunning = 1;
#ifdef MULTITHREAD
	pthread_create(&m_nSenderThread,NULL,CEpoll::InitateSenderThread,this);
#endif
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
			//continue;
		}
		for (int i=0; i < m_nCurrentNoOfEvents; i++)//we are now iterating and going to the ith event
		{
			//here we detect wether the recived connection is new or old//we detect events occuring on the server socket and add accordingly
			if (m_pstEpHolderEvent[i].data.fd == m_nSockFD)
			{
				std::cout << " data recvd from new client " << std::endl;
				int adr_sz=sizeof (lstClientData);
				int lnClientFD = accept(m_nSockFD, (struct sockaddr*) &lstClientData, (socklen_t*)&adr_sz);
				if( lnClientFD <= 0)
				{
					exit(-1);
				}
				m_nFDStore.insert(lnClientFD);
				m_stEpEvent.events = EPOLLIN;
				m_stEpEvent.data.fd = lnClientFD;
            int flags = fcntl(lnClientFD, F_GETFL, 0);
            flags = (flags | O_NONBLOCK);
            (fcntl(lnClientFD, F_SETFL, flags));
				epoll_ctl( m_nEpFD, EPOLL_CTL_ADD, lnClientFD, &m_stEpEvent);
			} else
			{
				std::cout << " reading data from the connected client with fd "<< m_pstEpHolderEvent[i].data.fd  << std::endl;
				//if connection is an old one it is checked if its a disconnect or a packet is recieved
				memset(m_cRecvBuffer,0,m_nRecvBufferLen);
				int str_len=read(m_pstEpHolderEvent[i].data.fd, m_cRecvBuffer, m_nRecvBufferLen);
				if (str_len == 0)
				{
					std::cout << " client disconnected with fd "<< m_pstEpHolderEvent[i].data.fd  << std::endl;
					m_nFDStore.erase(m_pstEpHolderEvent[i].data.fd);
					epoll_ctl(m_nEpFD, EPOLL_CTL_DEL, m_pstEpHolderEvent[i].data.fd, NULL);
					close(m_pstEpHolderEvent[i].data.fd);
					//printf("closed client: %d \n", m_pstEpHolderEvent[i].data.fd);
				}
				else
				{
					int lnRetVal = -1;
					std::cout << "custom handler called for fd " << m_pstEpHolderEvent[i].data.fd  << std::endl;
					#ifdef MULTITHREAD
					std::cout << "data recieved with " << str_len << " bytes" << std::endl;
					char* lpcNewMsg = new char[str_len];
					memcpy(lpcNewMsg,m_cRecvBuffer,str_len);
					int lnFD = m_pstEpHolderEvent[i].data.fd;
					m_cMsgs.SendData(std::make_pair( lnFD,lpcNewMsg));
					#endif
					#ifdef SINGLETHREAD
					m_nCurrentFD = m_pstEpHolderEvent[i].data.fd;
					std::cout << " value of recvBuffer " << m_cRecvBuffer << std::endl;
					memset(m_cSendBuffer,0,m_nSendBufferLen);
					lnRetVal = m_pFunc(m_cRecvBuffer,m_cSendBuffer,this);
					if (lnRetVal != 0)
					{
						std::cout << "ERROR in Processing" << std::endl;
						continue;
					}
               else if(lnRetVal == 0)
               {
                 int lnTotalLen = m_nSendBufferLen;
                 int lnBytesSent = 0;
                 while(true)
                 {
						  std::cout << "sending m_cSendBuffer " << m_cSendBuffer << std::endl;
                    lnBytesSent = send(m_nCurrentFD,m_cSendBuffer,lnTotalLen,0);
						  memset(m_cSendBuffer,0,m_nSendBufferLen);
						  if(lnBytesSent == lnTotalLen)
						  {
							  break;
						  }
						  else if((lnTotalLen != lnBytesSent) && ( lnBytesSent > 0 ))
                    {
                       lnTotalLen =  lnTotalLen - lnBytesSent; 
                    }
						  else
						  {
							  break;
						  }
                 }

               }
					#endif
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

int CEpoll::SetVariableLenResponse(int nLen)
{
	delete[] m_cSendBuffer;
	m_cSendBuffer = NULL;
	m_cSendBuffer = new char[nLen];
	return 0;
}

void CEpoll::SetResponseBuffer(char* pData,int nLen)
{

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

const std::set<int>&  CEpoll::GetFDStore()
{
	return m_nFDStore;
}

void CEpoll::StopServer()
{
   m_nRunning = 0;
}

#ifdef MULTITHREAD
bool CEpoll::SenderThread()
{
	int lnRetVal = 0;
	std::cout << "SenderThread" << std::endl;
	std::cout << "waiting for new msgs" << std::endl;
	while(m_nRunning == 1)
	{
		while( m_cMsgs.IsMessagesPresent())
		{
			if(m_nRunning == 0)
			{
				std::cout << "Exiting Sender Thread" << std::endl;
				break;
			}
			std::cout << "SenderThread Message Present" << std::endl;
			int lnFD = 0;
			char* pRecvr = nullptr;
			Fd_Msg_Pair lcPairData = m_cMsgs.GetData();
			lnFD = lcPairData.first;
			pRecvr = reinterpret_cast<char*>(lcPairData.second);
			memset(m_cSendBuffer,0,m_nSendBufferLen);
			lnRetVal = m_pFunc(pRecvr,m_cSendBuffer,this);
			if (lnRetVal != 0)
			{
				std::cout << "ERROR in Processing" << std::endl;
				//continue;
			}
			else if(lnRetVal == 0)
			{
				int lnTotalLen = m_nSendBufferLen;
				int lnBytesSent = 0;
				while(true)
				{
					std::cout << "sending m_cSendBuffer " << m_cSendBuffer << std::endl;
					lnBytesSent = send(lnFD,m_cSendBuffer,lnTotalLen,0);
					memset(m_cSendBuffer,0,m_nSendBufferLen);
					if(lnBytesSent == lnTotalLen)
					{
						break;
					}
					else if((lnTotalLen != lnBytesSent) && ( lnBytesSent > 0 ))
					{
						lnTotalLen =  lnTotalLen - lnBytesSent; 
					}
					else
					{
						break;
					}
				}

			}
			if(pRecvr != nullptr)
			{
				delete[] pRecvr;
				pRecvr = nullptr;
			}
		}
	}
	return true;
}

void*  CEpoll::InitateSenderThread(void* pData)
{
	std::cout << "sender thread started" << std::endl;
	CEpoll* pCEpoll = static_cast< CEpoll* >(pData);
	return reinterpret_cast<void*>(pCEpoll->SenderThread());
}
#endif
CEpoll::~CEpoll()
{
	close(m_nSockFD);
	delete[] m_cRecvBuffer;
	m_cRecvBuffer = nullptr;
   delete[] m_cSendBuffer;
   m_cSendBuffer = nullptr;
	free(m_pstEpHolderEvent);
#ifdef MULTITHREAD
	m_cMsgs.SendData(std::make_pair( 0,reinterpret_cast<void*>(NULL)));//Sending Dummy Data to release conditional Variable
	pthread_join(m_nSenderThread,NULL);
#endif
}

int   CEpoll::SimpleHandler(void* pRecvBuffer,void* pSendBuffer,CEpoll* cExecutingObj)
{
	return 0;
}
