
#include "server-methods.h"

using namespace std;

void ServerClass::getserverinfo( string port )
{
		// Status variable used by getaddrinfo()
		int status;
		
		// Make sure hints struct is clearint data_socket 
		memset(&hints, 0, sizeof(hints));
    
		// Type family is unspecified so IPv4 or IPv6 can be uint data_socket sed
		hints.ai_family = AF_UNSPEC;

		// Socket type is set to socket stream i.e tcp
		hints.ai_socktype = SOCK_STREAM;

		hints.ai_flags = AI_PASSIVE;
		
		// Call to getaddrinfo
		status = getaddrinfo(NULL, port.c_str(), &hints, &servinfo);

		if(status == 0)
		{
			cout << "Server info acquired" << endl << endl;
		}
		else
		{
			cerr << gai_strerror(status) << endl;
		}
}

void ServerClass::getserverinfo( string host, string port )
{
		// Status variable used by getaddrinfo()
		int status;
		
		// Make sure hints struct is clearint data_socket 
		memset(&hints, 0, sizeof(hints));
    
		// Type family is unspecified so IPv4 or IPv6 can be uint data_socket sed
		hints.ai_family = AF_UNSPEC;

		// Socket type is set to socket stream i.e tcp
		hints.ai_socktype = SOCK_STREAM;

		hints.ai_flags = AI_PASSIVE;
		
		// Call to getaddrinfo
		status = getaddrinfo(host.c_str(), port.c_str(), &hints, &servinfo);

		if(status == 0)
		{
			cout << "Server info acquired" << endl;
		}
		else
		{
			cerr << gai_strerror(status) << endl;
		}
}


int ServerClass::create_socket()
{
	
	// Call socket to open a socket
	int sockfd;
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) < 0)
	{
		cerr << strerror(errno)  << endl;
	}
	
	
	// Make sure soket can be used
	int yes = 1;
	
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		cerr << strerror(errno)  << endl;
	}
	
	return sockfd;
	
}

void ServerClass::bindserver( int sockfd )
{
	
	// Bind a socket
	if((bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen)) < 0)
		{
			cerr << strerror(errno)  << " When Binding Socket" <<  endl;
		}
		
}

void ServerClass::listenserver( int sockfd, int backlog )
{
	// Listen on a socket
	if(listen(sockfd, backlog) < 0)
	{
		cerr << strerror(errno)  << endl;
	}
	
}

struct addrinfo* ServerClass::getservinfo()
{
	// Get the servinfo struct
	return servinfo;
	
}

int ServerClass::acceptclients( int sockfd )
{
	
		// Sets up structs and data necessary for accepting clients
		struct sockaddr_in their_addr;
		their_addr.sin_family = AF_UNSPEC;
		int data_socket;
		socklen_t length = sizeof(their_addr);
		
		// Accept client
		if((data_socket = accept(sockfd, (struct sockaddr *)&their_addr, &length)) < 0)
		{
			cerr << strerror(errno)  << endl;
		}
		
		return data_socket;
}

void ServerClass::connectserver( int sockfd )
{
	// Connect to a server using socket sockfd whose address is stored in servinfo
	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
	{
		cerr << strerror(errno)  << endl;
	}	
}
	
void ServerClass::serversend( int sockfd, string str )
{
	
	// Send data stored in str through socket sockfd 
	if(send(sockfd, str.c_str(), str.size(), 0) < 0)
	{
		cerr << strerror(errno)  << endl;
	}
	
}

int ServerClass::serverreceive( int sockfd, char* buf, int buf_len )
{
	
	// Receive data from socket sockfd and store in buf
	int received_len = recv(sockfd, buf, buf_len, 0);
	buf[received_len] = '\0';
	return received_len;
}

//void socket_receive()

