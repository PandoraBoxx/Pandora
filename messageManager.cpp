#include "messageManager.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "encryptTool.h"
#include "serialInterface.h"
#include "progressBar.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include <QDebug>
#include <QDateTime>

MessageManager::MessageManager(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();
    m_encryptTool = m_mainWindow->getEncryptTool();
    m_serialInterface = m_mainWindow->getSerialInterface();
    m_timerRec = new QTimer(this);
    m_timerSend = new QTimer(this);

    m_baseDir = QDir::currentPath();
    m_receivingStatus = false;

    ProgressBar* msgRemoteSize = new ProgressBar(m_gui->msgReceivePage);
    m_msgRemoteSizeList.append(msgRemoteSize);
    m_gui->msgRecProgressHLayout->addWidget(msgRemoteSize);

    ProgressBar* msgLocalSize = new ProgressBar(m_gui->msgSendPage);
    m_msgLocalSizeList.append(msgLocalSize);
    m_gui->msgSendProgressHLayout->addWidget(msgLocalSize);

    connect(m_gui->msgTransmitButn, &QPushButton::clicked, this, &MessageManager::transmitMessage);
    connect(m_gui->msgGroupCbBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MessageManager::groupSelected);
    connect(m_serialInterface, &SerialInterface::msgRemoteLengthSignal, this, &MessageManager::remoteMessageProgress);
    connect(m_serialInterface, &SerialInterface::msgLocalLengthSignal, this, &MessageManager::localMessageProgress);
    connect(m_serialInterface, &SerialInterface::msgReceivedSignal, this, &MessageManager::messageDataReceived);
    connect(m_serialInterface, &SerialInterface::msgSentSignal, this, &MessageManager::messageDataSent);
    connect(m_gui->mainStackedWidget, &QStackedWidget::currentChanged, this, &MessageManager::pageChanged);
    connect(m_gui->msgStackedWidget, &QStackedWidget::currentChanged, this, &MessageManager::pageChanged);
    connect(m_timerRec, &QTimer::timeout, this, &MessageManager::checkMsgReceived);
    connect(m_timerSend, &QTimer::timeout, this, &MessageManager::checkMsgSent);
}

MessageManager::~MessageManager()
{
}

void MessageManager::messageDataSent()
{
    m_gui->msgSendResultLabel->setText("The local message was sent.");
    m_timerSend->start(2000);
}

void MessageManager::checkMsgSent()
{
    if (m_gui->msgSendPage->isVisible()) {
        m_sendingStatus = true;
        updateContactList();
        m_gui->msgSendResultLabel->setText("Ready to send a new message.");
    }

    m_msgLocalSizeList.at(0)->setLevel(0);
    m_timerSend->stop();
}

void MessageManager::pageChanged()
{
    if (m_gui->msgReceivePage->isVisible()) {
        m_serialInterface->recvMessage();
        m_receivingStatus = true;
        m_gui->msgRecResultLabel->setText("Ready to receive a new message.");
    } else {
        if (m_receivingStatus) {
            m_serialInterface->stopReceiving();
            m_receivingStatus = false;
        }
    }

    if (m_gui->msgSendPage->isVisible()) {
        m_sendingStatus = true;
        updateContactList();
        m_gui->msgSendResultLabel->setText("Ready to send a new message.");
    } else {
        if (m_sendingStatus) {
            m_sendingStatus = false;
        }
    }
}

void MessageManager::remoteMessageProgress(qreal length, QString type)
{
    m_msgRemoteSizeList.at(0)->setLevel(length);
    if (length < 1.0F) {
        m_gui->statusBar->showMessage("Receiving " + type + "...");
    } else {
        m_gui->statusBar->showMessage(type + " Received");
    }
}

void MessageManager::localMessageProgress(qreal length, QString type)
{
    m_msgLocalSizeList.at(0)->setLevel(length);
    if (length < 1.0F) {
        m_gui->statusBar->showMessage("Sending " + type + "...");
    } else {
        m_gui->statusBar->showMessage(type + " Sent");
    }
}

