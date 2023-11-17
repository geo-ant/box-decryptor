#include "AESHelper.h"
#include "AccountData.h"
#include "Base64Helper.h"
#include "FileData.h"
#include "HashHelper.h"
#include "PBKDF2Helper.h"
#include "RSAHelper.h"
#include "commandline.hpp"
#include "util.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <variant>

static void perform_decryption(commandline::SingleFileDecryptOptions &&);
static void perform_decryption(commandline::DirectoryDecriptOptions &&);
template <typename Iter>
static void decrypt_dir(Iter directory_iter,
                        std::string const &decryptedPrivateKey);

int main(int argc, char *argv[]) {
  try {

    auto exe_name = std::string(argv[0]);
    auto options = commandline::parse_commandline(argc, argv);

    std::visit([](auto &&opt) { perform_decryption(std::move(opt)); },
               std::move(options));

  } catch (commandline::Error const &e) {
    // help message has already been printed
    return e.error_code();
  } catch (std::exception const &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return -1;
  }

  return 0;
}

static void
perform_decryption(commandline::SingleFileDecryptOptions &&options) {
  std::filesystem::path const keyfilePath(options.user_account_info.keyfile);
  std::filesystem::path const encryptedFilePath(options.encrypted_file);
  std::string const password(options.user_account_info.password);
  std::filesystem::path const outputFilePath =
      options.output_file.has_value()
          ? std::filesystem::path(options.output_file.value())
          : std::filesystem::path(encryptedFilePath).replace_extension();

  std::cout << "Decryption process started..." << std::endl;

  // ============================================
  // AES decryption of private key in .bckey file
  // =============================================
  std::string const decryptedPrivateKey =
      util::decrypt_private_key(keyfilePath, password);

  // =============================================
  // RSA decryption of file information (header)
  // =============================================
  auto const decryptedFileBytes =
      util::decrypt_file(encryptedFilePath, decryptedPrivateKey);

  // ... and write it to disk
  util::write_file(outputFilePath, decryptedFileBytes);

  std::cout << "...Successfully decrypted file\n";
}

static void perform_decryption(commandline::DirectoryDecriptOptions &&options) {
  std::filesystem::path const keyfilePath(options.user_account_info.keyfile);
  std::string const password(options.user_account_info.password);

  std::cout << "Directory Decryption process started...\n";

  auto const decryptedPrivateKey =
      util::decrypt_private_key(keyfilePath, password);

  if (options.recursive) {
    std::filesystem::recursive_directory_iterator directory_iter(
        options.encrypted_directory);
    decrypt_dir(directory_iter, decryptedPrivateKey);
  } else {
    std::filesystem::directory_iterator directory_iter(
        options.encrypted_directory);
    decrypt_dir(directory_iter, decryptedPrivateKey);
  }
}

template <typename Iter>
static void decrypt_dir(Iter directory_iter,
                        std::string const &decryptedPrivateKey) {
  static_assert(
      std::is_same_v<typename Iter::value_type,
                     std::filesystem::directory_entry>,
      "Iter must be an iterator over std::filesystem::directory_entry");

  std::vector<std::filesystem::directory_entry> failures;

  for (auto const &entry : directory_iter) {
    if (entry.is_regular_file() && entry.path().extension() == ".bc") {
      try {
        std::cout << "Decrypting file: " << entry.path() << '\n';
        auto const decryptedFileBytes =
            util::decrypt_file(entry.path(), decryptedPrivateKey);

        auto const outputFilePath =
            std::filesystem::path(entry.path()).replace_extension();

        util::write_file(outputFilePath, decryptedFileBytes);
      } catch (std::exception const &e) {
        std::cerr << "Error: " << e.what() << '\n';
        failures.push_back(entry);
      }
    }
  }
  if (failures.empty()) {
    std::cout << "All files (hopefully) successfully decrypted\n";
  } else {
    std::cerr << "The following files could not be decrypted:\n";
    for (auto const &entry : failures) {
      std::cerr << entry.path() << '\n';
    }
  }
}
