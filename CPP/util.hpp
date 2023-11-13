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
std::vector<byte>
decrypt_file(std::filesystem::path const &encrypted_file_path);

} // namespace util
#endif
