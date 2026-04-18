#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include "form_handler.h"
#include "logger.h"
#include "utils.h"

std::optional<FormData> parse_form_body(const std::string& body) {
    FormData data;

    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        if (pair.empty()) continue;

        auto eq = pair.find('=');
        if (eq == std::string::npos) return std::nullopt; // key with no value is malformed

        std::string key = url_decode(pair.substr(0, eq));
        std::string value = url_decode(pair.substr(eq + 1));

        if (key.empty()) return std::nullopt;

        data[sanitise_field(key)] = sanitise_field(value);
    }

    return data;
}

std::string sanitise_field(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (unsigned char c : value) {
        // Allow printable ASCII only — no control chars, no extended bytes
        if (c >= 32 && c < 127) {
            result += static_cast<char>(c);
        }
    }

    // Trim leading/trailing whitespace
    auto start = result.find_first_not_of(" \t");
    auto end = result.find_last_not_of(" \t");

    if (start == std::string::npos) return "";
    return result.substr(start, end - start + 1);
}

bool save_form_data(const FormData& data) {
    std::filesystem::create_directories("./data");

    // Timestamp as filename so submissions don't overwrite each other
    auto now = std::chrono::system_clock::now();
    auto ts = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();

    std::string path = "./data/submission_" + std::to_string(ts) + ".txt";

    std::ofstream file(path);
    if (!file.is_open()) return false;

    for (const auto& [key, value] : data) {
        file << key << ": " << value << "\n";
    }

    return true;
}

/* IPC Protocol
  Simple length-prefixed framing:
  [4 bytes: body length as uint32_t][body bytes]
  This lets the receiver know exactly how many bytes to read without
  needing a delimiter that might appear in the body itself. 
*/

static bool write_all(int fd, const void* buf, size_t len) {
    const char* ptr = static_cast<const char*>(buf);
    while (len > 0) {
        ssize_t written = send(fd, ptr, len, 0);
        if (written <= 0) return false;
        ptr += written;
        len -= written;
    }
    return true;
}

static bool read_all(int fd, void* buf, size_t len) {
    char* ptr = static_cast<char*>(buf);
    while (len > 0) {
        ssize_t bytes = recv(fd, ptr, len, 0);
        if (bytes <= 0) return false;
        ptr += bytes;
        len -= bytes;
    }
    return true;
}

bool send_to_form_handler(int ipc_fd, const std::string& body) {
    // Send length prefix first so the child knows how much to read
    uint32_t len = static_cast<uint32_t>(body.size());
    if (!write_all(ipc_fd, &len, sizeof(len))) return false;
    if (!write_all(ipc_fd, body.c_str(), len)) return false;
    return true;
}

void form_handler_loop(int ipc_fd) {
    // This is the child process — it just sits here waiting for work.
    // The parent sends POST bodies over the socket; we parse and save them.
    while (true) {
        uint32_t len = 0;
        if (!read_all(ipc_fd, &len, sizeof(len))) {
            // Parent closed the connection or server is shutting down
            break;
        }

        if (len == 0 || len > MAX_BODY_SIZE) {
            LOG_WARN("[form handler] rejected bad length: " + std::to_string(len));
            std::cerr << "[form handler] bad length: " << len << std::endl;
            continue;
        }

        std::string body(len, '\0');
        if (!read_all(ipc_fd, body.data(), len)) break;

        auto form_data = parse_form_body(body);
        if (!form_data) {
            LOG_WARN("[form handler] failed to parse body");
            std::cerr << "[form handler] failed to parse body" << std::endl;
            continue;
        }

        if (!save_form_data(*form_data)) {
            LOG_ERROR("[form handler] failed to save submission to disk");
            std::cerr << "[form handler] failed to save submission" << std::endl;
        } else {
            LOG_INFO("[form handler] submission saved");
            std::cerr << "[form handler] submission saved" << std::endl;

        }
    }

    close(ipc_fd);
}