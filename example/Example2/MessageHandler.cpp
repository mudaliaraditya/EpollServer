#include "MessageNos.h"
#include "OrderNwStructs.h"
#include "EpollClass.h"
#include <stdint.h>
struct CEpoll;



int MessageHandler(void* pRecvBuffer,void* pSendBuffer,CEpoll* cExecutingObj)
{
	MetaData* lpstData = (MetaData*)pRecvBuffer;
   long long lnBytes = lpstData->nSizeOfBytesAhead;
   int16_t* lpMessageStruct =(int16_t*) &(lpstData->pData);
   switch(*lpMessageStruct)
	{
		case MESSAGE_CODE_ORDER_ENTRY_REQUEST:
			{
				OrderEntryRequest* lpcOrderEntryRequest = (OrderEntryRequest*)(lpMessageStruct); 
			   MetaData* lpstDataForResponse = (MetaData*)pSendBuffer;
            lpstDataForResponse->nSizeOfBytesAhead = lnBytes;
            printf(" size of bytes ahead val : %ld",lpstDataForResponse->nSizeOfBytesAhead);
			   OrderEntryRequest* lpcOrderEntryResponse  = (OrderEntryRequest*)&(lpstDataForResponse->pData);
				*lpcOrderEntryResponse = *lpcOrderEntryRequest;
				lpcOrderEntryResponse->m_cOrderType = 'K';	
			}
			break;            
		case MESSAGE_CODE_ORDER_ENTRY_RESPONSE:          
			{
			}
			break;

		case MESSAGE_CODE_ORDER_FETCH_REQUEST:
			{
			}
			break;            
		case MESSAGE_CODE_ORDER_FETCH_RESPONSE:          
			{
			}
			break; 
		case MESSAGE_CODE_ORDER_MOD_REQUEST:             
			{
			}
			break; 
		case MESSAGE_CODE_ORDER_MOD_RESPONSE:            
			{
			}
			break; 
		case MESSAGE_CODE_ORDER_ORDERBOOK_FETCH_REQUEST: 
			{
			}
			break; 
		case MESSAGE_CODE_ORDER_ORDERBOOK_FETCH_RESPONSE:
			{
			}
			break; 
		case MESSAGE_CODE_ORDER_TRADE_RESPONSE:          
			{
			}
			break; 

			//case

	}

	return 0;
}
