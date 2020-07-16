# Simple EpollServer
A Simple class wrapper for creating/using the epoll to handle multiple tcp connections
Simplifies the task of handling request and response using Epoll for handling Multiple Connections  

   **NO_EVENTS_WILL_HANDLE** = No of max events you will be handling at any given time  
   **PORT** = Port on which the server will be bind  
   **RECV_BUFFER_LEN** = Your client message len which was sent from the client end  
   **SEND_BUFFER_LEN** = Your response message which will be sent back to the client  

   CEpoll lcEpoll(NO_EVENTS_WILL_HANDLE, PORT,RECV_BUFFER_LEN,SEND_BUFFER_LEN);  

   Call the below Functions in that order  

   lnRetVal = lcEpoll.**Initialize**();  
   lnRetVal = lcEpoll.**SetHandlerFunction**(MessageProcessing); //your actual Request Handler/Message Processing function needs to be given here  
   lnRetVal = lcEpoll.**StartEpollListener**();  

   in order to stop the Epoll call on  
   lcEpoll.**StopServer**();  
   either from other thread or through signal to stop the server  


   your Message Processing Function will need to have the following format  

   **int MessageProcessing(void* pRequestBuffer,void* pResponseBuffer,CEpoll* cExecutingObj)**  

   pRequestBuffer - this will hold the request data buffer/information recieved from the client end  
   pResponseBuffer - this buffer needs to be filled with the response  
   cExecutingObj - if you want to handle/stop on some message  
   
   return 0 for sucess or -1 to exit application due to unrecoverable failure  

   feel free to raise issue/suggestions  
   mail:mudaliar.aditya@gmail.com  

