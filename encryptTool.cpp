#include "encryptTool.h"

#include <openssl/pem.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <QDebug>

EVP_PKEY* EncryptTool::m_localKeypair;
EVP_PKEY* EncryptTool::m_remoteKeypair;

EncryptTool::EncryptTool(QObject* parent) : QObject(parent)
{
    m_localKeypair = nullptr;
    m_remoteKeypair = nullptr;

    /* Initalize contexts */
    m_rsaEncryptContext = EVP_CIPHER_CTX_new();
    m_aesEncryptContext = EVP_CIPHER_CTX_new();

    m_rsaDecryptContext = EVP_CIPHER_CTX_new();
    m_aesDecryptContext = EVP_CIPHER_CTX_new();

    /* Check if any of the contexts initializations failed */
    if(m_rsaEncryptContext == nullptr || m_aesEncryptContext == nullptr ||
       m_rsaDecryptContext == nullptr || m_aesDecryptContext == nullptr) {
        qDebug() << "Falied to create contexts";
    }

    /* Don't set key or IV right away; we want to set lengths */
    EVP_CIPHER_CTX_init(m_aesEncryptContext);
    EVP_CIPHER_CTX_init(m_aesDecryptContext);

    EVP_CipherInit_ex(m_aesEncryptContext, EVP_aes_256_cbc(), nullptr, nullptr, nullptr, 1);

    /* Now we can set key and IV lengths */
    m_aesKeyLength = static_cast<unsigned int>(EVP_CIPHER_CTX_key_length(m_aesEncryptContext));
    m_aesIvLength = static_cast<unsigned int>(EVP_CIPHER_CTX_iv_length(m_aesEncryptContext));

    /* Generate AES keys */
    generateAesKey(&m_aesKey, &m_aesIv);
}

EncryptTool::~EncryptTool()
{
    EVP_PKEY_free(m_localKeypair);
    EVP_PKEY_free(m_remoteKeypair);

    EVP_CIPHER_CTX_free(m_rsaEncryptContext);
    EVP_CIPHER_CTX_free(m_aesEncryptContext);

    EVP_CIPHER_CTX_free(m_rsaDecryptContext);
    EVP_CIPHER_CTX_free(m_aesDecryptContext);

    free(m_aesKey);
    free(m_aesIv);
}

int EncryptTool::generateRsaKeypair(EVP_PKEY** keypair)
{
    EVP_PKEY_CTX* context = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);

    if(EVP_PKEY_keygen_init(context) <= 0) return FAILURE;
    if(EVP_PKEY_CTX_set_rsa_keygen_bits(context, RSA_KEYLEN) <= 0) return FAILURE;
    if(EVP_PKEY_keygen(context, keypair) <= 0) return FAILURE;

    EVP_PKEY_CTX_free(context);

    return SUCCESS;
}

int EncryptTool::generateAesKey(unsigned char** aesKey, unsigned char** aesIv)
{
    *aesKey = reinterpret_cast<unsigned char*>(malloc(m_aesKeyLength));
    *aesIv = reinterpret_cast<unsigned char*>(malloc(m_aesIvLength));

    if(aesKey == nullptr || aesIv == nullptr) return FAILURE;

    /* For the AES key we have the option of using a PBKDF or just using straight random
       data for the key and IV. Depending on your use case, you will want to pick one or another. */
    if(RAND_bytes(*aesKey, static_cast<int>(m_aesKeyLength)) == 0) return FAILURE;
    if(RAND_bytes(*aesIv, static_cast<int>(m_aesIvLength)) == 0) return FAILURE;

    return SUCCESS;
}

