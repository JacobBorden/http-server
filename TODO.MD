# ToDo

## Existing Items
- [ ] add support for multiple connections (Current implementation is detached thread per connection, consider thread pool)
- [ ] build out http request parsing (Current implementation uses fragile stringstream logic)
- [ ] add more types to mime look up

## Newly Identified Improvements
- [ ] Implement a Thread Pool to replace the `std::thread(...).detach()` model for better scalability and resource management.
- [ ] Enhance HTTP parsing to support chunked transfer encoding and larger bodies.
- [ ] Sanitize the requested URI in `HandleGetRequest` to prevent directory traversal attacks (e.g., path normalization before fetching the file).
- [ ] Add configuration options (port, host, public directory) via command-line arguments or a config file instead of hardcoded values.
- [ ] Add support for proper logging framework rather than console output, expanding on the `server.LogToFile` usage.
- [ ] Implement proper handlers for POST, PUT, and DELETE methods beyond placeholder responses.
