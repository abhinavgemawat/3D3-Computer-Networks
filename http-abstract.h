#ifndef HTTP_ABSTRACT_H
#define HTTP_ABSTRACT_H

#include <string>

class HttpRequest
{
public:

	HttpRequest();

	void setURL(std::string);
	std::string getURL();

	std::string getHost();
	std::string getPath();
	std::string getPort();
	
	void setPersistent();
	
	void splitURL();
	std::string getHTTPMessage();
	
private:

	std::string url;
	std::string path;
	std::string host;
	std::string port;
	bool persistent = false;
};

class HttpRequestHandler
{
public:

	HttpRequestHandler( char*, std::string );
	
	int getRequestType( );
	
	void setRequestedFile( );
	
	std::string getRequestedFile();
	
	int getFileLength();
	
	std::string getHTTPResponse();
	
	void setPath( std::string );
	
	bool isPersistent();
	
private:
	
	std::string request;
	std::string requested_file;
	
	int request_type = 0;
	
	std::string badrequest = "HTTP/1.0 400 Bad Request \r\n\r\n";
	
	std::string notfound = "HTTP/1.0 404 Not Found \r\n\r\n";
	
	std::string ok1 = "HTTP/1.0 200 OK\nConnection:Keep-Alive\nContent-Type: text/html, image; charset=UTF-8\nContent-Length: ";
	std::string ok2 = "\r\n\r\n";
	
	int file_length;
	
	std::string path;
};

#endif