int EncryptTool::rsaEncrypt(const unsigned char* message, size_t messageLength,
                       unsigned char** encryptedMessage, unsigned char** encryptedKey,
                       size_t* encryptedKeyLength, unsigned char** iv, size_t* ivLength)
{
    /* Allocate memory for everything */
    size_t encryptedMessageLength = 0;
    size_t blockLength = 0;

    *encryptedKey = reinterpret_cast<unsigned char*>(malloc(static_cast<unsigned int>(EVP_PKEY_size(m_remoteKeypair))));
    *iv = reinterpret_cast<unsigned char*>(malloc(EVP_MAX_IV_LENGTH));
    *ivLength = EVP_MAX_IV_LENGTH;

    if(*encryptedKey == nullptr || *iv == nullptr) return FAILURE;

    *encryptedMessage = reinterpret_cast<unsigned char*>(malloc(messageLength + EVP_MAX_IV_LENGTH));
    if(encryptedMessage == nullptr) return FAILURE;

    /* Encrypt it! */
    if(m_remoteKeypair == nullptr) return FAILURE;
    if (!EVP_SealInit(m_rsaEncryptContext, EVP_aes_256_cbc(),
                      encryptedKey, reinterpret_cast<int*>(encryptedKeyLength),
                      *iv, &m_remoteKeypair, 1)) {
        return FAILURE;
    }

    if (!EVP_SealUpdate(m_rsaEncryptContext, *encryptedMessage + encryptedMessageLength,
                        reinterpret_cast<int*>(&blockLength), reinterpret_cast<const unsigned char*>(message),
                        static_cast<int>(messageLength))) {
        return FAILURE;
    }
    encryptedMessageLength += blockLength;

    if(!EVP_SealFinal(m_rsaEncryptContext, *encryptedMessage + encryptedMessageLength, reinterpret_cast<int*>(&blockLength))) {
        return FAILURE;
    }
    encryptedMessageLength += blockLength;

    return static_cast<int>(encryptedMessageLength);
}

int EncryptTool::rsaDecrypt(unsigned char* encryptedMessage, size_t encryptedMessageLength,
                       unsigned char* encryptedKey, size_t encryptedKeyLength,
                       unsigned char* iv, size_t ivLength, unsigned char** decryptedMessage)
{
    /* Allocate memory for everything */
    size_t decryptedMessageLength = 0;
    size_t blockLength = 0;

    *decryptedMessage = reinterpret_cast<unsigned char*>(malloc(encryptedMessageLength + ivLength));
    if(*decryptedMessage == nullptr) return FAILURE;

    /* Decrypt it! */
    if (!EVP_OpenInit(m_rsaDecryptContext, EVP_aes_256_cbc(), encryptedKey, static_cast<int>(encryptedKeyLength), iv, m_localKeypair)) {
        return FAILURE;
    }
    if (!EVP_OpenUpdate(m_rsaDecryptContext, reinterpret_cast<unsigned char*>(*decryptedMessage + decryptedMessageLength),
                        reinterpret_cast<int*>(&blockLength), encryptedMessage, static_cast<int>(encryptedMessageLength))) {
        return FAILURE;
    }
    decryptedMessageLength += blockLength;

    if (!EVP_OpenFinal(m_rsaDecryptContext, reinterpret_cast<unsigned char*>(*decryptedMessage + decryptedMessageLength),
                       reinterpret_cast<int*>(&blockLength))) {
        return FAILURE;
    }
    decryptedMessageLength += blockLength;

    return static_cast<int>(decryptedMessageLength);
}

int EncryptTool::setRemotePublicKey(unsigned char* publicKey, size_t publicKeyLength)
{
    BIO* bio = BIO_new(BIO_s_mem());

    if(BIO_write(bio, publicKey, static_cast<int>(publicKeyLength)) != static_cast<int>(publicKeyLength)) {
        return FAILURE;
    }

    PEM_read_bio_PUBKEY(bio, &m_remoteKeypair, nullptr, nullptr);
    BIO_free_all(bio);

    return SUCCESS;
}

int EncryptTool::setRemotePrivateKey(unsigned char* privateKey, size_t privateKeyLength)
{
    BIO* bio = BIO_new(BIO_s_mem());

    if(BIO_write(bio, privateKey, static_cast<int>(privateKeyLength)) != static_cast<int>(privateKeyLength)) {
        return FAILURE;
    }

    PEM_read_bio_PrivateKey(bio, &m_remoteKeypair, nullptr, nullptr);
    BIO_free_all(bio);

    return SUCCESS;
}

