#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include "MessageNos.h"
#include "OrderNwStructs.h"
#include "EpollClass.h"

//struct CEpoll;



int MessageHandler(void* pRecvBuffer,void* pSendBuffer,CEpoll* cExecutingObj);

#endif
