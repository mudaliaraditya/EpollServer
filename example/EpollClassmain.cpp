#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <iostream>
#include <set>
#include "EpollClass.h"

using namespace std;


int DummySimpleHandler(void* pBuffer,CEpoll* cExecutingObj)
{
    
   return 0;
}

const int PORT =  4176;
const int BUFFER_LEN = 10;
const int NO_EVENTS_WILL_HANDLE = 2;

int main()
{
   int  lnRetVal = 0;
   CEpoll lcEpoll(NO_EVENTS_WILL_HANDLE, PORT,BUFFER_LEN);

   lnRetVal = lcEpoll.Initialize();
   if(lnRetVal != 0 )
   {
      cout << "error" << endl;
      return -1;
   }

   lnRetVal = lcEpoll.SetHandlerFunction(DummySimpleHandler);
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
