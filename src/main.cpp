#include <iostream>
#include <string>
#include <cstring>
#include <functional>
#include <thread>
#include "server.h"
#include "http.h"
#include "logger.h"
#include "server_config.h"

void HandleClientConnection(Networking::Server& server, Networking::ClientConnection _pClient)
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

int main(int argc, char* argv[]){
	std::uint16_t port;
	std::string error;
	if (!ParseServerPort(argc, argv, port, error)) {
		std::cerr << error << std::endl;
		return 2;
	}
	static Networking::Server server(port, Networking::ServerType::IPv4);

 
	if(server.ServerIsRunning()) {
		std::cout<<"Server Initalized"<<std::endl;
	}
	else {
		std::cout<<"Failed to initialize server"<<std::endl;
		return 1;
	}

	while (true) {
		// Accept a client connection
		Networking::ClientConnection client = server.Accept();
		std::thread clientThread(HandleClientConnection, std::ref(server), client);
		clientThread.detach();
	}

	return 0;
}
