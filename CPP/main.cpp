#include "AESHelper.h"
#include "AccountData.h"
#include "Base64Helper.h"
#include "FileData.h"
#include "HashHelper.h"
#include "PBKDF2Helper.h"
#include "RSAHelper.h"
#include "util.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  auto exe_name = std::string(argv[0]);

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
  try {
    std::filesystem::path keyfilePath(argv[1]);
    std::filesystem::path encryptedFilePath(argv[2]);
    std::string password(argv[3]);
    // TODO make optional
    std::filesystem::path outputFilePath =
        argc > 4 ? std::filesystem::path(argv[4]) : "";

    std::cout << "Decryption process started" << std::endl;

    // ============================================
    // AES decryption of private key in .bckey file
    // =============================================

    std::cout << "Password: " << password << std::endl;

    // collect information about the user account
    AccountData const accountInfo(keyfilePath, password);

    // decrypt the private key from the .bckey file
    std::string const decryptedPrivateKey =
        util::decrypt_private_key(accountInfo);

    // =============================================
    // RSA decryption of file information (header)
    // =============================================

    // collect information about the file to be decrypted
    FileData const fileData(encryptedFilePath, outputFilePath);

    // decrypt the file key (from file header) used for decryption of file data
    auto const decryptedFileKey = RSAHelper::DecryptFileKey(
        fileData.GetEncryptedFileKey(), decryptedPrivateKey);

    auto const fileCryptoKey = std::vector<byte>(decryptedFileKey.begin() + 32,
                                                 decryptedFileKey.begin() + 64);

    // =============================================
    // AES decryption of encrypted file
    // =============================================

    // decrypt the file data ...
    std::vector<byte> const decryptedFileBytes = AESHelper::DecryptFile(
        fileData.GetEncryptedFilePath(), fileCryptoKey, fileData.GetBaseIVec(),
        fileData.GetBlockSize(), fileData.GetHeaderLen(),
        fileData.GetCipherPadding());

    std::ofstream ofs(fileData.GetOutputFilepath(), std::ios::binary);
    if (!ofs.good()) {
      std::string errorMsg(
          "Can't create decrypted file at location '" +
          fileData.GetOutputFilepath().string() +
          "' (make sure you have the necessary file system rights to write to "
          "this location or specify another path)");
      throw std::runtime_error(errorMsg);
    }

    // ... and write it to disk
    ofs.write((char *)decryptedFileBytes.data(), decryptedFileBytes.size());

    std::cout << "Successfully decrypted file '"
              << fileData.GetEncryptedFilePath() << "', output: '"
              << fileData.GetOutputFilepath() << "'" << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
