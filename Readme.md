# Box Cryptor File Decryptor
This is a fork of the original boxcryptor single file descriptor repo with 
some major usability improvements and a bit of future proofing. 

## Context
[Boxcryptor](https://www.boxcryptor.com/en/) was bought by dropbox and it seems
their boxcryptor service is slowly dying. Unfortunately the boxcryptor portable apps
that I tried (for Linux and Mac) did not help me as much to decrypt my files
because they would constantly give errors when downloading files from my dropbox.
That gave me a burst of panic because as of now I have tons of boxcryptr encrypted
files (`*.bc`) which might become useless in future. That's a problem.

There's an [off-migration guide](https://support.boxcryptor.com/en/help/faq-and-troubleshooting/portable/)
but it works pretty badly with the portable version of boxcryptor which I need 
because I am on linux. I'll try the other versions again but none of that is 
confidence inspiring. What if I am stuck with a dropbox full of encrypted files
and boxcryptor is out of business?

## Solution
Luckily someone at that company foresaw that problem and open sourced a repository
which produces an executable that can decrypt single files. For that to work
we need:

* our `.bckey` file, which we can obtain by 
[exporting our keys](https://support.boxcryptor.com/en/help/boxcryptor-account/portable/#export-your-keys/)
* our password, which is the regular password used for logging in to
boxcryptor

Once we have those things, we can build this executable (I'll describe how
to do that below) and then use the executable as described below to point
it to a single file to decrypt it.

## Building the Executable
The original repo contained source code in C++, Java, and C#. Since this
is a fork of this repo, we can still recover that from the git history if we wanted,
but we should not have to. For this branch I have trimmed it down to the C++
version. The source code for the executable is in the `CPP` directory.

I have also bundled the source code for the [crypto++](https://www.cryptopp.com/#download)
library version 7.0.0 in the `libcryptopp700` directory. It can be built by 
running `make` inside that directory, but it's not necessary to do that
by hand, because I have set up the a cmake build file that takes care of this.

To build the executable

```shell
$ cd CPP
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```
This requires CMake, make, and C++ compiler that speaks at least C++14, which almost
all compilers do unless they are horribly out of date. The final linking step
takes a long time when building for the first time because the crypto++ library
is built in the background. Everything should finish without problems and you
now have the `bc-decrypt` executable that you can use as described below in
the original readme.

## Original Readme (incl Manual)
What follows are the contents of the original readme, which still apply.

### USE WITH CAUTION
**This project is intended as a documentation on how to decrypt files encrypted with Boxcryptor (2.x). Only use it with copies of your files and never use it in a production environment.
When using this software you agree that Secomba GmbH will not accept any responsibility for any potential loss of data.**

### Description
The purpose of this repository is to show / document how to decrypt Boxcryptor files (.bc) with your individual keyfile (.bckey) and your password as input. If you want to know more about the decryption process or include similiar functionality in your own applications you can have a look at the source code in your preferred language.
For information about the build process please consult the corresponding readme files in the language subfolders.
**Please also note that you should only give sensitive information like the password to your Boxcryptor account (especially in combination with your personal .bckey-file) to applications you trust, we therefore recommend you having a look at the source code to be absolutely sure how the password is used before doing so.**

### Usage
After having successfully build the application you can run it with the following command line arguments:
1. The path to your personal .bckey-file (you can get this file by following this guide: https://www.boxcryptor.com/en/help/boxcryptor-account/windows/#export-your-keys)
2. The path to the encrypted file you want to decrypt
3. The password to your Boxcryptor account
4. Optionally: The path where the decrypted file should be saved to (if not specified, the output will be derived from the encrypted file and will be located in the same folder)

If your input was correct, you should now have the decrypted file at the path you specified.
