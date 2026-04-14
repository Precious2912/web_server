#ifndef FORM_HANDLER_H
#define FORM_HANDLER_H

#include <string>
#include <unordered_map>
#include <optional>

static const size_t MAX_BODY_SIZE = 1024 * 1024; // 1MB hard cap

// URL-decoded, sanitised key=value pairs from the form body
using FormData = std::unordered_map<std::string, std::string>;

// Returns nullopt if parsing fails (malformed encoding etc.)
std::optional<FormData> parse_form_body(const std::string& body);

// Strips anything that isn't plain text — no HTML, no control chars
std::string sanitise_field(const std::string& value);

// Persists form data as a timestamped file under ./data/
// Returns false if the write fails
bool save_form_data(const FormData& data);

#endif