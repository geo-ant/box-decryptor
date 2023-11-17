#ifndef RSADECRYPTOR_H
#define RSADECRYPTOR_H

#include "TypeDefs.h"
#include <string>
#include <vector>

namespace RSAHelper {
std::vector<byte> DecryptFileKey(const std::string &encryptedFileKey,
                                 const std::string &decryptedPrivateKey);
}; // namespace RSAHelper

#endif
