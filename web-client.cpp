#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <thread>
#include <iostream>
#include "http-abstract.h"
#include "server-methods.h"
using namespace std;


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
		string url_1;
		
		cout << argc << endl;
		
		for(int i = 1; i < argc; i++)
		{
			url_1 = argv[i];

			// Initialize the http request handler using default constructor
			// Default constructor might have to be changed to encapsulate all functions as one constructor
			HttpRequest httpreq;

			// Set http request handler url to url inputted at comman line
			httpreq.setURL(url_1);

			// Split the inputted url into parts named host, port and path
			// Host holds the hostname of server we're trying to contact
			// Port holds the port on that server we're trying to contact on
			// Path holds path of file we're looking for on server
			httpreq.splitURL();

			cout << "Host Name: " <<  httpreq.getHost() << endl << "Port Number: " << httpreq.getPort() << endl << "Path Name: " <<  httpreq.getPath() << endl;

			ServerClass client;

			client.getserverinfo( httpreq.getHost(), httpreq.getPort() );
			
			int server_socket =client.create_socket();

			client.connectserver( server_socket );
			
			string persistent;
			cout << endl << "Persistent? (y/n)" << endl;
			cin >> persistent;
			
			if(persistent == "y")
			{
				httpreq.setPersistent();
			}
			
			cout << "Sent: " << endl << endl << httpreq.getHTTPMessage() << endl;
				
			client.serversend( server_socket, httpreq.getHTTPMessage() );
			
			int numbytes;
			char buffer[500];
			while((numbytes = client.serverreceive(server_socket, buffer, 500)) > 0)
			{			
			
				cout << "Received:" << endl << endl << buffer << endl;
				
			}
			
			if(numbytes == 0)
			{
				close(server_socket);
				cout << "Connection closed..." << endl;
			}
			
			if(numbytes < 0)
			{
				cerr << strerror(errno) << " when receiving"  << endl;
			}
		}
	}
	return 0;

}
