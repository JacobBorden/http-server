#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include "server.h"
#include "http.h"
#include "logger.h"
#include "thread_pool.h"

Networking::Server server(80, Networking::ServerType::IPv4);

void HandleClientConnection(Networking::ClientConnection _pClient)
{// Read the incoming request
		std::string request = &server.Receive(_pClient)[0];
		HTTP::HTTPREQUEST httpRequest = HTTP::ParseHttpRequest(request);
		std::string ip = server.GetClientIPAddress(_pClient);
		//Log the http request
		server.LogToFile(
				"Client IP: " + ip +
				" - Request: " + httpRequest.method + " " + httpRequest.uri +
				" - HTTP version: " +httpRequest.protocol);
		// Send a response to the client
		std::string response = HTTP::GenerateResponse(httpRequest);
		server.Send(&response[0], _pClient);
		server.DisconnectClient(_pClient);
	

}

int main(){

 
	if(server.ServerIsRunning()) {
		std::cout<<"Server Initalized"<<std::endl;
	}
	else {
		std::cout<<"Failed to initialize server"<<std::endl;
		return 1;
	}

	ThreadPool pool(8);

	while (true) {
		// Accept a client connection
		Networking::ClientConnection client = server.Accept();
		pool.enqueue([client]() { HandleClientConnection(client); });
	}

	return 0;
}
