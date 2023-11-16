# Box Cryptor File Decryptor
This is a fork of the original boxcryptor single file descriptor repo with 
some major usability improvements and a bit of future proofing. It may be 
helpful to some that cannot get the original boxcryptor executables to work 
but I take absolutely no responsibility if it utterly destroys your files, see 
the section on warning and licensing.

## Context
[Boxcryptor](https://www.boxcryptor.com/en/) was bought by dropbox and it seems
their boxcryptor service is slowly dying. Unfortunately the boxcryptor portable apps
that I tried (for Linux and Mac) did not help me as much to decrypt my files
because they would constantly give errors when downloading files from my dropbox.
That gave me a burst of panic because as of now I have tons of boxcryptor encrypted
files (`*.bc`) which might become useless in future. That's a problem.

There's an [off-migration guide](https://support.boxcryptor.com/en/help/faq-and-troubleshooting/portable/)
but it works pretty badly with the portable version of boxcryptor which I need 
because I am on linux. I'll try the other versions again but none of that is 
inspiring a lot of confidence. What if I am stuck with a dropbox full of encrypted files
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

To build the executable go into your shell and execute the following commands

```shell
$ cd CPP
$ mkdir build && cd build
$ cmake ..
$ cmake --build .
```

This requires CMake, make, and C++ compiler that speaks at least C++17, which many
compilers do unless they are pretty out of date. The final linking step
takes a long time when building for the first time, because the crypto++ library
is built in the background. Everything should finish without problems and you
now have the `bc-decrypt` executable in your `build` directory that you can use as 
described in the next section.

## Usage

The executable is self documenting which is why I won't go into much detail here 
because that might go out of sync with the actual commands. Just in general:
this is a command line app that is executed from the terminal.

If you give it the argument `--help` or `-h` it will print a help text. For example

```shell 
$ ./bc-decrypt --help 
```

will print a help text that tells you that a subcommand is required. The subcommands 
(at this time) are either `file` or `dir` which control whether the decryption
is performed for a single file or for a complete directory. If you again pass 
`--help` to the subcommands, like so:

```shell
$ ./bc-decrypt file --help 
```

If will give you the command line options it required. We need to pass the 
boxcryptor password, the keyfile and the file we want to decrypt. Optionally
we can also specify how the output file is called. If we don't specify that, it 
will be called like the encrypted file with the `.bc` stripped.

# License and Warning

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the “Software”), to deal in 
the Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
