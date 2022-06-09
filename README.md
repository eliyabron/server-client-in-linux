# server-client-in-linux

A server,client code in linux environment.
The server get a request with signal calculate and send the result in file
to_client_xxxxxx.
The client code send request to the server using SIGUSR2
after the calculation,the server send a signal using SIGUSR1 so the client looks for
file to_client_xxxxxx and print the result.
