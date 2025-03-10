#include "FileData.h"
#include "TypeDefs.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

// this method should ideally be implemented using a proper JSON library,
// but for the purpose of demonstrating which infos are needed from
// the file header simple string searches should be sufficient
FileData::FileData(const std::filesystem::path &encryptedFilePath) {
  if (encryptedFilePath.extension() != ".bc") {
    throw std::runtime_error(
        "Given filepath does not have the right extension ('.bc'), please "
        "specify a Boxcryptor encrypted file");
  }

  std::ifstream headerFile(encryptedFilePath, std::ios::binary);
  if (!headerFile.good()) {
    std::string errorMsg(
        "Encrypted file (" + encryptedFilePath.string() +
        ") can't be opened (make sure the provided path is correct, the file "
        "exists and you have the right to open the file)");
    throw std::runtime_error(errorMsg);
  }

  this->m_encryptedFilePath = encryptedFilePath;

  // read the first 16 bytes which contain the file version
  // and information about the length of the different file parts
  std::vector<char> rawHeaderBytes(16);
  headerFile.read(&rawHeaderBytes[0], 16);

  auto fileVersionBytes =
      std::vector<byte>(rawHeaderBytes.begin(), rawHeaderBytes.begin() + 4);
  if (fileVersionBytes != this->m_supportedFileVersion) {
    throw std::runtime_error(
        "Unknown file version found in header, aborting...");
  }

  auto headerCoreLenBytes =
      std::vector<byte>(rawHeaderBytes.begin() + 4, rawHeaderBytes.begin() + 8);
  auto headerPaddingLenBytes = std::vector<byte>(rawHeaderBytes.begin() + 8,
                                                 rawHeaderBytes.begin() + 12);
  auto cipherPaddingLenBytes = std::vector<byte>(rawHeaderBytes.begin() + 12,
                                                 rawHeaderBytes.begin() + 16);

  unsigned int headerRawLen = 48; // always 48 bytes
  unsigned int headerCoreLen =
      headerCoreLenBytes.at(3) << 24 | headerCoreLenBytes.at(2) << 16 |
      headerCoreLenBytes.at(1) << 8 | headerCoreLenBytes.at(0);
  unsigned int headerPaddingLen =
      headerPaddingLenBytes.at(3) << 24 | headerPaddingLenBytes.at(2) << 16 |
      headerPaddingLenBytes.at(1) << 8 | headerPaddingLenBytes.at(0);
  unsigned int cipherPaddingLen =
      cipherPaddingLenBytes.at(3) << 24 | cipherPaddingLenBytes.at(2) << 16 |
      cipherPaddingLenBytes.at(1) << 8 | cipherPaddingLenBytes.at(0);

  this->m_headerData.rawLen = headerRawLen;
  this->m_headerData.coreLen = headerCoreLen;
  this->m_headerData.corePaddingLen = headerPaddingLen;
  this->m_headerData.cipherPaddingLen = cipherPaddingLen;

  // go to the end of the raw header and read the core header
  headerFile.seekg(this->m_headerData.rawLen);
  std::vector<char> coreHeaderBytes(headerCoreLen);
  headerFile.read(&coreHeaderBytes[0], this->m_headerData.coreLen);

  std::string coreHeader(coreHeaderBytes.begin(), coreHeaderBytes.end());

  // find the blocksize
  std::string searchString = R"("blockSize")";
  size_t posBegin = coreHeader.find(searchString);
  posBegin = coreHeader.find(":", posBegin + searchString.length());
  size_t posEnd = coreHeader.find(",", posBegin + 1);

  if (posBegin != std::string::npos && posEnd != std::string::npos &&
      posEnd - posBegin > 0) {
    std::string blockSize =
        coreHeader.substr(posBegin + 1, posEnd - posBegin - 1);
    try {
      this->m_blockSize = std::stoi(blockSize);
    } catch (...) {
      throw std::runtime_error("Could not convert block size to integer");
    }
  } else {
    throw std::runtime_error("Could not find block size in file header");
  }

  // find the initialization vector
  searchString = R"("iv")";
  posBegin = coreHeader.find(searchString);
  posBegin = coreHeader.find(R"(")", posBegin + searchString.length());
  posEnd = coreHeader.find(R"(")", posBegin + 1);

  if (posBegin != std::string::npos && posEnd != std::string::npos &&
      posEnd - posBegin > 0) {
    this->m_baseIVec = coreHeader.substr(posBegin + 1, posEnd - posBegin - 1);
  } else {
    throw std::runtime_error(
        "Could not find initialization vector in file header");
  }

  // find the (first) encrypted file key object
  searchString = R"("encryptedFileKeys")";
  posBegin = coreHeader.find(searchString);
  posBegin = coreHeader.find("{", posBegin + searchString.length());
  posEnd = coreHeader.find("}", posBegin + 1);

  // and the 'value' within
  searchString = R"("value")";
  posBegin = coreHeader.find(searchString, posBegin + 1);
  posBegin = coreHeader.find(R"(")", posBegin + searchString.length());

  // check if we are still in the right block / JSON object
  if (posBegin > posEnd) {
    throw std::runtime_error(
        "The (first) file key object has no suitable key value");
  }
  posEnd = coreHeader.find(R"(")", posBegin + 1);

  if (posBegin != std::string::npos && posEnd != std::string::npos &&
      posEnd - posBegin > 0) {
    this->m_encryptedFileKey =
        coreHeader.substr(posBegin + 1, posEnd - posBegin - 1);
  } else {
    throw std::runtime_error("Could not find file key in file header");
  }
}

std::string FileData::GetEncryptedFileKey() const {
  return this->m_encryptedFileKey;
}

std::filesystem::path const &FileData::GetEncryptedFilePath() const {
  return this->m_encryptedFilePath;
}

std::string FileData::GetBaseIVec() const { return this->m_baseIVec; }

unsigned int FileData::GetBlockSize() const { return this->m_blockSize; }

unsigned int FileData::GetHeaderLen() const {
  return this->m_headerData.rawLen + this->m_headerData.coreLen +
         this->m_headerData.corePaddingLen;
}

unsigned int FileData::GetCipherPadding() const {
  return this->m_headerData.cipherPaddingLen;
}
