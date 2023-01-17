#include "http.h"

HTTP::HTTPREQUEST HTTP::ParseHttpRequest(std::string _pRequest)
{
	HTTPREQUEST request;
	std::istringstream requestStream(_pRequest);
	std::string requestLine;
	std::getline(requestStream, requestLine);
	std::istringstream requestLineStream (requestLine);
	requestLineStream >> request.method >> request.uri >> request.protocol;
	request.body = _pRequest.substr(requestLine.size());
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

std::string HTTP::HandleGetRequest(HTTP::HTTPREQUEST _pRequest)
{
	std::ostringstream responseStream;
	HTTP::HTTPRESPONSE httpResponse;
	httpResponse.protocol = _pRequest.protocol;
	httpResponse.statusCodeNumber = 200;
	httpResponse.reasonPhrase = statusCode.at(httpResponse.statusCodeNumber);
	httpResponse.contentType = "text/html";
	httpResponse.body = "<H1> Hello from the server</H1>";
	responseStream << httpResponse.protocol <<" " <<httpResponse.statusCodeNumber<<" "<<httpResponse.reasonPhrase<<"\r\n";
	responseStream<<"Content-Type: "<<httpResponse.contentType<<"\r\n";
	responseStream<<"Content-Length: "<< httpResponse.body.size()<<"\r\n";
	responseStream<<"\r\n";
	responseStream<<httpResponse.body;
	std::string response = responseStream.str();
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
	httpResponse.body = "<H1>Data Processed Successfully</H1>";
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
