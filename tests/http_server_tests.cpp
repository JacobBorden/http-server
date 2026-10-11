#include <gtest/gtest.h>
#include "http.h"
#include "server.h"
#include "server_config.h"

TEST(ServerConfigTests, DefaultAndExplicitPort) {
    std::uint16_t port = 0;
    std::string error;
    const char* defaultArgs[] = {"server.exe"};
    EXPECT_TRUE(ParseServerPort(1, defaultArgs, port, error));
    EXPECT_EQ(port, 80);
    EXPECT_TRUE(error.empty());

    const char* minArgs[] = {"server.exe", "--port", "1"};
    EXPECT_TRUE(ParseServerPort(3, minArgs, port, error));
    EXPECT_EQ(port, 1);
    const char* maxArgs[] = {"server.exe", "--port", "65535"};
    EXPECT_TRUE(ParseServerPort(3, maxArgs, port, error));
    EXPECT_EQ(port, 65535);
}

TEST(ServerConfigTests, RejectsInvalidArguments) {
    std::uint16_t port = 0;
    std::string error;
    for (const char* value : {"", "0", "65536", "999999999999999999", "-1", "+2", "3x"}) {
        const char* args[] = {"server.exe", "--port", value};
        EXPECT_FALSE(ParseServerPort(3, args, port, error)) << value;
        EXPECT_FALSE(error.empty());
    }
    const char* missing[] = {"server.exe", "--port"};
    EXPECT_FALSE(ParseServerPort(2, missing, port, error));
    const char* extra[] = {"server.exe", "--port", "8080", "extra"};
    EXPECT_FALSE(ParseServerPort(4, extra, port, error));
    const char* unknown[] = {"server.exe", "--host", "localhost"};
    EXPECT_FALSE(ParseServerPort(3, unknown, port, error));
}

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

// Add more test cases as needed
