#ifndef ENCRYPTTOOL_H
#define ENCRYPTTOOL_H

#include <openssl/evp.h>
#include <QObject>

#define RSA_KEYLEN 2048
#define AES_ROUNDS 6

#define SUCCESS 0
#define FAILURE -1

#define KEY_SERVER_PRI 0
#define KEY_SERVER_PUB 1
#define KEY_CLIENT_PUB 2
#define KEY_CLIENT_PRI 3
#define KEY_AES        4
#define KEY_AES_IV     5

class EncryptTool : public QObject
{
    Q_OBJECT

public:
    explicit EncryptTool(QObject* parent = nullptr);
    ~EncryptTool();

    int rsaEncrypt(const unsigned char* message, size_t messageLength, unsigned char** encryptedMessage,
                   unsigned char** encryptedKey, size_t* encryptedKeyLength, unsigned char** iv, size_t* ivLength);
    int rsaDecrypt(unsigned char* encryptedMessage, size_t encryptedMessageLength, unsigned char* encryptedKey,
                   size_t encryptedKeyLength, unsigned char* iv, size_t ivLength, unsigned char** decryptedMessage);

    int setRemotePublicKey(unsigned char* publicKey, size_t publicKeyLength);
    int setRemotePrivateKey(unsigned char* privateKey, size_t privateKeyLength);
    int setLocalPublicKey(unsigned char* publicKey, size_t publicKeyLength);
    int setLocalPrivateKey(unsigned char* privateKey, size_t privateKeyLength);

    int writeKeyToFile(FILE* file, int key);
    int readFile(char* filename, unsigned char** file);
    int writeFile(char* filename, unsigned char* file, size_t fileLength);

    void generateRsa();
    bool signRsa(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen, EVP_PKEY* pkey);
    bool verifyRsa(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen, EVP_PKEY* pkey);
    bool signHash(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen);
    bool verifyHash(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen);
    bool computeHash(char* unhashed, size_t unhashedSiz, unsigned char** hashed, size_t* hashedSiz, int type);

private:
    static EVP_PKEY* m_localKeypair;
    static EVP_PKEY* m_remoteKeypair;

    EVP_CIPHER_CTX* m_rsaEncryptContext;
    EVP_CIPHER_CTX* m_aesEncryptContext;

    EVP_CIPHER_CTX* m_rsaDecryptContext;
    EVP_CIPHER_CTX* m_aesDecryptContext;

    unsigned char* m_aesKey;
    unsigned char* m_aesIv;

    size_t m_aesKeyLength;
    size_t m_aesIvLength;

    int generateRsaKeypair(EVP_PKEY** keypair);
    int generateAesKey(unsigned char** aesKey, unsigned char** aesIv);
    int bioToString(BIO* bio, unsigned char** string);
};

#endif // ENCRYPTTOOL_H
