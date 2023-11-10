#include "util.hpp"
#include "AESHelper.h"

namespace util {
std::string decrypt_private_key(AccountData const &account_data) {
  std::string decryptedPrivateKey;
  AESHelper::DecryptDataPBKDF2(
      account_data.GetEncryptedPrivateKey(), account_data.GetPassword(),
      account_data.GetPBKDF2Salt(), account_data.GetPBKDF2Iterations(),
      decryptedPrivateKey);
  return decryptedPrivateKey;
}
} // namespace util
