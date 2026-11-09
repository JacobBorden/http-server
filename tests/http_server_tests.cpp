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
        "<!DOCTYPE html><html><head><title>Test</title></head><body><h1>Hello, world!</h1></body></html>";

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

TEST(HTTPServerTests, MalformedRequestLineReturnsBadRequest) {
    for (const std::string& raw : {
             "GET /index.html\r\n\r\n",
             "GET  HTTP/1.1\r\n\r\n",
             "GET /index.html HTTP/1.1 extra\r\n\r\n",
             "\r\n\r\n"}) {
        const auto response = HTTP::GenerateResponse(HTTP::ParseHttpRequest(raw));
        EXPECT_EQ(response.find("HTTP/1.1 400 Bad Request\r\n"), 0u) << raw;
    }
}

TEST(HTTPServerTests, UnsupportedMethodStillReturnsMethodNotAllowed) {
    const auto response = HTTP::GenerateResponse(
        HTTP::ParseHttpRequest("PATCH /index.html HTTP/1.1\r\n\r\n"));
    EXPECT_EQ(response.find("HTTP/1.1 405 Method Not Allowed\r\n"), 0u);
}

TEST(HTTPServerTests, WhitespaceOnlyHeaderReturnsBadRequest) {
    const auto response = HTTP::GenerateResponse(HTTP::ParseHttpRequest(
        "GET /index.html HTTP/1.1\r\n \r\n\r\n"));
    EXPECT_EQ(response.find("HTTP/1.1 400 Bad Request\r\n"), 0u);
}

TEST(HTTPServerTests, HeaderWithoutColonReturnsBadRequest) {
    const auto response = HTTP::GenerateResponse(HTTP::ParseHttpRequest(
        "GET /index.html HTTP/1.1\r\nHost example.com\r\n\r\n"));
    EXPECT_EQ(response.find("HTTP/1.1 400 Bad Request\r\n"), 0u);
}

TEST(HTTPServerTests, HeaderValueNeedsNoLeadingSpace) {
    const auto request = HTTP::ParseHttpRequest(
        "GET /index.html HTTP/1.1\r\nHost:example.com\r\nReferer:https://example.com/a\r\n\r\n");
    EXPECT_EQ(request.protocol, "HTTP/1.1");
    EXPECT_EQ(request.headers.at("Host"), "example.com");
    EXPECT_EQ(request.headers.at("Referer"), "https://example.com/a");
}

TEST(HTTPServerTests, HeaderTabWhitespace) {
    const auto request = HTTP::ParseHttpRequest(
        "GET /index.html HTTP/1.1\r\nHost:\t example.com \t\r\n\r\n");
    EXPECT_EQ(request.protocol, "HTTP/1.1");
    EXPECT_EQ(request.headers.at("Host"), "example.com");

    const auto response = HTTP::GenerateResponse(HTTP::ParseHttpRequest(
        "GET /index.html HTTP/1.1\r\nHost\t: example.com\r\n\r\n"));
    EXPECT_EQ(response.find("HTTP/1.1 400 Bad Request\r\n"), 0u);
}

// Add more test cases as needed


TEST(HTTPServerTests, BinaryResponsePreservesLength) {
    HTTP::HTTPREQUEST request;
    request.method = "GET";
    request.uri = "/binary.txt";
    request.protocol = "HTTP/1.1";
    std::string response = HTTP::GenerateResponse(request);
    const auto split = response.find("\r\n\r\n");
    ASSERT_NE(split, std::string::npos);
    EXPECT_NE(response.find("Content-Length: 3\r\n"), std::string::npos);
    EXPECT_EQ(response.substr(split + 4), std::string("a\0b", 3));
}
