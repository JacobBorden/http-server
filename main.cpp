#include <iostream>
#include <string>
#include <cstring>
#include <thread>
#include "server.h"
#include "http.h"

Networking::Server server(80, Networking::ServerType::IPv4);

void HandleClientConnection(Networking::ClientConnection _pClient)
{// Read the incoming request
		std::string request = &server.Receive(_pClient)[0];
		HTTP::HTTPREQUEST httpRequest = HTTP::ParseHttpRequest(request);
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

	while (true) {
		// Accept a client connection
		Networking::ClientConnection client = server.Accept();
		std::thread clientThread(HandleClientConnection, client);
		clientThread.detach();
	}

	return 0;
}
