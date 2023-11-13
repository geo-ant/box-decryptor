cmake_minimum_required(VERSION 3.24)

# Not ideal to use this global variable, but necessary to make sure that tooling
# and projects use the same version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# information about processor count
include(ProcessorCount)
ProcessorCount(NCORES)
if(NOT NCORES EQUAL 0)
  set(CMAKE_BUILD_PARALLEL_LEVEL ${NCORES})
endif()

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example when
# compiling with PCH enabled set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and language
project(
  boxdecryptor
  VERSION 0.0.1
  DESCRIPTION "single file boxcryptor decryptor"
  LANGUAGES CXX C)

add_executable(
  bc-decrypt
  AccountData.cpp
  AESHelper.cpp
  Base64Helper.cpp
  FileData.cpp
  HashHelper.cpp
  PBKDF2Helper.cpp
  util.cpp
  RSAHelper.cpp
  main.cpp)

target_include_directories(bc-decrypt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/
                                              cryptopp/include)

# now we need to build and link the cryptopp library version 7.0.0 I have the
# complete source code for this in the libcryptopp700 directory in the root
# folder of this repo To build the library we just execute the make command in
# the libcryptopp700 directory we can do this from cmake by adding a custom
# command and then making this a dependency for our executable
add_custom_command(
  TARGET bc-decrypt
  PRE_BUILD
  COMMAND make -j ${CMAKE_BUILD_PARALLEL_LEVEL}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../libcryptopp700
  VERBATIM)

# link the cryptopp library for this to work, you need to have the cryptopp
# library built do this by cd'ing into the libcryptopp700 directory and running
# make
target_link_directories(bc-decrypt PRIVATE
                        ${CMAKE_CURRENT_SOURCE_DIR}/../libcryptopp700)
target_link_libraries(bc-decrypt PRIVATE cryptopp)

if(MSVC)
  target_compile_options(bc-decrypt PRIVATE /W4 /WX)
else()
  target_compile_options(bc-decrypt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

enable_testing()

# copy the contents of the test folder next to the executable
add_custom_command(
  TARGET bc-decrypt
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test
          ${CMAKE_CURRENT_BINARY_DIR}
  VERBATIM)

# execute the script test-decrypt.sh after the build with the arguments
# helloworld.txt.bc and helloworld.txt.good this will decrypt the file and
# compare it to the good file use the cmake testing framework  to do this
add_test(NAME test-decrypt COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test-decrypt.sh
                                   helloworld.txt.bc helloworld.txt.good)
