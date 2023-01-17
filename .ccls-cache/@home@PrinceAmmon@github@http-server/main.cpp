#include <iostream>
#include <string>
#include <cstring>
#include "server.h"
#include "http.h"
int main(){

	Networking::Server server(80, Networking::ServerType::IPv4);

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
		// Read the incoming request
		std::string request = &server.Receive(client)[0];
		HTTP::HTTPREQUEST httpRequest = HTTP::ParseHttpRequest(request);
		// Send a response to the client
		std::string response = HTTP::GenerateResponse(httpRequest);
		server.Send(&response[0], client);
	}

	return 0;
}
