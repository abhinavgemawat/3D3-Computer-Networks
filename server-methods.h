#ifndef SERVER_METHODS_H
#define SERVER_METHODS_H

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <iostream>

class ServerClass
{
public:

	int create_socket();
	void getserverinfo( std::string );
	void getserverinfo( std::string, std::string );
	void bindserver( int );
	void listenserver( int, int );
	int acceptclients( int );
	
	void connectserver( int );
	
	void serversend( int, std::string );
	int serverreceive( int, char*, int );
	
	struct addrinfo* getservinfo();
	
private:

	// Hints struct used by getaddrinfo()
	struct addrinfo hints;

	// Pointer to address info struct
	struct addrinfo *servinfo;

};

#endif