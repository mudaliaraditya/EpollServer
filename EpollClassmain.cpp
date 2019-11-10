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


void*  TheSimpleHandler(void* pBuffer,CEpoll* cExecutingObj)
{
    
   return NULL;
}

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

   lnRetVal = lcEpoll.SetHandlerFunction(TheSimpleHandler);
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
