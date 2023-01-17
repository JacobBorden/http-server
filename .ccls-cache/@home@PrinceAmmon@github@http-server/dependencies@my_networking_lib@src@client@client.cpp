#include "client.h" // include the header file for the client class

// Constructor that initializes the client socket
Networking::Client::Client()
{
	try {
		// Initialize the client socket
		Networking::Client::InitClientSocket();
	}

	// Catch any exceptions that are thrown
	catch(int errorCode) {
		// Print the error code
		std::cout<<"Exception thrown. Error Code"<<errorCode;
	}
}

// Constructor that initializes the client socket and connects to the server
Networking::Client::Client(char* _pHost, int _pPortNumber)
{
	try{
		// Initialize the client socket
		Networking::Client::InitClientSocket();
		// Create a TCP socket
		Networking::Client::CreateClientTCPSocket(_pHost, _pPortNumber);
		// Connect to the server
		Networking::Client::ConnectClientSocket();
	}
	// Catch any exceptions that are thrown
	catch(int errorCode) {
		// Print the error code
		std::cout<<"Exception thrown. Error Code "<<errorCode;
	}
}

// Destructor for the client class
Networking::Client::~Client()
{
}

// Destructor for the client class
bool Networking::Client::InitClientSocket()
{
    #ifdef _WIN32
	// Initialize the Windows Sockets DLL
	int errorCode = WSAStartup(VERSIONREQUESTED, &this->wsaData);
	// If there was an error, throw an exception
	if(errorCode)
		throw errorCode;
    #endif
	// If there was an error, throw an exception
	return true;
}

// Create a TCP socket
bool Networking::Client::CreateClientTCPSocket(char* _pHost, int _pPort)
{
	// Clear the address info structure
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	// Set the socket family to AF_INET (IPv4)
	Networking::Client::SetFamily(AF_INET);
	// Set the socket type to SOCK_STREAM (TCP)
	Networking::Client::SetSocketType(SOCK_STREAM);
	// Set the socket protocol to IPPROTO_TCP (TCP)
	Networking::Client::SetProtocol(IPPROTO_TCP);

	// Get the address info for the specified host and port
	int errorCode = getaddrinfo((PCSTR)_pHost, (PCSTR)getservbyport(_pPort, NULL)->s_name,(const addrinfo*) &addressInfo,&hostAddressInfo );

// If there was an error, throw an exception
	if(errorCode)
	{
    #ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
    #endif
		// Throw the error code
		throw errorCode;
	}

	// Create a socket using the address info
	connectionSocket = socket(hostAddressInfo->ai_family, hostAddressInfo->ai_socktype,  hostAddressInfo->ai_protocol);

	// If the socket is invalid, throw an exception
	if(INVALIDSOCKET(connectionSocket))
	{
		// Get the error code
		int errorCode = GETERROR();
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}
	// Return true if the socket was created successfully
	return true;
}

bool Networking::Client::CreateClientUDPSocket(char* _pHost, int _pPort)
{
	// Clear the address info structure
	ZeroMemory(&addressInfo, sizeof(addressInfo));
	// Set the socket family to AF_INET (IPv4)
	Networking::Client::SetFamily(AF_INET);
	Networking::Client::SetSocketType(SOCK_DGRAM); // Use SOCK_DGRAM for UDP sockets
	Networking::Client::SetProtocol(IPPROTO_UDP);

	// Get the address info for the specified host and port
	int errorCode = getaddrinfo((PCSTR)_pHost, (PCSTR)getservbyport(_pPort, NULL)->s_name,(const addrinfo*) &addressInfo,&hostAddressInfo );

// If there was an error, throw an exception
	if(errorCode)
	{
    #ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
    #endif
		// Throw the error code
		throw errorCode;
	}

	// Create the UDP socket
	connectionSocket = socket(hostAddressInfo->ai_family, hostAddressInfo->ai_socktype,  hostAddressInfo->ai_protocol);

	// If the socket is invalid, throw an exception
	if(INVALIDSOCKET(connectionSocket))
	{
		// Get the error code
		int errorCode = GETERROR();

	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}
	// Return true if the UDP socket was created successfully
	return true;
}

