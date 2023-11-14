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

int main(int argc, char *argv[]) {
  try {

    auto exe_name = std::string(argv[0]);
    auto options = commandline::parse_commandline(argc, argv);

    if (argc < 4) {
      std::cout << "Usage: " << exe_name << " "
                << "[path to .bckey file] "                      // argv[1]
                << "[path to encrypted file] "                   // argv[2]
                << "[pwd] "                                      // argv[3]
                << "[path for output (optional)] " << std::endl; // argv[4]
      return 0;
    }

    // for the sake of keeping this program short just catch
    // all exceptions in one place and show the error before exiting
    std::filesystem::path const keyfilePath(argv[1]);
    std::filesystem::path const encryptedFilePath(argv[2]);
    std::string const password(argv[3]);
    std::filesystem::path const outputFilePath =
        argc > 4 ? std::filesystem::path(argv[4])
                 : std::filesystem::path(encryptedFilePath).replace_extension();

    std::cout << "Decryption process started" << std::endl;

    // ============================================
    // AES decryption of private key in .bckey file
    // =============================================

    std::cout << "Password: " << password << std::endl;

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

    std::cout << "Successfully decrypted file '";
  } catch (commandline::Error const &e) {
    return e.exit();
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
