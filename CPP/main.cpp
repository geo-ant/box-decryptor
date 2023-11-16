#include "AESHelper.h"
#include "AccountData.h"
#include "Base64Helper.h"
#include "FileData.h"
#include "HashHelper.h"
#include "PBKDF2Helper.h"
#include "RSAHelper.h"
#include "commandline.hpp"
#include "util.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <variant>

static void perform_decryption(commandline::SingleFileDecryptOptions &&);
static void perform_decryption(commandline::DirectoryDecriptOptions &&);

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

  // collect information about the user account
  AccountData const accountInfo(keyfilePath, password);

  // decrypt the private key from the .bckey file
  // this is the same for all files of this account
  std::string const decryptedPrivateKey =
      util::decrypt_private_key(accountInfo);

  // =============================================
  // RSA decryption of file information (header)
  // =============================================
  auto const decryptedFileBytes =
      util::decrypt_file(encryptedFilePath, decryptedPrivateKey);

  // ... and write it to disk
  util::write_file(outputFilePath, decryptedFileBytes);

  std::cout << "...Successfully decrypted file";
}
static void perform_decryption(commandline::DirectoryDecriptOptions &&) {}
