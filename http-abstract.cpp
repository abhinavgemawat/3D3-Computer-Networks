
#include <iostream>
#include <fstream>
#include "http-abstract.h"
using namespace std;

// Default constructor
HttpRequest::HttpRequest(){}

// Take external Url and set it as private data member url
void HttpRequest::setURL(string input_url)
{

	url = input_url;

}

void HttpRequest::setPersistent()
{
	persistent = true;
}

// return url store in class
string HttpRequest::getURL()
{

	return url;
}

// If host name empty give error and return host name
string HttpRequest::getHost()
{
	if(host == "")
	{
		cerr << "Host Name Empty" << endl;
	}
	return host;
}

string HttpRequest::getPath()
{
	if(path == "")
	{
		cerr << "Path Name Empty" << endl;
	}

	return path;
}

string HttpRequest::getPort()
{

	if(port == "")
	{
		cerr << "No Port Specified" << endl;
	}

	return port;
}

// Function that splits url into host , port and path
// Host = hostname of server
// Port = port number used to contact server
// Path = path to requested file on server
void HttpRequest::splitURL()
{

	// Host name begins after 2 // e.g http://www.example.com
	// Will implement check for case wher no // appear
	size_t host_begin = url.find("//");
	
	if(host_begin == -1)
	{
		host_begin = 0;
	}
	else
	{
		host_begin += 2;
	}
	
	// Host ends when path begins, this means port comes before host ends!
	size_t host_end = url.find("/", host_begin);

	// Port number begins after : in middle of host name
	size_t port_begin = url.find(":", host_begin);
	
	size_t host_len;

	if( port_begin == string::npos )
	{
		// host length is end - beginning if no port number supplied
		host_len = host_end - host_begin;
		port = "80";
	}
	else
	{
		// Host length is cut at the start of port number if port number supplied
		host_len = port_begin - host_begin;
		
		
		
		// Port length is end of host - start of port
		size_t port_len = host_end - (port_begin + 1);
		// Extract port number as string
		port = url.substr(port_begin + 1, port_len);
		
	}
	// Extract host and path
	host = url.substr(host_begin, host_len);
	path = url.substr(host_end);
}

// Write HTTP request message and encode it as aray of characters, i.e bytes
string HttpRequest::getHTTPMessage()
{
	string persistentReq = "";
	
	if(persistent)
		persistentReq = "\r\nConnection: keep-alive";
	// Write GET message
	string http_message = "GET " + path + " HTTP/1.0\r\n" + "HOST: " + host + persistentReq + "\r\n\r\n";
	//cout << "Making string " << http_message << endl	
	// Return GET message in bytes
	return http_message;

}

HttpRequestHandler::HttpRequestHandler( char* in_request, string in_path )
{
	
	std::string strreq(in_request);
	
	request = strreq;
	
	setPath( in_path );
	
	if(getRequestType() == 0)
	{
		setRequestedFile();
	}
	else
	{
		request_type = 1;
	}
	
}
	
int HttpRequestHandler::getRequestType( )
{
	
	size_t req_index = request.find(" ");
	std::string type = request.substr(0, req_index);
	
	if( type == "GET" )
		return 0;
	else
		return 1;
}
	
void HttpRequestHandler::setRequestedFile( )
{
	size_t req_index = request.find(" ") + 1;
	size_t file_index = request.find(" ", req_index) - 4;
	requested_file = request.substr(req_index, file_index);
	requested_file = path + requested_file;
	cout << requested_file  << endl;
}
	
std::string HttpRequestHandler::getRequestedFile()
{
	return requested_file;
}

std::string HttpRequestHandler::getHTTPResponse()
{
	
	if(request_type == 1)
	{
		return badrequest;
	}
	
	fstream fs;
	
	fs.open(requested_file);
	if(!fs)
	{
		return notfound;
	}
	
	fs.seekg(0, fs.end);
	file_length = fs.tellg();
	fs.seekg(0, fs.beg);
	
	return ok1 + to_string(file_length) + ok2;
	
}

int HttpRequestHandler::getFileLength()
{
	return file_length;
}

void HttpRequestHandler::setPath( string in_path )
{
	path = in_path;
}

bool HttpRequestHandler::isPersistent()
{
	if(request.find("keep-alive") == -1)
		return false;
	else
		return true;
}