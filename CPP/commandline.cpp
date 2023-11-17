#include "commandline.hpp"
#include "CLI11/CLI11.hpp"
#include <memory>

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

  auto app = cli11::App("Boxcryptor Decryptor");
  try {
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
        /* ->add_option("-i,--input", single_file_options.encrypted_file, */
        ->add_option("filename", single_file_options.encrypted_file,
                     "The encrypted file you want to decrypt")
        ->required(true);
    add_user_account_options(single_file_subcommand,
                             single_file_options.user_account_info);

    auto directory_subcommand =
        app.add_subcommand("dir", "Decrypt a directory");

    add_user_account_options(directory_subcommand,
                             directory_options.user_account_info);

    directory_subcommand
        ->add_flag("-r,--recursive", directory_options.recursive,
                   "Recursively decrypt all files in the "
                   "directory and its subdirectories")
        ->capture_default_str();

    directory_subcommand
        ->add_option("directory", directory_options.encrypted_directory,
                     "The encrypted directory you want to decrypt")
        ->required(true);

    app.parse(argc, argv);

    if (app.got_subcommand(single_file_subcommand)) {
      return single_file_options;
    } else if (app.got_subcommand(directory_subcommand)) {
      return directory_options;
    } else {
      // indicates a bug in the program
      throw std::runtime_error("Unknown subcommand");
    }
  } catch (const cli11::ParseError &e) {
    throw commandline::Error(app.exit(e));
  }
}
