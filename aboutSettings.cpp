#include "aboutSettings.h"
#include "mainWindow.h"
#include "ui_mainWindow.h"
#include "encryptTool.h"

#include <QDir>
#include <QDebug>

AboutSettings::AboutSettings(QObject* parent) : QObject(parent)
{
    m_mainWindow = reinterpret_cast<MainWindow*>(parent);
    m_gui = m_mainWindow->getGUI();
    m_encryptTool = m_mainWindow->getEncryptTool();
    m_process1 = new QProcess(this);
    m_process2 = new QProcess(this);

    QDir mediaDir;
    mediaDir.mkdir(QDir::homePath() + "/PandoraContacts");
    m_baseDir = QDir::homePath() + "/PandoraContacts";

    QPixmap aPixmap(":/img/pandora.png");
    m_gui->aboutPicLabel->setAttribute(Qt::WA_TranslucentBackground);
    m_gui->aboutPicLabel->setPixmap(aPixmap);

    QRegularExpression rxv("[A-Za-z0-9-_*]{0,20}");
    QValidator* validator = new QRegularExpressionValidator(rxv, this);
    m_gui->passwordLEdit->setValidator(validator);

    connect(m_gui->passwordButn, &QPushButton::clicked, this, &AboutSettings::applySettings);
    connect(m_process1, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(loadLocalKeys(int, QProcess::ExitStatus)));
    connect(m_process2, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(saveLocalKeys(int, QProcess::ExitStatus)));
}

AboutSettings::~AboutSettings()
{
}

void AboutSettings::applySettings()
{
    QByteArray path1, path2;
    bool exPath1, exPath2;
    QString srcfname, dstfname;

    path1 = m_baseDir.toUtf8() + "/localPub.txt";
    path2 = m_baseDir.toUtf8() + "/localPriv.enc";;
    exPath1 = QFileInfo::exists(path1);
    exPath2 = QFileInfo::exists(path2);
    if (exPath1 && exPath2) {

        srcfname = m_baseDir + "/localPub.txt";
        dstfname = "/mnt/ramdisk/localPub.txt";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QFile::copy(srcfname, dstfname);
        srcfname = m_baseDir + "/localPriv.enc";
        dstfname = "/mnt/ramdisk/localPriv.enc";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QFile::copy(srcfname, dstfname);

        dstfname = "/mnt/ramdisk/localPriv.txt";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QString passw = m_gui->passwordLEdit->text();
        QString file = "openssl enc -d -pbkdf2 -aes-256-cbc -in /mnt/ramdisk/localPriv.enc -out /mnt/ramdisk/localPriv.txt -k " + passw;
        m_process1->start(file);
    } else {

        generateLocalKeys();
        dstfname = "/mnt/ramdisk/localPriv.enc";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QString passw = m_gui->passwordLEdit->text();
        QString file = "openssl enc -pbkdf2 -aes-256-cbc -in /mnt/ramdisk/localPriv.txt -out /mnt/ramdisk/localPriv.enc -k " + passw;
        m_process2->start(file);
    }

    int result;
    unsigned char* message = nullptr;

    path1 = "/mnt/ramdisk/localPub.txt";
    result = m_encryptTool->readFile(path1.data(), &message);
    if (result == -1) {
        qDebug() << "Failed to load localPub data";
        if (message != nullptr) free(message);
        m_mainWindow->setSettingOffFunction();
        return;
    }
    size_t messageLength = static_cast<unsigned int>(result);
    unsigned char* hashed = nullptr;
    size_t hashedSiz = 0;

    if (!m_encryptTool->computeHash(reinterpret_cast<char*>(message), messageLength, &hashed, &hashedSiz, 1)) {
        qDebug() << "Signature failed";
        if (message != nullptr)          free(message);
        if (hashed != nullptr)           OPENSSL_free(hashed);
        m_mainWindow->setSettingOffFunction();
        return;
    }

    QByteArray rfname = QByteArray::fromRawData(reinterpret_cast<const char*>(hashed), hashedSiz);
    m_pKeyHash.clear();
    for (int idx=0; idx<rfname.size(); idx++) {
        int numb = rfname.at(idx) & 0xff;
        m_pKeyHash.append(QString::number(numb, 16).toUpper());
    }

    /* Clean up */
    if (message != nullptr)          free(message);
    if (hashed != nullptr)           OPENSSL_free(hashed);
}

