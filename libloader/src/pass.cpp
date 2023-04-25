#include <iostream>
#include <fstream>
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <memory>
#include <sstream>
    
void generate_key_pair(const std::string &private_key_file, const std::string &public_key_file)
{
    struct bignumDeleter
    {
        void
        operator()(BIGNUM* instance)
        {
            BN_free(instance);
        }
    };
    using bignumUniquePtr=std::unique_ptr<BIGNUM,bignumDeleter>;


    int bits = 4096; // Увеличим длину ключа для повышения безопасности
    // BIGNUM *e = BN_new();
    auto exp = bignumUniquePtr(BN_new());
    if (!BN_set_word(exp.get(), RSA_F4))
    {
        std::cerr << "Error: failed to set RSA exponent." << std::endl;
        return;
    }
    RSA *rsa = RSA_new();
    if (!RSA_generate_key_ex(rsa, bits, exp.get(), nullptr))
    {
        std::cerr << "Error: failed to generate RSA key pair." << std::endl;
        return;
    }
    // BN_free(e);
    FILE *private_key_file_ptr = fopen(private_key_file.c_str(), "wb");
    PEM_write_RSAPrivateKey(private_key_file_ptr, rsa, nullptr, nullptr, 0, nullptr, nullptr);
    fclose(private_key_file_ptr);
    FILE *public_key_file_ptr = fopen(public_key_file.c_str(), "wb");
    PEM_write_RSAPublicKey(public_key_file_ptr, rsa);
    fclose(public_key_file_ptr);
    RSA_free(rsa);
}



void encrypt_password_file(const std::string &password_file_name, const std::string &public_key_file)
{
    std::string encrypted_file_name = password_file_name + ".enc";
    std::ifstream password_file(password_file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = password_file.tellg();
    password_file.seekg(0, std::ios::beg);
    if (!password_file)
    {
        std::cerr << "Error: failed to open the password file." << std::endl;
        return;
    }
    unsigned char *input = new unsigned char[size];
    password_file.read((char *)input, size);
    password_file.close();
    FILE *public_key_file_ptr = fopen(public_key_file.c_str(), "rb");
    RSA *rsa = RSA_new(); 
    PEM_read_RSAPublicKey(public_key_file_ptr, &rsa, nullptr, nullptr);
    fclose(public_key_file_ptr);
    unsigned char *output = new unsigned char[RSA_size(rsa)];
    int len = RSA_public_encrypt(size, input, output, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    if (len == -1)
    {
        std::cerr << "Error: failed to encrypt the password file." << std::endl;
        delete[] input;
        delete[] output;
        return;
    }
    std::ofstream encrypted_file(encrypted_file_name, std::ios::binary);
    encrypted_file.write((char *)output, len);
    encrypted_file.close();
    delete[] input;
    delete[] output;
    std::remove(password_file_name.c_str());
    return;
}

std::stringstream decrypt_password_file_to_mem(const std::string &encrypted_file_name, const std::string &private_key_file)
{
    std::stringstream ss("");
    std::string password_file_name = encrypted_file_name.substr(0, encrypted_file_name.rfind(".enc"));
    std::ifstream encrypted_file(encrypted_file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = encrypted_file.tellg();
    encrypted_file.seekg(0, std::ios::beg);
    if (!encrypted_file)
    {
        std::cerr << "Error: failed to open the encrypted file." << std::endl;
        return ss;
    }
    unsigned char *input = new unsigned char[size];
    RSA *rsa = RSA_new(); 
    FILE *private_key_file_ptr = fopen(private_key_file.c_str(), "rb");
    PEM_read_RSAPrivateKey(private_key_file_ptr, &rsa, nullptr, nullptr);
    fclose(private_key_file_ptr);
    unsigned char *output = new unsigned char[RSA_size(rsa)];
    encrypted_file.read((char *)input, size);
    encrypted_file.close();
    int len = RSA_private_decrypt(size, input, output, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    delete[] input;
    if (len == -1)
    {
        std::cerr << "Error: failed to decrypt the password file." << std::endl;
        delete[] output;
        return ss;
    }
    // std::ofstream password_file(password_file_name, std::ios::binary);
    ss.write((char *)output, len);
    // password_file.close();
    delete[] output;
    // std::remove(encrypted_file_name.c_str());
    return ss;
}


void decrypt_password_file(const std::string &encrypted_file_name, const std::string &private_key_file)
{
    std::string password_file_name = encrypted_file_name.substr(0, encrypted_file_name.rfind(".enc"));
    std::ifstream encrypted_file(encrypted_file_name, std::ios::binary | std::ios::ate);
    std::streamsize size = encrypted_file.tellg();
    encrypted_file.seekg(0, std::ios::beg);
    if (!encrypted_file)
    {
        std::cerr << "Error: failed to open the encrypted file." << std::endl;
        return;
    }
    unsigned char *input = new unsigned char[size];
    RSA *rsa = RSA_new(); 
    FILE *private_key_file_ptr = fopen(private_key_file.c_str(), "rb");
    PEM_read_RSAPrivateKey(private_key_file_ptr, &rsa, nullptr, nullptr);
    fclose(private_key_file_ptr);
    unsigned char *output = new unsigned char[RSA_size(rsa)];
    encrypted_file.read((char *)input, size);
    encrypted_file.close();
    int len = RSA_private_decrypt(size, input, output, rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    delete[] input;
    if (len == -1)
    {
        std::cerr << "Error: failed to decrypt the password file." << std::endl;
        delete[] output;
        return;
    }
    std::ofstream password_file(password_file_name, std::ios::binary);
    password_file.write((char *)output, len);
    password_file.close();
    delete[] output;
    std::remove(encrypted_file_name.c_str());
}


int main(int argc, char* argv[])
{
    // Сначала нужно сгенерировать пару ключей
    generate_key_pair("private_key.pem", "public_key.pem");
    // Затем можно зашифровать файл с парольными данными с помощью открытого ключа
    encrypt_password_file(argv[1], "public_key.pem");
    unsigned char* data;
    auto ss=decrypt_password_file_to_mem("passwords.txt.enc", "private_key.pem");
    // std::cout<<(char*)data<<std::endl;
    // printf("%.*s\n",size,data);
    std::string line2;
    while (std::getline(ss, line2))
    {
        std::cout << line2 << std::endl;
    }
    // Теперь можно расшифровать файл с помощью закрытого ключа
    decrypt_password_file("passwords.txt.enc", "private_key.pem");
    // Здесь файл passwords.txt уже расшифрован
    std::ifstream file("passwords.txt");
    std::string line;
    while (std::getline(file, line))
    {
        std::cout << line << std::endl;
    }
    file.close();
    return 0;
}