int EncryptTool::setLocalPublicKey(unsigned char* publicKey, size_t publicKeyLength)
{
      BIO* bio = BIO_new(BIO_s_mem());

      if(BIO_write(bio, publicKey, static_cast<int>(publicKeyLength)) != static_cast<int>(publicKeyLength)) {
          return FAILURE;
      }

      PEM_read_bio_PUBKEY(bio, &m_localKeypair, nullptr, nullptr);
      BIO_free_all(bio);

      return SUCCESS;
}

int EncryptTool::setLocalPrivateKey(unsigned char* privateKey, size_t privateKeyLength)
{
    BIO* bio = BIO_new(BIO_s_mem());

    if(BIO_write(bio, privateKey, static_cast<int>(privateKeyLength)) != static_cast<int>(privateKeyLength)) {
        return FAILURE;
    }

    PEM_read_bio_PrivateKey(bio, &m_localKeypair, nullptr, nullptr);
    BIO_free_all(bio);

    return SUCCESS;
}

int EncryptTool::writeKeyToFile(FILE* file, int key)
{
    switch(key) {
        case KEY_SERVER_PRI:
        if (!PEM_write_PrivateKey(file, m_localKeypair, nullptr, nullptr, 0, nullptr, nullptr)) {
            return FAILURE;
        }
        break;

        case KEY_SERVER_PUB:
        if (!PEM_write_PUBKEY(file, m_localKeypair)) {
            return FAILURE;
        }
        break;

        case KEY_CLIENT_PUB:
        if (!PEM_write_PUBKEY(file, m_remoteKeypair)) {
            return FAILURE;
        }
        break;

        case KEY_CLIENT_PRI:
        if (!PEM_write_PrivateKey(file, m_remoteKeypair, nullptr, nullptr, 0, nullptr, nullptr)) {
            return FAILURE;
        }
        break;

        case KEY_AES:
        fwrite(m_aesKey, 1, m_aesKeyLength* 8, file);
        break;

        case KEY_AES_IV:
        fwrite(m_aesIv, 1, m_aesIvLength* 8, file);
        break;

        default:
        return FAILURE;
    }

    return SUCCESS;
}

int EncryptTool::bioToString(BIO* bio, unsigned char** string)
{
    size_t bioLength = static_cast<unsigned int>(BIO_pending(bio));
    *string = reinterpret_cast<unsigned char*>(malloc(bioLength + 1));

    if(string == nullptr) return FAILURE;

    BIO_read(bio, *string, static_cast<int>(bioLength));

    /* Insert the NUL terminator */
    (*string)[bioLength] = '\0';

    BIO_free_all(bio);

    return static_cast<int>(bioLength);
}

void EncryptTool::generateRsa()
{
    generateRsaKeypair(&m_localKeypair);
}

