#include "utils.h"
#include <algorithm>

// Decodes %XX
// Without this, /%2e%2e/etc/passwd sails right past has_traversal().
std::string url_decode(const std::string& s) {
    std::string result;
    result.reserve(s.size());

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '+') {
            result += ' ';
        } else if (s[i] == '%' && i + 2 < s.size()) {
            std::string hex = s.substr(i + 1, 2);
            bool valid = std::all_of(hex.begin(), hex.end(), ::isxdigit);
            if (valid) {
                char decoded = static_cast<char>(std::stoi(hex, nullptr, 16));
                if (decoded == '\0') return ""; // %00 is always an attack
                result += decoded;
                i += 2;
            } else {
                result += s[i];
            }
        } else {
            result += s[i];
        }
    }
    return result;
}