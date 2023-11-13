#ifndef FILEINFORMATION_H
#define FILEINFORMATION_H

#include "TypeDefs.h"
#include <filesystem>
#include <string>
#include <vector>

struct HeaderData {
  unsigned int rawLen = 48;
  unsigned int coreLen;
  unsigned int corePaddingLen;
  unsigned int cipherPaddingLen;
};

class FileData {
public:
  FileData() = default;
  FileData(const FileData &) = delete;
  FileData &operator=(const FileData &) = delete;
  /// collect some information about the file being decrypted
  /// throws an exception on error
  FileData(std::filesystem::path const &encryptedFilePath,
           std::string const &outputFilePath);

  std::filesystem::path const &GetOutputFilepath() const;
  std::string GetEncryptedFileKey() const;
  std::filesystem::path const &GetEncryptedFilePath() const;
  std::string GetBaseIVec() const;
  unsigned int GetBlockSize() const;
  unsigned int GetHeaderLen() const;
  unsigned int GetCipherPadding() const;

private:
  std::string m_encryptedFileKey;
  std::string m_baseIVec;
  std::filesystem::path m_encryptedFilePath;
  unsigned int m_blockSize;
  HeaderData m_headerData;
  std::filesystem::path m_outputFilePath;

  // The following byte sequence corresponds to bc01;
  // Note: There is another file version for bc02 now.
  const std::vector<byte> m_supportedFileVersion = {98, 99, 48, 49};
};

#endif
