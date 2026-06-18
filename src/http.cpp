#include "http.h"

std::string trim(const std::string& str)
{
	const std::string whitespace = " \r";
	size_t start = str.find_first_not_of(whitespace);
	size_t end = str.find_last_not_of(whitespace);

	if (start == std::string::npos) // No non-whitespace characters found
		return "";

	return str.substr(start, end - start + 1);
}


HTTP::HTTPREQUEST HTTP::ParseHttpRequest(std::string _pRequest)
{
	HTTPREQUEST request;
	std::istringstream requestStream(_pRequest);
	std::string requestLine;
	std::getline(requestStream, requestLine);
	std::istringstream requestLineStream (requestLine);
	requestLineStream >> request.method >> request.uri >> request.protocol;
	std::string line;
	while(std::getline(requestStream, line) && line != "\r")
	{
		std::istringstream lineStream(line);
		std::string headerName;
		std::string headerValue;
		lineStream >> headerName;
		if(headerName.substr(headerName.size()-1) == ":")
		{
			headerName = headerName.substr(0, headerName.size()-1);
			std::getline(lineStream, headerValue);
			request.headers[headerName] = trim(headerValue);
		}

	}

	while(std::getline(requestStream,line))
	{
		if(line !="\r")
			request.body +=line;
	}
	return request;
}

std::string HTTP::GenerateResponse(HTTP::HTTPREQUEST _pRequest)
{
	if (_pRequest.method == "GET")
		return HandleGetRequest(_pRequest);
	else if(_pRequest.method == "POST")
		return HandlePostRequest(_pRequest);
	else if(_pRequest.method == "PUT")
		return HandlePutRequest(_pRequest);
	else if( _pRequest.method == "DELETE")
		return HandleDeleteRequest(_pRequest);
	else
		return HandleInvalidMethod(_pRequest);

}

std::string HTTP::HandleInvalidMethod(HTTP::HTTPREQUEST _pRequest)
{
	return GenerateErrorResponse("405 Method Not Allowed", 405);
}

std::string HTTP::HandleGetRequest(HTTP::HTTPREQUEST _pRequest)
{
	if(_pRequest.uri.find("..") != std::string::npos)
		return GenerateErrorResponse("400 Bad Request", 400);

	if(_pRequest.uri == "/")
		_pRequest.uri = "/index.html";
	std::string filePath = "public" +_pRequest.uri;

	// check if the file exists
	std::ifstream file(filePath, std::ios::binary);
	if(!file.is_open())
		return GenerateErrorResponse("404 Not Found",404);

	// Get the file size
	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);

	//read file into a buffer
	char* buffer = new char[fileSize];
	file.read(buffer, fileSize);

	//Create the response
	std::ostringstream responseStream;
	HTTP::HTTPRESPONSE httpResponse;
	httpResponse.protocol = _pRequest.protocol;
	httpResponse.statusCodeNumber = 200;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = GetMimeType(filePath);
	httpResponse.body = buffer;
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< fileSize<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
	delete []  buffer;
	return response;
}

std::string HTTP::HandlePostRequest(HTTP::HTTPREQUEST _pRequest)
{

	//Process the data

	HTTP::HTTPRESPONSE httpResponse;
	std::ostringstream responseStream;
	httpResponse.protocol = _pRequest.protocol;
	httpResponse.statusCodeNumber = 200;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = "text/html";
	httpResponse.body = _pRequest.body;
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< httpResponse.body.size()<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
	return response;

}

std::string HTTP::HandlePutRequest(HTTP::HTTPREQUEST _pRequest)
{

	//Process the data

	HTTP::HTTPRESPONSE httpResponse;
	std::ostringstream responseStream;
	httpResponse.protocol = _pRequest.protocol;
	httpResponse.statusCodeNumber = 200;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = "text/html";
	httpResponse.body = "<H1>Data Processed Successfully</H1>";
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< httpResponse.body.size()<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
	return response;

}

std::string HTTP::HandleDeleteRequest(HTTP::HTTPREQUEST _pRequest)
{

	//Process the data

	HTTP::HTTPRESPONSE httpResponse;
	std::ostringstream responseStream;
	httpResponse.protocol = _pRequest.protocol;
	httpResponse.statusCodeNumber = 200;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = "text/html";
	httpResponse.body = "<H1>Data Processed Successfully</H1>";
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< httpResponse.body.size()<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
	return response;

}


std::string HTTP::GenerateErrorResponse(std::string _pErrorMessage, int _pErrorCode)
{
	HTTP::HTTPRESPONSE httpResponse;
	std::ostringstream responseStream;
	httpResponse.protocol = "HTTP/1.1";
	httpResponse.statusCodeNumber =_pErrorCode;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = "text/html";
	httpResponse.body = _pErrorMessage;
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< httpResponse.body.size()<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
	return response;


}

std::string HTTP::GetMimeType(std::string _pFilename)
{
	static const std::unordered_map<std::string, std::string> mimeTypes = {
		{"html", "text/html"},
		{"css", "text/css"},
		{"js", "application/javascript"},
		{"jpg", "image/jpeg"},
		{"jpeg", "image/jpeg"},
		{"png", "image/png"},
		{"gif", "image/gif"},
		{"json", "application/json"},
		{"xml", "application/xml"},
		{"pdf", "application/pdf"},
		{"zip", "application/zip"},
		{"txt", "text/plain"},
		{"csv", "text/csv"},
		{"svg", "image/svg+xml"},
		{"ico", "image/x-icon"},
		{"webp", "image/webp"},
		{"mp3", "audio/mpeg"},
		{"mp4", "video/mp4"}
	};

	size_t dotPos = _pFilename.find_last_of(".");
	if (dotPos == std::string::npos || dotPos == _pFilename.length() - 1)
		return "application/octet-stream";

	std::string fileExtension = _pFilename.substr(dotPos + 1);
	auto it = mimeTypes.find(fileExtension);
	if (it != mimeTypes.end()) {
		return it->second;
	}

	return "application/octet-stream";
}
