#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <optional>
#include <string>

std::optional<std::string> read_file(const std::string& path);
std::string mime_type(const std::string& path);

#endif