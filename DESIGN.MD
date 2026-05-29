# HTTP Server Design Document

## 1. Overview

This project implements a simple, multithreaded HTTP server in C++. It is capable of handling basic HTTP 1.1 requests, serving static files, and processing basic RESTful methods (GET, POST, PUT, DELETE). The server relies on a custom `NetworkingLibrary` for raw socket operations and uses `std::thread` to handle multiple concurrent connections.

## 2. Architecture

The architecture can be broken down into three main components:

1.  **Networking Layer:** Responsible for socket creation, binding, listening, accepting connections, and sending/receiving raw byte streams.
2.  **HTTP Processing Layer:** Responsible for parsing raw HTTP requests, determining the appropriate action, and formatting HTTP responses.
3.  **Concurrency Model:** Responsible for managing multiple simultaneous client connections.

### 2.1 Networking Layer (`NetworkingLibrary`)

The server uses an external library, `NetworkingLibrary`, which abstracts the platform-specific socket API.

-   **`Networking::Server`**: Instantiated to listen on a specific port (default 80) and IP protocol (IPv4). It provides methods to `Accept()` new connections, `Receive()` data from a client, `Send()` data to a client, and `DisconnectClient()`.
-   **`Networking::ClientConnection`**: Represents an active connection to a client.

### 2.2 HTTP Processing Layer (`src/http.cpp`)

This layer parses the incoming raw string from the networking layer and generates an appropriate response string.

-   **Parsing (`ParseHttpRequest`)**:
    -   Reads the request line to extract the Method (e.g., `GET`), URI (e.g., `/index.html`), and Protocol (e.g., `HTTP/1.1`).
    -   Reads header lines, splitting them by `:` and storing them in an `std::unordered_map`.
    -   Reads the remaining body.
-   **Routing (`GenerateResponse`)**:
    -   Inspects the HTTP method and delegates to specific handlers (`HandleGetRequest`, `HandlePostRequest`, etc.).
-   **Response Generation**:
    -   **GET Requests**: Maps the URI to a local file path within a `public/` directory. It reads the file, determines the MIME type based on the file extension, and constructs an HTTP 200 OK response with the file contents. If the file is not found, it returns a 404 Not Found.
    -   **POST/PUT/DELETE Requests**: Currently implement placeholder responses echoing basic success messages or the sent body.
    -   **Error Handling**: `GenerateErrorResponse` constructs valid HTTP error responses (e.g., 405 Method Not Allowed, 404 Not Found).

### 2.3 Concurrency Model

The server employs a **thread-per-connection** concurrency model.

1.  The main thread runs an infinite loop, calling `server.Accept()`. This call blocks until a new client connects.
2.  Upon accepting a connection, a new `std::thread` is spawned, passing the `HandleClientConnection` function and the client connection object.
3.  The thread is immediately detached (`clientThread.detach()`), allowing it to run independently of the main thread.
4.  The client thread reads the request, processes it, sends the response, and disconnects, then terminates.

## 3. Data Structures

The core data structures used in the HTTP layer are simple `structs`:

```cpp
struct HTTPREQUEST {
	std::string method;
	std::string uri;
	std::string protocol;
	std::unordered_map<std::string,std::string> headers;
	std::string body;
};

struct HTTPRESPONSE {
	std::string protocol;
	int statusCodeNumber;
	std::string reasonPhrase;
	std::string contentType;
	std::string body;
};
```

## 4. Limitations and Future Improvements

*   **Concurrency**: The thread-per-connection model does not scale well to thousands of concurrent connections due to thread creation overhead and memory usage. A Thread Pool or asynchronous I/O (e.g., using `epoll` or `kqueue`) would be more robust.
*   **HTTP Parsing**: The current stringstream-based parsing is fragile and assumes well-formed, line-terminated headers. It does not handle chunked transfer encoding or large streaming bodies.
*   **Security**: There is no protection against directory traversal attacks in the GET handler (e.g., requesting `/../../etc/passwd`). The URI parsing needs sanitization.
*   **Configuration**: Port, host, and public directory are hardcoded. These should be configurable via command-line arguments or a configuration file.