void MessageManager::transmitMessage()
{
    QString nBox = m_gui->msgNameCbBox->currentText();
    QString gBox = m_gui->msgGroupCbBox->currentText();
    QByteArray folder;
    QString date;

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    QString ftext = "select ContactDir, ContactDate from ContactTable where ContactName='" + nBox + "' and ContactGroup='" + gBox + "'";
    query.exec(ftext);
    if (query.next()) {
        folder = query.value(0).toByteArray();
        date = query.value(1).toString();
        unsigned char* file = nullptr;
        int result = m_encryptTool->readFile(folder.data(), &file);
        if (result == -1) {
            qDebug() << "Failed to load remote public key";
            if (file != nullptr) free(file);
            return;
        }
        size_t fileLength = static_cast<unsigned int>(result);
        m_encryptTool->setRemotePublicKey(file, fileLength);
        if (file != nullptr) free(file);
    } else {
        return;
    }

    /* Get the message to encrypt */
    int result;
    unsigned char* message = nullptr;
    QByteArray path;

    path = m_baseDir.toUtf8() + "/audioLocal.org";
    result = m_encryptTool->readFile(path.data(), &message);
    if (result == -1) {
        qDebug() << "Failed to load audioLocal.org data";
        if (message != nullptr) free(message);
        return;
    }

    size_t messageLength = static_cast<unsigned int>(result);

    /* Encrypt the message with RSA +1 on the string length
    argument because we want to encrypt the NUL terminator too */
    unsigned char* encryptedMessage = nullptr;
    unsigned char* encryptedKey     = nullptr;
    unsigned char* iv               = nullptr;
    size_t encryptedKeyLength;
    size_t ivLength;

    int encryptedMessageLength = m_encryptTool->rsaEncrypt(message, messageLength, &encryptedMessage,
                                                      &encryptedKey, &encryptedKeyLength, &iv, &ivLength);
    if(encryptedMessageLength == -1) {
        qDebug() << "Encryption failed";
        if (message != nullptr)          free(message);
        if (encryptedMessage != nullptr) free(encryptedMessage);
        if (encryptedKey != nullptr)     free(encryptedKey);
        if (iv != nullptr)               free(iv);
        return;
    }

    unsigned char* sig = nullptr;
    size_t slen = 0;
    unsigned char* hashed = nullptr;
    size_t hashedSiz = 0;

    if (m_encryptTool->computeHash(reinterpret_cast<char*>(message), messageLength, &hashed, &hashedSiz, 0)) {
        if(!m_encryptTool->signHash(hashed, hashedSiz, &sig, &slen)) {
            qDebug() << "Hash failed";
            if (message != nullptr)          free(message);
            if (encryptedMessage != nullptr) free(encryptedMessage);
            if (encryptedKey != nullptr)     free(encryptedKey);
            if (iv != nullptr)               free(iv);
            if (sig != nullptr)              OPENSSL_free(sig);
            if (hashed != nullptr)           OPENSSL_free(hashed);
            return;
        }
    } else {
        qDebug() << "Signature failed";
        if (message != nullptr)          free(message);
        if (encryptedMessage != nullptr) free(encryptedMessage);
        if (encryptedKey != nullptr)     free(encryptedKey);
        if (iv != nullptr)               free(iv);
        if (sig != nullptr)              OPENSSL_free(sig);
        if (hashed != nullptr)           OPENSSL_free(hashed);
        return;
    }

    qint32 encMessageLength = encryptedMessageLength;
    QByteArray encMessage = QByteArray::fromRawData(reinterpret_cast<const char*>(encryptedMessage), encryptedMessageLength);
    qint32 encKeyLength = static_cast<qint32>(encryptedKeyLength);
    QByteArray encKey = QByteArray::fromRawData(reinterpret_cast<const char*>(encryptedKey), encKeyLength);
    qint32 encIvLength = static_cast<qint32>(ivLength);
    QByteArray encIv = QByteArray::fromRawData(reinterpret_cast<const char*>(iv), encIvLength);
    qint32 encSignLen = static_cast<qint32>(slen);
    QByteArray encSign = QByteArray::fromRawData(reinterpret_cast<const char*>(sig), encSignLen);

    QFileInfo info(QDir::currentPath() + "/audioLocal.org");
    QByteArray fdate = info.lastModified().toString().toUtf8();

    QFile fileOut(m_baseDir + "/audioLocal.enc");
    fileOut.open(QIODevice::WriteOnly);
    if (!fileOut.isOpen()) {
        qDebug() << "File audioLocal.enc open for write failed";
        if (message != nullptr)          free(message);
        if (encryptedMessage != nullptr) free(encryptedMessage);
        if (encryptedKey != nullptr)     free(encryptedKey);
        if (iv != nullptr)               free(iv);
        if (sig != nullptr)              OPENSSL_free(sig);
        if (hashed != nullptr)           OPENSSL_free(hashed);
        return;
    } else {
        QDataStream out(&fileOut);
        QString pKeyHash = m_mainWindow->getOwnPKeyHash();
        out << pKeyHash << fdate << encMessage << encMessageLength << encKey << encKeyLength << encIv << encIvLength << encSign << encSignLen;
        fileOut.close();
    }

    /* Clean up */
    if (message != nullptr)          free(message);
    if (encryptedMessage != nullptr) free(encryptedMessage);
    if (encryptedKey != nullptr)     free(encryptedKey);
    if (iv != nullptr)               free(iv);
    if (sig != nullptr)              OPENSSL_free(sig);
    if (hashed != nullptr)           OPENSSL_free(hashed);

    m_serialInterface->sendMessage();
}

void MessageManager::groupSelected(int idx)
{
    QString text, name, ftext;

    if (idx == -1)
        return;

    m_gui->msgNameCbBox->clear();
    m_gui->msgSendResultLabel->clear();

    text = m_gui->msgGroupCbBox->itemText(idx);
    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    ftext = "select ContactName from ContactTable where ContactGroup='" + text + "'";
    query.exec(ftext);
    while (query.next()) {
        name = query.value(0).toString();
        m_gui->msgNameCbBox->addItem(QIcon(), name);
    }
}

