#include "http.h"
#include <algorithm>
#include <locale>

namespace {
bool IsHttpVersion(const std::string& version)
{
	return version.size() == 8 && version.compare(0, 5, "HTTP/") == 0 &&
		version[5] >= '0' && version[5] <= '9' && version[6] == '.' &&
		version[7] >= '0' && version[7] <= '9';
}

bool IsHttpTokenCharacter(unsigned char c)
{
	static const std::string punctuation = "!#$%&'*+-.^_`|~";
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
		(c >= '0' && c <= '9') ||
		punctuation.find(c) != std::string::npos;
}
std::string TrimFrom(const std::string& str, std::size_t offset)
{
	const char* whitespace = " \t\r";
	const auto start = str.find_first_not_of(whitespace, offset);
	if (start == std::string::npos)
		return "";

	const auto end = str.find_last_not_of(whitespace);
	return str.substr(start, end - start + 1);
}
}

std::string trim(const std::string& str)
{
	return TrimFrom(str, 0);
}

HTTP::HTTPREQUEST HTTP::ParseHttpRequest(std::string _pRequest)
{
	HTTPREQUEST request;
	std::istringstream requestStream(_pRequest);
	std::string requestLine;
	if (!std::getline(requestStream, requestLine, '\n') || requestStream.eof())
		return request;
	if (!requestLine.empty() && requestLine.back() == '\r')
		requestLine.pop_back();

	if (requestLine.empty() || requestLine.front() == ' ' || requestLine.front() == '\t' ||
		requestLine.back() == ' ' || requestLine.back() == '\t') {
		return request;
	}

	if (std::any_of(requestLine.begin(), requestLine.end(), [](unsigned char c) {
		return (c < 0x20 && c != '\t') || c == 0x7f;
	})) {
		return request;
	}

	std::istringstream requestLineStream (requestLine);
	requestLineStream.imbue(std::locale::classic());
	std::string extraToken;
	if (!(requestLineStream >> request.method >> request.uri >> request.protocol) ||
		(requestLineStream >> extraToken)) {
		request.protocol.clear();
		return request;
	}
	if (!std::all_of(request.method.begin(), request.method.end(), IsHttpTokenCharacter) ||
		!IsHttpVersion(request.protocol)) {
		request.protocol.clear();
		return request;
	}
	std::string line;
	while(std::getline(requestStream, line, '\n') && line != "\r")
	{
		const auto colon = line.find(':');
		if (colon == std::string::npos || colon == 0 ||
			!std::all_of(line.begin(), line.begin() + colon, IsHttpTokenCharacter)) {
			request.protocol.clear();
			return request;
		}
		const auto valueStart = colon + 1;
		const auto valueEnd = line.back() == '\r' ? line.end() - 1 : line.end();
		if (std::any_of(line.begin() + valueStart, valueEnd, [](unsigned char c) {
			return (c < 0x20 && c != '\t') || c == 0x7f;
		})) {
			request.protocol.clear();
			return request;
		}
		request.headers[line.substr(0, colon)] = TrimFrom(line, valueStart);

	}

	// A complete header section ends with an empty CRLF line.
	if (!requestStream || requestStream.eof()) {
		request.protocol.clear();
		return request;
	}

	while(std::getline(requestStream, line, '\n'))
	{
		if(line !="\r")
			request.body +=line;
	}
	return request;
}

std::string HTTP::GenerateResponse(HTTP::HTTPREQUEST _pRequest)
{
	if (_pRequest.method.empty() || _pRequest.uri.empty() || _pRequest.protocol.empty())
		return GenerateErrorResponse("400 Bad Request", 400);

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
	httpResponse.body.assign(buffer, static_cast<size_t>(fileSize));
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
	std::string fileExtention = _pFilename.substr(_pFilename.find_last_of(".") + 1);
	if(fileExtention == "html")
		return "text/html";
	else if(fileExtention == "css")
		return "text/css";
	else if (fileExtention == "js")
		return "application/javascript";
	else if (fileExtention  == "jpg" || fileExtention == "jpeg")
		return "image/jpeg";
	else if (fileExtention == "png")
		return "image/png";
	else if (fileExtention == "gif")
		return "image/gif";
	else return "application/octet-stream";
}
