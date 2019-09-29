#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <string>
//#include <cstring>
#include <thread>
#include <iostream>
#include <fstream>

#include "http-abstract.h"
#include "server-methods.h"

using namespace std;

#define NUMTHREADS 10

struct communication_info {
	int sockfd;
	ServerClass myServer;
	int clientID;
	string pathtofile;
	pthread_t thread;
};

struct timeout {
	int sockfd;
	pthread_t thread;
};
	
void *threadtimeout(void *threadarg) {
	
	struct timeout *my_info;
	my_info = (struct timeout *) threadarg;
	sleep(20);
	close(my_info->sockfd);
	cout << "Timeout ";
	pthread_exit(NULL);
	
}
	
void *threadsendreceive(void *threadarg) {
	
	pthread_t timing_thread;
	struct timeout to;
	
	
	struct communication_info *my_info;
	my_info = (struct communication_info *) threadarg;
	
	to.sockfd = my_info->sockfd;
	to.thread = my_info->thread;
	
	char buffer[500];
		
	fstream fs;

		pthread_create(&timing_thread, NULL, threadtimeout, (void *) &to);
	
		int numbytes;
		while((numbytes = my_info->myServer.serverreceive(my_info->sockfd, buffer, 500)) > 0)
		{
			pthread_cancel(timing_thread);
			
			cout << "Client " << my_info->clientID << " Received :" << endl << buffer << endl;
			
			HttpRequestHandler httpHandler(buffer, my_info->pathtofile);			
			
			if(httpHandler.getRequestType() == 1)
			{
				cout << "Client " << my_info->clientID << " Sent :" << endl;
				my_info->myServer.serversend(my_info->sockfd, httpHandler.getHTTPResponse());
			}
			else
			{
				
				fs.open(httpHandler.getRequestedFile());
				if(!fs)
				{
					cout << "file open failed" << endl << endl;
					cout << "Client " << my_info->clientID << " Sent :" << endl;
					my_info->myServer.serversend(my_info->sockfd, httpHandler.getHTTPResponse());
				}
				else
				{
					cout << "Client " << my_info->clientID << " Sent :" << endl;
					
					my_info->myServer.serversend(my_info->sockfd, httpHandler.getHTTPResponse());
					
					char output_buffer[httpHandler.getFileLength()];
					
					fs.read(output_buffer, httpHandler.getFileLength());
					
					cout << output_buffer << endl;
					
					my_info->myServer.serversend(my_info->sockfd, output_buffer);
					
					fs.close();
				
				}
			}
			cout << httpHandler.getHTTPResponse() << endl;
			
			if (!httpHandler.isPersistent())
			{
				close(my_info->sockfd);
				cout << "Connection closed... " << endl;
				pthread_exit(NULL);
			}
			
			pthread_create(&timing_thread, NULL, threadtimeout, (void *) &to);
			
		}
		
		if(numbytes == 0)
		{
			close(my_info->sockfd);
			cout << "Connection closed..." << endl;
			pthread_exit(NULL);
		}
		
		if(numbytes < 0)
		{
			cerr << strerror(errno) << " when receiving"  << endl;
			pthread_exit(NULL);
		}
}

int main(int argc, char* argv[])
{
  //std::cerr << "web client is not implemented yet" << std::endl;
  // do your stuff here! or not if you don't want to.
	if (argc <= 1)
	{
		cerr << "No Url Supplied" << endl;
	}
	else
	{
		// If an argument was supplied at the command line enter this block

		// Take first command line argument and stoe the string in variable url_1
		// This will need to be replaced with a for loop that takes all arguments from argc
		// And stores them for processing
		string hostname = argv[1];
		string port = argv[2];
		string path = argv[3];
		
		pthread_t threads[NUMTHREADS];
		int rc;
		int i = 0;
		struct communication_info ci[NUMTHREADS];
		
		int BACKLOG = 1;
		
		ServerClass server;

		server.getserverinfo( port );

		int server_socket = server.create_socket();
		
		server.bindserver( server_socket );
		
		
		while(1)
		{
			server.listenserver( server_socket, BACKLOG );
			
			int data_socket = server.acceptclients( server_socket );
			
			ci[i].sockfd = data_socket;
			ci[i].myServer = server;
			ci[i].clientID = i;
			ci[i].pathtofile = path;
			ci[i].thread = threads[i];
			rc = pthread_create(&threads[i], NULL, threadsendreceive, (void *) &ci[i]);
			
			if(rc)
			{
				cout << "Unable to create thread" << rc << endl;
				exit(-1);
			}

			i++;
		}
		
	
		close(server_socket);

	}
	return 0;

}