void MessageManager::updateContactList()
{
    QString name;

    m_gui->msgNameCbBox->clear();
    m_gui->msgGroupCbBox->clear();

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    query.exec("select DISTINCT ContactGroup from ContactTable");
    while (query.next()) {
        name = query.value(0).toString();
        m_gui->msgGroupCbBox->addItem(QIcon(), name);
    }
}

void MessageManager::messageDataReceived()
{
    QByteArray encMessage, encKey, encIv, encSign, path, fdate;
    QString contactInfo;
    qint32 encMessageLength, encKeyLength, encIvLength, encSignLen;

    QFile fileIn(m_baseDir + "/audioRemote.enc");
    fileIn.open(QIODevice::ReadOnly);
    if (!fileIn.isOpen()) {
        qDebug() << "File audioRemote.enc open for read failed";
        return;
    } else {
        QDataStream in(&fileIn);
        in >> contactInfo >> fdate >> encMessage >> encMessageLength >> encKey >> encKeyLength >> encIv >> encIvLength >> encSign >> encSignLen;
        fileIn.close();
    }

    QDir contDir;
    QByteArray name, group, folder;
    contDir.mkdir(QDir::currentPath() + "/Contacts");
    QString dstfname = QDir::currentPath() + "/Contacts/" + contactInfo;

    QSqlDatabase db = QSqlDatabase::database("ContactDatabase");
    QSqlQuery query(db);
    QString ftext = "select ContactName, ContactGroup, ContactDir from ContactTable where (ContactDir like '%" + dstfname + "%')";
    query.exec(ftext);
    if (query.next()) {
        name = query.value(0).toByteArray();
        group = query.value(1).toByteArray();
        folder = query.value(2).toByteArray();

        unsigned char* file = nullptr;
        int result = m_encryptTool->readFile(folder.data(), &file);
        if (result == -1) {
            qDebug() << "Failed to load contact public key";
            if (file != nullptr) free(file);
            return;
        }
        size_t fileLength = static_cast<unsigned int>(result);
        m_encryptTool->setRemotePublicKey(file, fileLength);
        if (file != nullptr) free(file);
    }

    /* Decrypt the message */
    char* decryptedMessage = nullptr;

    int decryptedMessageLength = m_encryptTool->rsaDecrypt(reinterpret_cast<unsigned char*>(encMessage.data()), static_cast<unsigned int>(encMessageLength),
                                                      reinterpret_cast<unsigned char*>(encKey.data()), static_cast<unsigned int>(encKeyLength),
                                                      reinterpret_cast<unsigned char*>(encIv.data()), static_cast<unsigned int>(encIvLength),
                                                      reinterpret_cast<unsigned char**>(&decryptedMessage));

    unsigned char* hashedD = nullptr;
    size_t hashedDSiz = 0;
    int result;

    if (decryptedMessageLength == -1) {
        qDebug() << "Decryption failed\n";
        if (decryptedMessage != nullptr) free(decryptedMessage);
        return;
    } else {
        if (m_encryptTool->computeHash(decryptedMessage, static_cast<unsigned int>(decryptedMessageLength), &hashedD, &hashedDSiz, 0)) {
            if(m_encryptTool->verifyHash(hashedD, hashedDSiz, reinterpret_cast<unsigned char*>(encSign.data()), static_cast<unsigned int>(encSignLen))) {
                path = m_baseDir.toUtf8() + "/audioRemote.dec";
                result = m_encryptTool->writeFile(path.data(), reinterpret_cast<unsigned char*>(decryptedMessage), static_cast<unsigned int>(decryptedMessageLength));
                if (result == -1) {
                    qDebug() << "Failed to write audioRemote.dec file";
                    if (decryptedMessage != nullptr) free(decryptedMessage);
                    if (hashedD != nullptr)          OPENSSL_free(hashedD);
                    return;
                }
            } else {
                qDebug() << "Verification failed";
                if (decryptedMessage != nullptr) free(decryptedMessage);
                if (hashedD != nullptr)          OPENSSL_free(hashedD);
                return;
            }
        } else {
            qDebug() << "Hash failed";
            if (decryptedMessage != nullptr) free(decryptedMessage);
            if (hashedD != nullptr)          OPENSSL_free(hashedD);
            return;
        }
    }

    /* Clean up */
    if (decryptedMessage != nullptr) free(decryptedMessage);
    if (hashedD != nullptr)          OPENSSL_free(hashedD);

    m_gui->msgRecResultLabel->setText("The remote message was received.");
    m_timerRec->start(2000);

    return;
}

void MessageManager::checkMsgReceived()
{
    if (m_gui->msgReceivePage->isVisible()) {
        m_serialInterface->recvMessage();
        m_receivingStatus = true;
        m_gui->msgRecResultLabel->setText("Ready to receive a new message.");
    }

    m_msgRemoteSizeList.at(0)->setLevel(0);
    m_timerRec->stop();
}
