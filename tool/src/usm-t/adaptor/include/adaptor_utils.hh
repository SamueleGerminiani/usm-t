#pragma once

#include <string>
namespace usmt {
/**
 * Scans a directory for CSVs.
 * If max_length is > 0: splits the total allowed data rows among the valid CSVs (N)
 * and truncates them to (max_length / N) + 1 lines.
 * If max_length is <= 0: copies the full files.
 */
void copyCSV_trim_length(const std::string &in_path_str,
                         const std::string &out_path_str,
                         long long max_length = -1);
} // namespace usmt
