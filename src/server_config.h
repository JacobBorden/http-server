#pragma once

#include <cstdint>
#include <string>

// Parse configuration before constructing the listening socket.
inline bool ParseServerPort(int argc, const char* const argv[], std::uint16_t& port,
                            std::string& error) {
    port = 80;
    error.clear();
    if (argc == 1) return true;
    if (argc != 3 || std::string(argv[1]) != "--port") {
        error = "Usage: server.exe [--port 1..65535]";
        return false;
    }

    const std::string value(argv[2]);
    if (value.empty()) {
        error = "Port must be a decimal number from 1 to 65535";
        return false;
    }
    unsigned int parsed = 0;
    for (char digit : value) {
        if (digit < '0' || digit > '9') {
            error = "Port must be a decimal number from 1 to 65535";
            return false;
        }
        parsed = parsed * 10 + static_cast<unsigned int>(digit - '0');
        if (parsed > 65535) {
            error = "Port must be a decimal number from 1 to 65535";
            return false;
        }
    }
    if (parsed == 0) {
        error = "Port must be a decimal number from 1 to 65535";
        return false;
    }
    port = static_cast<std::uint16_t>(parsed);
    return true;
}
