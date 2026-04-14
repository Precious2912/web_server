#include "form_handler.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <algorithm>

std::optional<FormData> parse_form_body(const std::string& body) {
    FormData data;

    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        if (pair.empty()) continue;

        auto eq = pair.find('=');
        if (eq == std::string::npos) return std::nullopt; // key with no value is malformed

        std::string key   = url_decode(pair.substr(0, eq));
        std::string value = url_decode(pair.substr(eq + 1));

        if (key.empty()) return std::nullopt; // empty key is not something we accept

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
    auto end   = result.find_last_not_of(" \t");

    if (start == std::string::npos) return "";
    return result.substr(start, end - start + 1);
}

bool save_form_data(const FormData& data) {
    // Make sure ./data/ exists — create it if not
    std::filesystem::create_directories("./data");

    // Timestamp as filename so submissions don't overwrite each other
    auto now = std::chrono::system_clock::now();
    auto ts  = std::chrono::duration_cast<std::chrono::seconds>(
                   now.time_since_epoch()).count();

    std::string path = "./data/submission_" + std::to_string(ts) + ".txt";

    std::ofstream file(path);
    if (!file.is_open()) return false;

    for (const auto& [key, value] : data) {
        file << key << ": " << value << "\n";
    }

    return true;
}