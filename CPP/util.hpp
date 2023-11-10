/**
 * @file util.hpp
 * @brief Utility functions for BoxDecryptor
 * @author Georgios Antononpoulos
 */
#ifndef BOXDECRYPTOR_UTIL_HPP_INCLUDED
#define BOXDECRYPTOR_UTIL_HPP_INCLUDED

#include "AccountData.h"
#include <string>

namespace util {
/// decrypt a private key from the account information. The account data
/// must hold valid information abtained with the keyfile and password
/// @return decrypted private key
/// @throws exception on error
std::string decrypt_private_key(AccountData const &account_data);

} // namespace util
#endif