void AboutSettings::loadLocalKeys(int retCod, QProcess::ExitStatus)
{
    int result;
    QByteArray path;

    if (retCod) {
        m_mainWindow->setSettingOffFunction();
    } else {
        /* Read the local public key */
        unsigned char* file1 = nullptr;
        path = "/mnt/ramdisk/localPub.txt";
        result = m_encryptTool->readFile(path.data(), &file1);
        if (result == -1) {
            qDebug() << "Failed to load localPub.txt keys";
            if (file1 != nullptr) free(file1);
            m_mainWindow->setSettingOffFunction();
            return;
        }
        size_t fileLength1 = static_cast<unsigned int>(result);
        m_encryptTool->setLocalPublicKey(file1, fileLength1);

        /* Read the local private key */
        unsigned char* file0 = nullptr;
        path = "/mnt/ramdisk/localPriv.txt";
        result = m_encryptTool->readFile(path.data(), &file0);
        if (result == -1) {
            qDebug() << "Failed to load localPriv.txt keys";
            if (file0 != nullptr) free(file0);
            if (file1 != nullptr) free(file1);
            m_mainWindow->setSettingOffFunction();
            return;
        }
        size_t fileLength0 = static_cast<unsigned int>(result);
        m_encryptTool->setLocalPrivateKey(file0, fileLength0);

        if (file0 != nullptr) free(file0);
        if (file1 != nullptr) free(file1);

        QFile::remove("/mnt/ramdisk/localPub.txt");
        QFile::remove("/mnt/ramdisk/localPriv.txt");
        QFile::remove("/mnt/ramdisk/localPriv.enc");

        m_gui->selAudioButn->setEnabled(true);
        m_gui->selTextButn->setEnabled(true);
        m_gui->selContactsButn->setEnabled(true);
        m_gui->selMessagesButn->setEnabled(true);
        m_gui->selSettingsButn->setEnabled(true);
        m_gui->setSerialButn->setEnabled(true);
        m_gui->setAboutButn->setEnabled(true);
        m_gui->passwordButn->setHidden(true);
        m_gui->passwordLabel->setHidden(true);
        m_gui->passwordLEdit->setHidden(true);
    }
}

void AboutSettings::generateLocalKeys()
{
    QByteArray path;

    path = "/mnt/ramdisk/localPriv.txt";
    FILE* fd0 = fopen(path.data(), "wb");
    if(fd0 == nullptr) {
        qDebug() << "Failed to open localPriv.txt file: " << strerror(errno);
        m_mainWindow->setSettingOffFunction();
        return;
    }
    path = "/mnt/ramdisk/localPub.txt";
    FILE* fd1 = fopen(path.data(), "wb");
    if(fd1 == nullptr) {
        qDebug() << "Failed to open localPub.txt file: " << strerror(errno);
        fclose(fd0);
        m_mainWindow->setSettingOffFunction();
        return;
    }

    m_encryptTool->generateRsa();
    m_encryptTool->writeKeyToFile(fd0, KEY_SERVER_PRI);
    m_encryptTool->writeKeyToFile(fd1, KEY_SERVER_PUB);

    fclose(fd0);
    fclose(fd1);
}

void AboutSettings::saveLocalKeys(int retCod, QProcess::ExitStatus)
{
    if (retCod) {
        m_mainWindow->setSettingOffFunction();
    } else {
        QString srcfname = "/mnt/ramdisk/localPub.txt";
        QString dstfname = m_baseDir + "/localPub.txt";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QFile::copy(srcfname, dstfname);
        srcfname = "/mnt/ramdisk/localPriv.enc";
        dstfname = m_baseDir + "/localPriv.enc";
        if (QFile::exists(dstfname)) QFile::remove(dstfname);
        QFile::copy(srcfname, dstfname);

        QFile::remove("/mnt/ramdisk/localPub.txt");
        QFile::remove("/mnt/ramdisk/localPriv.txt");
        QFile::remove("/mnt/ramdisk/localPriv.enc");

        m_gui->selAudioButn->setEnabled(true);
        m_gui->selTextButn->setEnabled(true);
        m_gui->selContactsButn->setEnabled(true);
        m_gui->selMessagesButn->setEnabled(true);
        m_gui->selSettingsButn->setEnabled(true);
        m_gui->setSerialButn->setEnabled(true);
        m_gui->setAboutButn->setEnabled(true);
        m_gui->passwordButn->setHidden(true);
        m_gui->passwordLabel->setHidden(true);
        m_gui->passwordLEdit->setHidden(true);
    }
}

QString AboutSettings::getOwnPKeyHash()
{
    return m_pKeyHash;
}
