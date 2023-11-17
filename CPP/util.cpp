#include "util.hpp"
#include "AESHelper.h"
#include "FileData.h"
#include "RSAHelper.h"
#include <cstdio>

namespace util {
std::string decrypt_private_key(AccountData const &account_data) {
  std::string decryptedPrivateKey;
  AESHelper::DecryptDataPBKDF2(
      account_data.GetEncryptedPrivateKey(), account_data.GetPassword(),
      account_data.GetPBKDF2Salt(), account_data.GetPBKDF2Iterations(),
      decryptedPrivateKey);
  return decryptedPrivateKey;
}
std::string decrypt_private_key(std::filesystem::path const &keyfile_path,
                                std::string const &password) {
  return decrypt_private_key(AccountData(keyfile_path, password));
}

std::vector<byte> decrypt_file(std::filesystem::path const &encryptedFilePath,
                               std::string decryptedPrivateKey) {
  // collect information about the file to be decrypted
  FileData const fileData(encryptedFilePath);

  // decrypt the file key (from file header) used for decryption of file data
  auto const decryptedFileKey = RSAHelper::DecryptFileKey(
      fileData.GetEncryptedFileKey(), decryptedPrivateKey);

  auto const fileCryptoKey = std::vector<byte>(decryptedFileKey.begin() + 32,
                                               decryptedFileKey.begin() + 64);

  // decrypt the file data ...
  return AESHelper::DecryptFile(fileData.GetEncryptedFilePath(), fileCryptoKey,
                                fileData.GetBaseIVec(), fileData.GetBlockSize(),
                                fileData.GetHeaderLen(),
                                fileData.GetCipherPadding());
}

void write_file(std::filesystem::path const &outputFilePath,
                std::vector<byte> const &decryptedFileBytes) {
  auto file = std::fopen(outputFilePath.c_str(), "wb");
  auto defer_fclose = util::defer([file]() { std::fclose(file); });

  auto const count = std::fwrite(decryptedFileBytes.data(), sizeof(byte),
                                 decryptedFileBytes.size(), file);
  if (count != decryptedFileBytes.size()) {
    throw std::runtime_error("Failed to write decrypted file to disk");
  }
}

} // namespace util
