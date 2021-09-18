#ifndef	ORDERNWSTRUCTS_H
#define	ORDERNWSTRUCTS_H

#include <stdint.h>

#pragma push
//#pragma pack(1)
#pragma pack(push,1)
typedef struct
{
  int16_t      nMessageCode;
  //double     lnst1;
  //long long  lnst2;
  //char       lcVal;
}MessageStruct;


typedef struct MetaData
{
  int64_t   nTypeOfData;
  int64_t   nSizeOfBytesAhead;
  char*     pData;
}MetaData;




typedef struct OrderEntryRequest //: public MessageStruct
{
   MessageStruct m_stMessage;
   int64_t       m_nOrderId;
   int64_t       m_nOrderNo;
   int16_t       m_nSide;// 0 means buy and 1 means sell//Manadatory
   int64_t       m_nPlacedOrderTime;
   double        m_nPrice;//Mandatory
   char          m_cSymbol[20];//Mandatory
   int64_t       m_nBroker;//Mandatory
   int64_t       m_nQuantity;//Mandatory
   char          m_cOrderType;//Mandatory
}OrderEntryRequest;

typedef struct OrderEntryResponse //: public MessageStruct
{
   MessageStruct m_stMessage;
   int64_t       m_nOrderId;
   int64_t       m_nOrderNo;
   int16_t       m_nSide;// 0 means buy and 1 means sell//Manadatory
   int64_t       m_nPlacedOrderTime;
   double        m_nPrice;//Mandatory
   char          m_cSymbol[20];//Mandatory
   int64_t       m_nBroker;//Mandatory
   int64_t       m_nQuantity;//Mandatory
   char          m_cOrderType;//Mandatory
}OrderEntryResponse;



typedef struct CancelOrderRequest //: public MessageStruct
{
   MessageStruct m_stMessage;
   long long   m_nOrderId;
   long long   m_nOrderNo;//Mandatory
   short       m_nSide;// 0 means buy 1 means sell
   long long   m_nPlacedOrderTime;
   double      m_nPrice;
   char        m_cSymbol[20];
   long long   m_nBroker;
   long long   m_nQuantity;
   char        m_cOrderType;
}CancelOrderRequest;

typedef struct ReplaceOrderRequest //: public MessageStruct
{
   MessageStruct m_stMessage;
   long long   m_nOrderId;
   long long   m_nOrderNo;//Mandatory
   short       m_nSide;// 0 means buy 1 means sell
   long long   m_nPlacedOrderTime;
   double      m_nPrice;
   char        m_cSymbol[20];
   long long   m_nBroker;
   long long   m_nQuantity;
   char        m_cOrderType;
}ReplaceOrderRequest;



#pragma pack(pop)

#endif
