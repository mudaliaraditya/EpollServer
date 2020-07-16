# Simple EpollServer
A Simple class wrapper for creating/using the epoll to handle multiple tcp connections
Simplifies the task of handling request and response using Epoll for handling Multiple Connections

   NO_EVENTS_WILL_HANDLE = NO of max events you will be handling at any given time
   PORT = on which the server will be bind
   RECV_BUFFER_LEN = your client message len which was sent from the client end
   SEND_BUFFER_LEN = your response message which will be sent back to the client

   CEpoll lcEpoll(NO_EVENTS_WILL_HANDLE, PORT,RECV_BUFFER_LEN,SEND_BUFFER_LEN);
   
   Call the below Functions in that order

   lnRetVal = lcEpoll.Initialize();
   lnRetVal = lcEpoll.SetHandlerFunction(DummySimpleHandler);
   lnRetVal = lcEpoll.StartEpollListener();
   
   in order to stop the Epoll call on
   lcEpoll.StopServer();
	either from other thread or through signal to stop the server
