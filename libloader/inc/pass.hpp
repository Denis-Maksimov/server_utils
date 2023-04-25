#ifndef PASS
#define PASS
#include <string>
#include <sstream>

void generate_key_pair(const std::string &private_key_file, const std::string &public_key_file);
void encrypt_password_file(const std::string &password_file_name, const std::string &public_key_file);
void decrypt_password_file(const std::string &encrypted_file_name, const std::string &private_key_file);
std::stringstream decrypt_password_file_to_mem(const std::string &encrypted_file_name, const std::string &private_key_file);





#endif /* PASS */
