#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <iostream>
#include <set>
#include "EpollClass.h"
#include <signal.h>
using namespace std;


CEpoll* g_pcEpoll = nullptr;
int DummySimpleHandler(void* pRequestBuffer,void* pResponseBuffer,CEpoll* cExecutingObj)
{
   //write code to handle request data
   cout << (char*)pRequestBuffer << endl;
   //write on the buffer to send response
	static int x = 0;
	snprintf((char*)pResponseBuffer,5,"Hi %d",x++);
   //strncpy( (char*)pResponseBuffer,"Hello", 5);
   return 0;
}

const int PORT =  4176;
const int RECV_BUFFER_LEN = 17;
const int SEND_BUFFER_LEN = 5;
const int NO_EVENTS_WILL_HANDLE = 2;

void handle_sigint(int sig) 
{ 
   printf("Caught signal %d\n", sig); 
	g_pcEpoll->StopServer();
   return;
} 


int main()
{
   int  lnRetVal = 0;
	signal(SIGINT, handle_sigint);
   g_pcEpoll  =   new CEpoll(NO_EVENTS_WILL_HANDLE, PORT,RECV_BUFFER_LEN,SEND_BUFFER_LEN);
   lnRetVal = g_pcEpoll->Initialize();
   if(lnRetVal != 0 )
   {
      cout << "error" << endl;
      return -1;
   }

   lnRetVal = g_pcEpoll->SetHandlerFunction(DummySimpleHandler);
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }

   lnRetVal = g_pcEpoll->StartEpollListener();
   if(0 != lnRetVal)
   {
      cout << "error" << endl;
      return -1;
   }
   delete g_pcEpoll;
   g_pcEpoll = nullptr;
   cout << "exiting..." << endl;
   return 0;
}
