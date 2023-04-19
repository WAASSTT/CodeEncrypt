#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>

class FileEncryptor {
public:
    bool EncryptFile(const std::string& inFile, const std::string& outFile, const std::string& password) {
        // 生成密钥
        GenerateKey(password);
        // 如果输入和输出文件名相同，为了避免覆盖输入文件，需要将输出文件名修改为临时文件名
        std::string tempOutFile = outFile;
        if (inFile == outFile) {
            tempOutFile += ".temp";
        }
        // 打开输入文件
        std::ifstream fin(inFile, std::ios::binary);
        if (!fin.is_open()) {
            std::cerr << "无法打开输入文件： " << inFile << std::endl;
            return false;
        }

        // 打开输出文件
        std::ofstream fout(tempOutFile, std::ios::binary);
        if (!fout.is_open()) {
            std::cerr << "无法打开输出文件： " << tempOutFile << std::endl;
            fin.close();
            return false;
        }

        // 生成随机 IV
        unsigned char iv[EVP_MAX_IV_LENGTH];
        if (RAND_bytes(iv, EVP_MAX_IV_LENGTH) != 1) {
            std::cerr << "无法生成随机 IV" << std::endl;
            fin.close();
            fout.close();
            return false;
        }

        // 将 IV 写入输出文件
        fout.write(reinterpret_cast<const char*>(iv), EVP_MAX_IV_LENGTH);

        // 初始化加密上下文
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (ctx == nullptr) {
            std::cerr << "无法创建加密上下文" << std::endl;
            fin.close();
            fout.close();
            return false;
        }

        // 设置加密算法和密码
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, this->key, iv) != 1) {
            std::cerr << "无法设置加密算法和密钥" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            fin.close();
            fout.close();
            return false;
        }

        // 逐块加密并写入输出文件
        std::vector<unsigned char> in_data(INPUT_BUFFER_SIZE);
        std::vector<unsigned char> out_data(OUTPUT_BUFFER_SIZE + EVP_MAX_BLOCK_LENGTH);

        // 获取 in_buffer 的底层数据指针
        unsigned char* in_buffer = in_data.data();

        // 获取 out_buffer 的底层数据指针
        unsigned char* out_buffer = out_data.data();
        int out_len;
        int total_len = 0;
        while (fin.good()) {
            fin.read(reinterpret_cast<char*>(in_buffer), INPUT_BUFFER_SIZE);
            int in_len = fin.gcount();
            total_len += in_len;

            if (EVP_EncryptUpdate(ctx, out_buffer, &out_len, in_buffer, in_len) != 1) {
                std::cerr << "无法加密输入数据" << std::endl;
                EVP_CIPHER_CTX_free(ctx);
                fin.close();
                fout.close();
                return false;
            }

            fout.write(reinterpret_cast<const char*>(out_buffer), out_len);
        }

        // 结束加密
        if (EVP_EncryptFinal_ex(ctx, out_buffer, &out_len) != 1) {
            std::cerr << "无法完成加密" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            fin.close();
            fout.close();
            return false;
        }

        fout.write(reinterpret_cast<const char*>(out_buffer), out_len);

        // 清理资源
        EVP_CIPHER_CTX_free(ctx);
        fin.close();
        fout.close();
        // 如果输入和输出文件名相同，需要将临时文件重命名为原文件名
        if (inFile == outFile) {
            std::remove(inFile.c_str());
            std::rename(tempOutFile.c_str(), outFile.c_str());
        }
        std::cout << "加密 " << total_len << " 字节来自 " << inFile << " 到 " << outFile << std::endl;
        return true;
    }

    bool DecryptFile(const std::string& inFile, const std::string& outFile, const std::string& password) {
        // 生成密钥
        GenerateKey(password);

        // 如果输入和输出文件名相同，为了避免覆盖输入文件，需要将输出文件名修改为临时文件名
        std::string tempOutFile = outFile;
        if (inFile == outFile) {
            tempOutFile += ".temp";
        }
        // 打开输入文件
        std::ifstream fin(inFile, std::ios::binary);
        if (!fin.is_open()) {
            std::cerr << "无法打开输入文件：" << inFile << std::endl;
            return false;
        }

        // 打开输出文件
        std::ofstream fout(tempOutFile, std::ios::binary);
        if (!fout.is_open()) {
            std::cerr << "无法打开输出文件：" << tempOutFile << std::endl;
            fin.close();
            return false;
        }

        // 读取 IV
        unsigned char iv[EVP_MAX_IV_LENGTH];
        fin.read(reinterpret_cast<char*>(iv), EVP_MAX_IV_LENGTH);

        // 初始化解密上下文
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (ctx == nullptr) {
            std::cerr << "无法创建解密上下文" << std::endl;
            fin.close();
            fout.close();
            return false;
        }

        // 设置解密算法和密码
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, this->key, iv) != 1) {
            std::cerr << "无法设置解密算法和密钥" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            fin.close();
            fout.close();
            return false;
        }

        // 逐块解密并写入输出文件
        std::vector<unsigned char> in_data(INPUT_BUFFER_SIZE + EVP_MAX_BLOCK_LENGTH);
        std::vector<unsigned char> out_data(OUTPUT_BUFFER_SIZE );
        // 获取 in_buffer 的底层数据指针
        unsigned char* in_buffer = in_data.data();

        // 获取 out_buffer 的底层数据指针
        unsigned char* out_buffer = out_data.data();
        int out_len;
        int total_len = 0;
        while (fin.good()) {
            fin.read(reinterpret_cast<char*>(in_buffer), INPUT_BUFFER_SIZE + EVP_MAX_BLOCK_LENGTH);
            int in_len = fin.gcount();
            total_len += in_len;

            if (EVP_DecryptUpdate(ctx, out_buffer, &out_len, in_buffer, in_len) != 1) {
                std::cerr << "无法解密输入数据" << std::endl;
                EVP_CIPHER_CTX_free(ctx);
                fin.close();
                fout.close();
                return false;
            }

            fout.write(reinterpret_cast<const char*>(out_buffer), out_len);
        }

        // 结束解密
        if (EVP_DecryptFinal_ex(ctx, out_buffer, &out_len) != 1) {
            std::cerr << "无法完成解密" << std::endl;
            EVP_CIPHER_CTX_free(ctx);
            fin.close();
            fout.close();
            return false;
        }

        fout.write(reinterpret_cast<const char*>(out_buffer), out_len);

        // 清理资源
        EVP_CIPHER_CTX_free(ctx);
        fin.close();
        fout.close();

        // 如果输入和输出文件名相同，需要将临时文件重命名为原文件名
        if (inFile == outFile) {
            std::remove(inFile.c_str());
            std::rename(tempOutFile.c_str(), outFile.c_str());
        }
        std::cout << "解密 " << total_len << " 字节来自 " << inFile << " 到 " << outFile << std::endl;
        return true;
    }

private:
    unsigned char key[EVP_MAX_KEY_LENGTH];
    const int INPUT_BUFFER_SIZE = 40960;
    const int OUTPUT_BUFFER_SIZE = 40960;

    void GenerateKey(const std::string& password) {
        EVP_BytesToKey(
            EVP_aes_256_cbc(),
            EVP_sha256(),
            NULL,
            reinterpret_cast<const unsigned char*>(password.c_str()),
            password.size(),
            1,
            this->key,
            NULL
        );
    }
};
