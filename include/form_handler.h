#ifndef FORM_HANDLER_H
#define FORM_HANDLER_H

#include <string>
#include <unordered_map>
#include <optional>

// URL-decoded, sanitised key=value pairs from the form body
using FormData = std::unordered_map<std::string, std::string>;

// Returns nullopt if parsing fails (malformed encoding etc.)
std::optional<FormData> parse_form_body(const std::string& body);

// Strips anything that isn't plain text — no HTML, no control chars
std::string sanitise_field(const std::string& value);

// Persists form data as a timestamped file under ./data/
// Returns false if the write fails
bool save_form_data(const FormData& data);

// Child process entry point — sits in a loop reading POST bodies
// from the IPC socket and writing them to disk
void form_handler_loop(int ipc_fd);

// Parent side — sends a POST body to the child for processing.
// Returns false if the send fails.
bool send_to_form_handler(int ipc_fd, const std::string& body);

#endif