bool Networking::Client::CreateClientSocket(char* _pHost, int _pPort)
{
	int errorCode = getaddrinfo((PCSTR)_pHost, (PCSTR)getservbyport(_pPort, NULL)->s_name,(const addrinfo*) &addressInfo,&hostAddressInfo );

	if(errorCode)
	{
    #ifdef _WIN32
		WSACleanup();
    #endif
		throw errorCode;
	}

	connectionSocket = socket(hostAddressInfo->ai_family, hostAddressInfo->ai_socktype,  hostAddressInfo->ai_protocol);

	if(INVALIDSOCKET(connectionSocket))
	{
		int errorCode = GETERROR();
	#ifdef _WIN32
		WSACleanup();
	#endif
		throw errorCode;
	}
	return true;
}


// Connect to the server
bool Networking::Client::ConnectClientSocket()
{
	// Connect to the server using the address info
	int errorCode= connect(connectionSocket, (sockaddr*) &hostAddressInfo->ai_addr, hostAddressInfo->ai_addrlen);

// If there was an error, throw an exception
	if (errorCode)
	{
		// Get the error code
		errorCode = GETERROR();
		CLOSESOCKET(connectionSocket);
    #ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
    #endif
		// Throw the error code
		throw errorCode;
	}

	clientIsConnected = true;
// Return true if the connection was successful
	return true;
}

void Networking::Client::SetSocketType(int _pSocketType)
{
	addressInfo.ai_socktype = _pSocketType;
}

void Networking::Client::SetFamily(int _pFamily)
{
	addressInfo.ai_family = _pFamily;
}

void Networking::Client::SetProtocol(int _pProtocol)
{
	addressInfo.ai_protocol = _pProtocol;
}


