#ifndef UTILS_H
#define UTILS_H

#include "TypeDefs.h"
#include <vector>

class HashHelper {
public:
  HashHelper() = delete;

  static bool ComputeSHA256HMAC(const std::vector<byte> &data,
                                const std::vector<byte> &key,
                                std::vector<byte> &hmac, bool silent = true);
  static bool ComputeSHA512HMAC(const std::vector<byte> &data,
                                const std::vector<byte> &key,
                                std::vector<byte> &hmac, bool silent = true);
};

#endif
