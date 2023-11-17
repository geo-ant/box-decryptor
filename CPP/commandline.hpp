#ifndef BC_DECRYPT_COMMANDLINE_HPP
#define BC_DECRYPT_COMMANDLINE_HPP

#include "CLI11/CLI11.hpp"
#include <exception>
#include <memory>
#include <optional>
#include <variant>

namespace commandline {

struct UserAccountInfo {
  std::string keyfile;
  std::string password;
};

struct SingleFileDecryptOptions {
  std::string encrypted_file;
  std::optional<std::string> output_file;
  UserAccountInfo user_account_info;
};

struct DirectoryDecriptOptions {
  bool recursive = false;
  std::string encrypted_directory;
  UserAccountInfo user_account_info;
};

using Options = std::variant<SingleFileDecryptOptions, DirectoryDecriptOptions>;

class Error : std::exception {
public:
  Error(int code) : code(code) {}

  char const *what() const noexcept override {
    // don't actually use what, use exit() which will give the return
    // code and prints a nice message to the terminal
    return "Error parsing commandline arguments";
  }

  int error_code() const { return code; }

private:
  int code;
};

/// return the strongly type options or thrown an exception
Options parse_commandline(int argc, char const *const *argv);

} // namespace commandline
#endif
