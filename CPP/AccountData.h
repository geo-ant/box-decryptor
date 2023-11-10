#ifndef ACCOUNTINFORMATION_H
#define ACCOUNTINFORMATION_H

#include <filesystem>
#include <string>

class AccountData {
public:
  AccountData() = default;

  /// extract the account data from a bckey file and the account password
  /// @throws exception on error
  AccountData(std::filesystem::path const &keyfilePath,
              std::string const &password);
  AccountData(const AccountData &) = delete;
  AccountData &operator=(const AccountData &) = delete;

  std::string GetPassword() const;
  std::string GetPBKDF2Salt() const;
  unsigned int GetPBKDF2Iterations() const;
  std::string GetEncryptedPrivateKey() const;

private:
  bool ParseBCKeyFile(const std::filesystem::path &keyfilePath);
  void SetPassword(const std::string &pw);

  std::string m_bckeyFilepath;
  std::string m_password;
  std::string m_encryptedPrivateKey;
  std::string m_pbkdf2Salt;
  unsigned int m_pbkdf2Iterations;
};

#endif
