#pragma once
#ifndef _HTTP_
#define _HTTP_

#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <fstream>
#include <iostream>
namespace HTTP {

const std::unordered_map <int, std::string> statusCode = {
	{200, "OK"},
	{201, "Created"},
	{204, "No Content"},
	{400, "Bad Request"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{500, "Internal Server Error"}
};

struct HTTPREQUEST
{
	std::string method;
	std::string uri;
	std::string protocol;
	std::unordered_map<std::string,std::string> headers;
	std::string body;
};

struct HTTPRESPONSE
{
	std::string protocol;
	int statusCodeNumber;
	std::string reasonPhrase;
	std::string contentType;
	std::string body;
};

HTTPREQUEST ParseHttpRequest(std::string _pRequest);
std::string GenerateResponse(HTTPREQUEST _pRequest);
std::string HandleGetRequest(HTTPREQUEST _pRequest);
std::string HandlePostRequest(HTTPREQUEST _pRequest);
std::string HandlePutRequest(HTTPREQUEST _pRequest);
std::string HandleDeleteRequest(HTTPREQUEST _pRequest);
std::string HandleInvalidMethod(HTTPREQUEST _pRequest);
std::string GenerateErrorResponse(std::string _pErrorMessage, int _pErrorCode);
std::string GetMimeType(std::string _pFilename);
}

#endif

