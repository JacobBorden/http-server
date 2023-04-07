#include <gtest/gtest.h>
#include "http.h"
#include "server.h"

// Test case for parsing an HTTP request
TEST(HTTPServerTests, ParseHttpRequest) {
    std::string raw_request =
        "GET /index.html HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: TestAgent/1.0\r\n"
        "Accept: text/html\r\n"
        "\r\n";

    HTTP::HTTPREQUEST request = HTTP::ParseHttpRequest(raw_request);

    ASSERT_EQ(request.method, "GET");
    ASSERT_EQ(request.uri, "/index.html");
    ASSERT_EQ(request.protocol, "HTTP/1.1");
    ASSERT_EQ(request.headers["Host"], "www.example.com");
    ASSERT_EQ(request.headers["User-Agent"], "TestAgent/1.0");
    ASSERT_EQ(request.headers["Accept"], "text/html");
}

// Test case for generating an HTTP response
TEST(HTTPServerTests, GenerateResponse) {
    HTTP::HTTPREQUEST request;
    request.method = "GET";
    request.uri = "/index.html";
    request.protocol = "HTTP/1.1";
    request.headers["Host"] = "www.example.com";
    request.headers["User-Agent"] = "TestAgent/1.0";
    request.headers["Accept"] = "text/html";

    std::string response = HTTP::GenerateResponse(request);
    std::string expected_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 95\r\n"
        "\r\n"
        "<!DOCTYPE html><html><head><title>Test</title></head><body><h1>Hello, world!</h1></body></html>\r\n";

    ASSERT_EQ(response, expected_response);
}

// Add more test cases as needed

