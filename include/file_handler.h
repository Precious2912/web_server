#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>

// Returns false if file not found or path is suspicious
bool read_file(const std::string& path, std::string& contents);
std::string mime_type(const std::string& path);

#endif