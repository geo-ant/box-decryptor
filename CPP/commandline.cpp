#include "commandline.hpp"

using namespace commandline;

namespace cli11 = CLI;

static void add_user_account_options(cli11::App *app,
                                     UserAccountInfo &user_account_info) {
  app->add_option("-k,--keyfile", user_account_info.keyfile,
                  "The keyfile to use for decryption")
      ->required(true);
  app->add_option("-p,--password", user_account_info.password,
                  "The password to use for decryption")
      ->required(true);
}

Options commandline::parse_commandline(int argc,
                                       char const *const *const argv) {

  auto app = cli11::App{"Boxcrytor Decryptor"};
  app.require_subcommand(1);

  SingleFileDecryptOptions single_file_options;
  DirectoryDecriptOptions directory_options;

  auto single_file_subcommand =
      app.add_subcommand("file", "Decrypt a single file");
  single_file_subcommand
      ->add_option("-o,--output", single_file_options.output_file,
                   "an optional name for the decrypted file")
      ->required(false);
  single_file_subcommand
      ->add_option("-i,--input", single_file_options.encrypted_file,
                   "The encrypted file you want to decrypt")
      ->required(true);
  add_user_account_options(single_file_subcommand,
                           single_file_options.user_account_info);

  /* auto directory_subcommand = app.add_subcommand("dir", "Decrypt a
   * directory"); */

  try {
    app.parse(argc, argv);
  } catch (const cli11::ParseError &e) {
    throw 1;
  }

  throw 1;
}
