#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <string>
#include <optional>

std::optional<std::string> read_file(const std::string& path);
std::string mime_type(const std::string& path);

#endif