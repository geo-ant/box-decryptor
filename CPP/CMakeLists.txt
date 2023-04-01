cmake_minimum_required(VERSION 3.16...3.23)

# Not ideal to use this global variable, but necessary to make sure
# that tooling and projects use the same version
set(CMAKE_CXX_STANDARD 17)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
# set(CMAKE_CXX_EXTENSIONS OFF)

# Set the project name and language
project(
  boxdecryptor
  VERSION 0.0.1
  DESCRIPTION "single file boxcryptor decryptor"
  LANGUAGES CXX C)
  
add_executable(bc-decrypt AccountData.cpp
                          AESHelper.cpp
                          Base64Helper.cpp
                          FileData.cpp
                          HashHelper.cpp
                          PBKDF2Helper.cpp
                          RSAHelper.cpp
                          main.cpp)

target_include_directories(bc-decrypt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/
  cryptopp/include)


target_link_directories(bc-decrypt PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../libcryptopp700)
# statically  link cryptopp
target_link_libraries(bc-decrypt PRIVATE cryptopp)
