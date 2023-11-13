/**
 * @file util.hpp
 * @brief Utility functions for BoxDecryptor
 * @author Georgios Antononpoulos
 */
#ifndef BOXDECRYPTOR_UTIL_HPP_INCLUDED
#define BOXDECRYPTOR_UTIL_HPP_INCLUDED

#include "AccountData.h"
#include "TypeDefs.h"
#include <filesystem>
#include <string>
#include <type_traits>
#include <vector>

namespace util {
/// decrypt a private key from the account information. The account data
/// must hold valid information abtained with the keyfile and password
/// @return decrypted private key
/// @throws exception on error
std::string decrypt_private_key(AccountData const &account_data);

/// decrypt the file and return the encrypted data as a vector of bytes
/// TODO: make this function run on the data read from the file into memory
/// then we can make it parallel
std::vector<byte> decrypt_file(std::filesystem::path const &encrypted_file_path,
                               std::string decrypted_private_key);

/// write the decrypted file to the output file path
void write_file(std::filesystem::path const &output_file_path,
                std::vector<byte> const &decrypted_file_data);

/// helper class for deferring function calls
template <typename F> class [[nodiscard]] DeferHelper {
public:
  static_assert(std::is_nothrow_destructible_v<F>,
                "deferred function must be nothrow destructible");
  static_assert(std::is_same_v<void, std::invoke_result_t<F>>,
                "deferred function must return void");

  ~DeferHelper() {
    try {
      function();
    } catch (...) {
    }
  }

private:
  F function;
  DeferHelper(F function) : function(std::move(function)) {}
  template <typename T> friend DeferHelper<T> defer(T);
};

/// helper function for deferring function calls
template <typename F> DeferHelper<F> defer(F function) {
  return DeferHelper<F>(std::move(function));
}

} // namespace util
#endif
