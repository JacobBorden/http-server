# AI Agent Guidelines

This project is a simple multithreaded C++ HTTP server built using a custom `NetworkingLibrary` and Google Test (`googletest`) for unit testing.

## Code Structure

- **`src/`**: Contains the source code.
  - `main.cpp`: Entry point. Sets up the server, listens for connections, and spawns a new thread for each client using `std::thread(HandleClientConnection, client).detach()`.
  - `http.h` / `http.cpp`: Core HTTP logic. Contains structs for `HTTPREQUEST` and `HTTPRESPONSE`, and functions for parsing requests, generating responses, and routing based on HTTP method (GET, POST, PUT, DELETE).
- **`tests/`**: Contains unit tests using Google Test.
  - `http_server_tests.cpp`: Includes tests for parsing HTTP requests and generating HTTP responses.
  - `test_resources/`: Contains test files such as `index.html`. These are copied to the build directory during CMake configuration.
- **`dependencies/`**: Contains external libraries included via `FetchContent` in CMake.
  - `googletest`: Google Test framework.
  - `my_networking_lib`: Custom networking library from GitHub.

## Building the Project

This project uses CMake.

1. Create a build directory: `mkdir -p bin && cd bin`
2. Run CMake: `cmake ..`
3. Build the project: `make`

The executable `server.exe` will be located in the `bin/` directory.

## Running Tests

Tests must be run from the `bin/tests/` directory to ensure relative paths for test resources (like `public/index.html`) resolve correctly.

1. Build the project first.
2. Navigate to the tests directory: `cd bin/tests/`
3. Run the tests: `./my_tests` or `ctest`

## Key Patterns and Constraints

- **Multithreading**: The server uses a "thread-per-connection" model via `std::thread(...).detach()`. This is not scalable for production but works for this simple implementation.
- **Networking**: Raw socket communication is handled by `NetworkingLibrary` (`Networking::Server` and `Networking::ClientConnection`).
- **HTTP Parsing**: Parsing is currently manual using `std::istringstream`. It is somewhat fragile and expects well-formed HTTP requests ending in `\r\n`.
- **Static Files**: GET requests try to serve files from a `public/` directory relative to the executable's current working directory.

## Project operations

Read `PROJECT_MANAGEMENT.md` for the repository baseline, prioritized management work, and verification requirements. Preserve existing TODO.MD and DESIGN.MD as the technical backlog and architecture sources.

Jacob authorized private Slack operations alerts on 2026-09-07. Send meaningful project changes, new blockers, recoveries, and required actions to #codex-ops (C0C01JWQ77Y). Suppress unchanged alerts, record confirmed delivery in task or automation memory, and retain the primary result if Slack is unavailable. Do not post credentials or raw private source material.
