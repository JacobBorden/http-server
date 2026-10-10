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

TEST(HTTPServerTests, PreservesPostBodyBytes) {
    const std::string body("first\r\nsecond\n\0last", 19);
    const std::string raw_request =
        "POST /echo HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Length: 19\r\n"
        "\r\n" + body;

    const HTTP::HTTPREQUEST request = HTTP::ParseHttpRequest(raw_request);
    EXPECT_EQ(request.body, body);
    EXPECT_EQ(request.headers.at("Content-Length"), "19");

    const std::string response = HTTP::GenerateResponse(request);
    const std::string separator = "\r\n\r\n";
    const size_t body_start = response.find(separator);
    ASSERT_NE(body_start, std::string::npos);
    EXPECT_NE(response.find("Content-Length: 19\r\n"), std::string::npos);
    EXPECT_EQ(response.substr(body_start + separator.size()), body);
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

// Test case for directory traversal attempt
TEST(HTTPServerTests, DirectoryTraversal) {
    HTTP::HTTPREQUEST request;
    request.method = "GET";
    request.uri = "/../etc/passwd";
    request.protocol = "HTTP/1.1";
    request.headers["Host"] = "www.example.com";
    request.headers["User-Agent"] = "TestAgent/1.0";
    request.headers["Accept"] = "text/html";

    std::string response = HTTP::GenerateResponse(request);

    // Check that it starts with the expected error status
    ASSERT_TRUE(response.find("HTTP/1.1 400 Bad Request") != std::string::npos);
}

// Add more test cases as needed