// Send data to the server
int Networking::Client::Send(char* _pSendBuffer)
{
	// Send the data to the server
	int bytesSent = send(connectionSocket,_pSendBuffer, strlen(_pSendBuffer),0 );

	// If there was an error, throw an exception
	if(bytesSent ==SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return  the number of bytes sent if the data was sent successfully
	return bytesSent;
}

// Send data to a specified address and port
int Networking::Client::SendTo(char* _pBuffer, char* _pAddress, int _pPort)
{
	// Create a sockaddr_in structure to hold the address and port of the recipient
	sockaddr_in recipient;

	// Zero out the sockaddr_in structure
	ZeroMemory(&recipient, sizeof(recipient));

	// Set the address family, port, and address of the recipient
	recipient.sin_family = AF_INET;
	recipient.sin_port = htons(_pPort);
	inet_pton(AF_INET, _pAddress, &recipient.sin_addr);

	// Send the data to the specified recipient
	int bytesSent = sendto(connectionSocket, _pBuffer, strlen(_pBuffer), 0, (sockaddr*)&recipient, sizeof(recipient));

	// If there was an error, throw an exception
	if(bytesSent == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return  the number of bytes sent if the data was sent successfully
	return bytesSent;
}


// Send a file to the server
void Networking::Client::SendFile(const std::string& _pFilePath)
{
	// Open the file for reading
	std::ifstream file(_pFilePath, std::ios::in | std::ios::binary);

	// Check if the file was opened successfully
	if(!file)
	{
		// Throw an exception if the file could not be opened
		throw std::runtime_error("Error: Unable to open file '" + _pFilePath + "'");
	}

	// Read the file data into a buffer
	std::vector<char> fileData((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

	// Send the file data to the server
	Send(&fileData[0]);
}


// Receive data from the server
std::vector <char> Networking::Client::Receive()
{
	// Initialize the number of bytes received to 0
	int bytesReceived =0;

	// Create a vector to store the received data
	std::vector<char> receiveBuffer;

	// Receive data from the server in a loop
	do{
		// Get the current size of the receive buffer
		int bufferStart = receiveBuffer.size();
		// Resize the buffer to make room for more data
		receiveBuffer.resize(bufferStart+512);
		// Receive data from the server
		bytesReceived = recv(connectionSocket, &receiveBuffer[bufferStart],512,0);
		// Resize the buffer to the actual size of the received data
		receiveBuffer.resize(bufferStart + bytesReceived);
	} while (bytesReceived == 512);

// If there was an error, throw an exception
	if (bytesReceived == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
    #ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
    #endif
		// Throw the error code
		throw errorCode;
	}
	// Return the vector containing the received data
	return receiveBuffer;
}


// Receive data from a specified address and port
std::vector<char> Networking::Client::ReceiveFrom(char* _pAddress, int _pPort)
{
	// Initialize the number of bytes received to 0
	int bytesReceived =0;

	// Create a sockaddr_in structure to hold the address and port of the sender
	sockaddr_in sender;

	// Zero out the sockaddr_in structure
	ZeroMemory(&sender, sizeof(sender));

	// Set the address family, port, and address of the sender
	sender.sin_family = AF_INET;
	sender.sin_port = htons(_pPort);
	inet_pton(AF_INET, _pAddress, &sender.sin_addr);

	// Receive data from the specified sender
	std::vector<char> receiveBuffer;


	// Receive data from the server in a loop
	do{
		// Get the current size of the receive buffer
		int bufferStart = receiveBuffer.size();
		// Resize the buffer to make room for more data
		receiveBuffer.resize(bufferStart+512);
		// Receive data from the server
		bytesReceived = recvfrom(connectionSocket, &receiveBuffer[bufferStart], 512, 0, (sockaddr*)&sender, (socklen_t*)sizeof(sender));
		// Resize the buffer to the actual size of the received data
		receiveBuffer.resize(bufferStart + bytesReceived);
	} while (bytesReceived == 512);


	// If there was an error, throw an exception
	if(bytesReceived == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return the received data
	return receiveBuffer;
}

// Receive a file from the server
void Networking::Client::ReceiveFile(const std::string& _pFilePath)
{
	// Open the file for writing
	std::ofstream file(_pFilePath, std::ios::out | std::ios::binary);

	// Check if the file was opened successfully
	if(!file)
	{
		// Throw an exception if the file could not be opened
		throw std::runtime_error("Error: Unable to open file '" + _pFilePath + "'");
	}

	// Receive the file data from the server
	std::vector<char> fileData = Receive();

	// Write the file data to the file
	file.write(&fileData[0], fileData.size());
}


// Disconnect from the server and close the client socket
bool Networking::Client::Disconnect()
{
	// Disconnect from the server
	int errorCode = shutdown(connectionSocket, SHUT_RDWR);

	// If there was an error, throw an exception
	if(errorCode)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Close the client socket
	errorCode = CLOSESOCKET(connectionSocket);

	// If there was an error, throw an exception
	if(errorCode)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}
	clientIsConnected = false;
	// Return true if the client was disconnected and the socket was closed successfully
	return true;
}

//Returns whether the client is currently connected to a host.
bool Networking::Client::IsConnected()
{
	return clientIsConnected;
}

// Get the hostname of the client
std::string Networking::Client::GetHostName()
{
	// Get the hostname of the client
	char hostname[128];
	if(gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return the hostname of the client
	return hostname;
}


// Get the hostname of the server
std::string Networking::Client::GetServerHostName()
{
	// Get the address info of the server
	addrinfo* serverAddressInfo = hostAddressInfo;

	// Get the hostname of the server
	char hostname[128];
	if(getnameinfo(serverAddressInfo->ai_addr, serverAddressInfo->ai_addrlen, hostname, sizeof(hostname), NULL, 0, 0) == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return the hostname of the server
	return hostname;
}

// Get the local IP address of the client
std::string Networking::Client::GetLocalIPAddress()
{
	// Get the local IP address of the client
	addrinfo* localAddress;
	if(getaddrinfo("localhost", NULL, NULL, &localAddress) == SOCKET_ERROR)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Convert the local IP address to a string
	char localIP[128];
	if(inet_ntop(AF_INET, &((sockaddr_in*)localAddress->ai_addr)->sin_addr, localIP, sizeof(localIP)) == NULL)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return the local IP address of the client
	return localIP;
}

// Get the remote IP address of the server
std::string Networking::Client::GetRemoteIPAddress()
{
	// Get the address info of the server
	addrinfo* serverAddressInfo = hostAddressInfo;

	// Convert the remote IP address to a string
	char remoteIP[128];
	if(inet_ntop(AF_INET, &((sockaddr_in*)serverAddressInfo->ai_addr)->sin_addr, remoteIP, sizeof(remoteIP)) == NULL)
	{
		// Get the error code
		int errorCode = GETERROR();

		// Close the socket
		CLOSESOCKET(connectionSocket);
	#ifdef _WIN32
		// Clean up the Windows Sockets DLL
		WSACleanup();
	#endif
		// Throw the error code
		throw errorCode;
	}

	// Return the remote IP address of the server
	return remoteIP;
}