int EncryptTool::readFile(char* filename, unsigned char** file)
{
    FILE* fd = fopen(filename, "rb");
    if(fd == nullptr) {
        qDebug() << "Failed to open file: " << strerror(errno);
        return -1;
    }

    /* Determine size of the file */
    fseek(fd, 0, SEEK_END);
    size_t fileLength = static_cast<unsigned int>(ftell(fd));
    fseek(fd, 0, SEEK_SET);

    /* Allocate space for the file */
    *file = reinterpret_cast<unsigned char*>(malloc(fileLength));
    if(*file == nullptr) {
        qDebug() << "Failed to allocate memory";
        fclose(fd);
        return -1;
    }

    /* Read the file into the buffer */
    size_t bytesRead = fread(*file, 1, fileLength, fd);

    if(bytesRead != fileLength) {
        qDebug() << "Error reading file";
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return static_cast<int>(fileLength);
}

int EncryptTool::writeFile(char* filename, unsigned char* file, size_t fileLength)
{
    FILE* fd = fopen(filename, "wb");
    if(fd == nullptr) {
        qDebug() << "Failed to open file: " << strerror(errno);
        return -1;
    }

    size_t bytesWritten = fwrite(file, 1, fileLength, fd);

    if (bytesWritten != fileLength) {
        qDebug() << "Error writing file";
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return static_cast<int>(bytesWritten);
}

bool EncryptTool::signRsa(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen, EVP_PKEY* pkey)
{
    bool result = false;

    EVP_MD_CTX* context = EVP_MD_CTX_create();

    if(context != nullptr) {
        const EVP_MD* md = EVP_get_digestbyname("SHA256");
        if(md != nullptr) {
            if(EVP_DigestInit_ex(context, md, nullptr)) {
                if(EVP_DigestSignInit(context, nullptr, md, nullptr, pkey)) {
                    if(EVP_DigestSignUpdate(context, msg, mlen)) {
                        size_t req = 0;
                        if(EVP_DigestSignFinal(context, nullptr, &req)) {
                            if(req > 0) {
                                *sig = reinterpret_cast<unsigned char*>(OPENSSL_malloc(req));
                                if(*sig != nullptr) {
                                    *slen = req;
                                    if(EVP_DigestSignFinal(context, reinterpret_cast<unsigned char*>(*sig), slen)) {
                                        result = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        EVP_MD_CTX_destroy(context);
    }
    return result;
}

bool EncryptTool::verifyRsa(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen, EVP_PKEY* pkey)
{
    bool result = false;

    EVP_MD_CTX* context = EVP_MD_CTX_create();

    if(context != nullptr) {
        const EVP_MD* md = EVP_get_digestbyname("SHA256");
        if(md != nullptr) {
            if(EVP_DigestInit_ex(context, md, nullptr)) {
                if(EVP_DigestVerifyInit(context, nullptr, md, nullptr, pkey)) {
                   if(EVP_DigestVerifyUpdate(context, msg, mlen)) {
                        ERR_clear_error(); /* Clear any errors for the call below */
                        if(EVP_DigestVerifyFinal(context, sig, slen)) {
                            result = true;
                        }
                    }
                }
            }
        }
        EVP_MD_CTX_destroy(context);
    }
    return result;
}

bool EncryptTool::signHash(const unsigned char* msg, size_t mlen, unsigned char** sig, size_t* slen)
{
    OpenSSL_add_all_algorithms();
    return signRsa(msg, mlen, sig, slen, m_localKeypair);
}

bool EncryptTool::verifyHash(const unsigned char* msg, size_t mlen, const unsigned char* sig, size_t slen)
{
    return  verifyRsa(msg, mlen, sig, slen, m_remoteKeypair);
}

bool EncryptTool::computeHash(char* unhashed, size_t unhashedSiz, unsigned char** hashed, size_t* hashedSiz, int type)
{
    bool success = false;

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if(context != nullptr) {
        if (type == 0) { // SHA256 hash (longer)
            if(EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
                if(EVP_DigestUpdate(context, unhashed, unhashedSiz)) {
                    *hashed = reinterpret_cast<unsigned char*>(OPENSSL_malloc(EVP_MAX_MD_SIZE));
                    if(*hashed != nullptr) {
                        if(EVP_DigestFinal_ex(context, *hashed, reinterpret_cast<unsigned int*>(hashedSiz))) {
                            success = true;
                        }
                    }
                }
            }
        } else { // MD5 hash (shorter)
            if(EVP_DigestInit_ex(context, EVP_md5(), nullptr)) {
                if(EVP_DigestUpdate(context, unhashed, unhashedSiz)) {
                    *hashed = reinterpret_cast<unsigned char*>(OPENSSL_malloc(EVP_MAX_MD_SIZE));
                    if(*hashed != nullptr) {
                        if(EVP_DigestFinal_ex(context, *hashed, reinterpret_cast<unsigned int*>(hashedSiz))) {
                            success = true;
                        }
                    }
                }
            }
        }
        EVP_MD_CTX_free(context);
    }
    return success;
}
