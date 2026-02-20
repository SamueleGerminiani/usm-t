#include "adaptor_utils.hh"
#include "message.hh"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace usmt {
void copyCSV_trim_length(const std::string &in_path_str,
                         const std::string &out_path_str,
                         long long max_length) {
  std::filesystem::path in_path(in_path_str);
  std::filesystem::path out_path(out_path_str);

  // --- 1. Basic Directory Checks ---
  if (!std::filesystem::exists(in_path) ||
      !std::filesystem::is_directory(in_path)) {
    messageError(
        "(CSV trimmer) Input path is not a valid directory: " +
        in_path_str);
    return;
  }

  if (!std::filesystem::exists(out_path) ||
      !std::filesystem::is_directory(out_path)) {
    messageError(
        "(CSV trimmer) Output path is not a valid directory: " +
        out_path_str);
    return;
  }

  // --- 2. Identify Valid Files ---
  // A valid file must have at least 2 lines (Header + 1 Data Row)
  std::vector<std::filesystem::path> valid_files;

  try {
    for (const auto &entry :
         std::filesystem::directory_iterator(in_path)) {
      if (entry.is_regular_file() &&
          entry.path().extension() == ".csv") {
        std::ifstream file(entry.path());
        if (file.good()) {
          // Check for 2 lines without reading the whole file
          std::string line;
          int line_count = 0;
          while (line_count < 2 && std::getline(file, line)) {
            line_count++;
          }

          if (line_count >= 2) {
            valid_files.push_back(entry.path());
          }
        }
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    messageError(
        std::string("(CSV trimmer) Filesystem error during scan: ") +
        e.what());
    return;
  }

  size_t file_count = valid_files.size();
  if (file_count == 0) {
    messageError("(CSV trimmer) No valid CSV files found in input "
                 "directory: " +
                 in_path_str);
    return;
  }

  // --- 3. Determine Lines to Keep ---
  long long lines_to_keep = -1; // -1 indicates "Copy All"

  if (max_length > 0) {
    // Validation: Ensure enough rows for at least 1 data row per file
    if (max_length < static_cast<long long>(file_count)) {
      messageError(
          "(CSV trimmer) max_length is too small. Total rows (" +
          std::to_string(max_length) + ") cannot be split among " +
          std::to_string(file_count) + " files (min 1 row each).");
      return;
    }

    long long rows_per_file =
        max_length / file_count;       // Integer division
    lines_to_keep = rows_per_file + 1; // +1 to include header
  }

  // --- 4. Process Files ---
  for (const auto &src_path : valid_files) {
    std::filesystem::path dest_path = out_path / src_path.filename();

    // Case A: No Limit (Copy Full File)
    if (lines_to_keep <= 0) {
      try {
        std::filesystem::copy_file(
            src_path, dest_path,
            std::filesystem::copy_options::overwrite_existing);
      } catch (const std::filesystem::filesystem_error &e) {
        messageError("(CSV trimmer) Failed to copy " +
                     src_path.string() + ": " + e.what());
      }
    }
    // Case B: Truncate (Stream processing)
    else {
      std::ifstream in_file(src_path);
      std::ofstream out_file(dest_path);

      if (!in_file.is_open()) {
        messageError("(CSV trimmer) Could not open input file: " +
                     src_path.string());
        continue;
      }
      if (!out_file.is_open()) {
        messageError("(CSV trimmer) Could not create output file: " +
                     dest_path.string());
        continue;
      }

      std::string line;
      long long current_line = 0;

      // Read and write until we hit the limit
      while (current_line < lines_to_keep &&
             std::getline(in_file, line)) {
        out_file << line << "\n";
        current_line++;
      }

      in_file.close();
      out_file.close();
    }
  }
}

} // namespace usmt
