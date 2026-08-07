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

// Test case for getting mime type
TEST(HTTPServerTests, GetMimeType) {
    ASSERT_EQ(HTTP::GetMimeType("file.html"), "text/html");
    ASSERT_EQ(HTTP::GetMimeType("file.json"), "application/json");
    ASSERT_EQ(HTTP::GetMimeType("doc.pdf"), "application/pdf");
    ASSERT_EQ(HTTP::GetMimeType("readme.txt"), "text/plain");
    ASSERT_EQ(HTTP::GetMimeType("vector.svg"), "image/svg+xml");
    ASSERT_EQ(HTTP::GetMimeType("archive.zip"), "application/zip");
    ASSERT_EQ(HTTP::GetMimeType("unknown.xyz"), "application/octet-stream");
    ASSERT_EQ(HTTP::GetMimeType("noextensionfile"), "application/octet-stream");
}

// Add more test cases as needed

